#include <Chip8/Board.h>
#include <Chip8/Cpu.h>
#include <Chip8/Memory.h>
#include <Chip8/Video.h>
#include <functional>

#include "../src/debugger.h"
#include "../src/sdlvideo.h"

#include <gtest/gtest.h>

namespace {
std::shared_ptr<Chip8::Board> g_board;

class Chip8Test : public ::testing::Test {
public:
  static void SetUpTestCase() {
    g_board =
        std::make_shared<Chip8::Board>(std::make_shared<Chip8::SDLVideo>());
  }

  static void TearDownTestCase() { g_board.reset(); }

  static void for_all_regs(const std::function<void(std::uint8_t)> &lambda) {
    for (std::uint8_t reg = 0; reg <= 0xF; reg++) {
      lambda(reg);
      if (HasFatalFailure())
        return;
    }
  }

  static void
  for_all_8bit_values(const std::function<void(std::uint8_t)> &lambda) {
    for (std::uint16_t reg = 0; reg <= 0xFF; reg++) {
      lambda(static_cast<std::uint8_t>(reg));
      if (HasFatalFailure())
        return;
    }
  }

  static void for_all_regs_8bit_pairs(
      const std::function<void(std::uint8_t, std::uint8_t, std::uint8_t)>
          &lambda) {
    for_all_regs([&](std::uint8_t reg) {
      for_all_8bit_values([&](std::uint8_t nn) {
        for_all_8bit_values(
            [&](std::uint8_t nn_orig) { lambda(reg, nn, nn_orig); });
      });
    });
  }

  void simple_XKK_test(
      uint8_t code,
      const std::function<uint8_t(uint8_t, uint8_t, uint8_t)> &getExpected) {
    for_all_regs_8bit_pairs([&](const std::uint8_t reg, const std::uint8_t nn,
                                const std::uint8_t nn_orig) {
      board()->reset();
      board()->LoadBinary({std::uint8_t(code | reg), nn});
      ASSERT_EQ(Chip8::ResultType::Ok, board()->cpu()->setVx(reg, nn_orig));
      board()->step();
      uint8_t out;
      uint8_t expected = getExpected(reg, nn, nn_orig);
      ASSERT_EQ(Chip8::ResultType::Ok, board()->cpu()->Vx(reg, out));
      ASSERT_EQ(expected, out) << "Reg: V" << (int)reg << " Val: " << (int)nn
                               << " OrigVal: " << (int)nn_orig;
      // Chip8::Debugger(board).dumpDisasm(Chip8::ProgramStartLocation, 4);
    });
  }

protected:
  virtual void SetUp() { g_board->reset(); }
  virtual void TearDown() {}

  virtual Chip8::Board *board() { return g_board.get(); }
  virtual std::shared_ptr<Chip8::Board> board_sp() { return g_board; }
};

// using Chip8Test = Chip8TestBase<::testing::Test>;

// class Chip8Test_RegsAnd8bitVals
//    : public Chip8TestBase<
//          ::testing::TestWithParam<::std::tuple<uint8_t, uint16_t, uint16_t>>>
//{
// protected:
//  std::uint8_t getReg() const { return ::std::get<0>(GetParam()); }
//  std::uint8_t getVal() const { return ::std::get<1>(GetParam()); }
//  std::uint8_t getOrigVal() const { return ::std::get<2>(GetParam()); }
//};

} // namespace

TEST_F(Chip8Test, LD_Vx_nn) {
  simple_XKK_test(0x60, [&](uint8_t /*reg*/, uint8_t nn, uint8_t /*nn_orig*/) {
    return nn;
  });
}

TEST_F(Chip8Test, ADD_Vx_nn) {
  simple_XKK_test(0x70, [&](uint8_t /*reg*/, uint8_t nn, uint8_t nn_orig) {
    return static_cast<uint8_t>(nn + nn_orig);
  });
}

// INSTANTIATE_TEST_CASE_P(
//    Chip8Test_RegsAnd8bitValsInstance, Chip8Test_RegsAnd8bitVals,
//    ::testing::Combine(::testing::Range<uint8_t>(0, 0xf + 1),
//                       ::testing::Range<uint16_t>(0, 0xff + 1),
//                       ::testing::Range<uint16_t>(0, 0xff + 1)));

int main(int argc, char **argv) {
  int rv;
  // Initialize SDL
  rv = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER |
                SDL_INIT_NOPARACHUTE);
  if (0 != rv) {
    std::fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    return 1;
  }
  testing::InitGoogleTest(&argc, argv);
  rv = RUN_ALL_TESTS();
  SDL_Quit();
  return rv;
}
