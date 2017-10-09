#include <Chip8/Board.h>
#include <Chip8/Cpu.h>
#include <Chip8/Memory.h>
#include <Chip8/Video.h>

#include "../src/debugger.h"
#include "../src/sdlvideo.h"

#include <gtest/gtest.h>

template <typename T, T min, T max> class Range { public: };

namespace {
std::shared_ptr<Chip8::Board> g_board;
}

template <typename T> class Chip8TestBase : public T {
public:
  static void SetUpTestCase() {
    g_board =
        std::make_shared<Chip8::Board>(std::make_shared<Chip8::SDLVideo>());
  }

  static void TearDownTestCase() { g_board.reset(); }

protected:
  virtual void SetUp() { g_board->reset(); }
  virtual void TearDown() {}

  virtual Chip8::Board *board() { return g_board.get(); }
  virtual std::shared_ptr<Chip8::Board> board_sp() { return g_board; }
};

using Chip8Test = Chip8TestBase<::testing::Test>;

class Chip8Test_RegsAnd8bitVals
    : public Chip8TestBase<
          ::testing::TestWithParam<::std::tuple<uint8_t, uint16_t, uint16_t>>>
// class Chip8Test_RegsAnd8bitVals : public
// Chip8TestBase<::testing::TestWithParam<uint8_t>>
{
protected:
  uint8_t getReg() const { return ::std::get<0>(GetParam()); }
  uint8_t getVal() const { return ::std::get<1>(GetParam()); }
  uint8_t getOrigVal() const { return ::std::get<2>(GetParam()); }
};

// TEST_F(Chip8Test, ADD_V0_0x25) {
//  board()->LoadBinary({0x70, 0x25,
//                    });
//  board()->step();
//  uint8_t out;
//  ASSERT_EQ(Chip8::ResultType::Ok, board()->cpu()->Vx(0x0, out));
//  EXPECT_EQ(0x25, out);
//  //Chip8::Debugger(board).dumpDisasm(Chip8::ProgramStartLocation, 4);
//}

TEST_P(Chip8Test_RegsAnd8bitVals, LD_Vx_nn) {
  const uint8_t reg = getReg();
  const uint8_t nn = getVal();
  const uint8_t nn_orig = getOrigVal();
  board()->LoadBinary({std::uint8_t(0x60 | reg), nn});
  ASSERT_EQ(Chip8::ResultType::Ok, board()->cpu()->setVx(reg, nn_orig));
  board()->step();
  uint8_t out;
  ASSERT_EQ(Chip8::ResultType::Ok, board()->cpu()->Vx(reg, out));
  EXPECT_EQ(nn, out);
  // Chip8::Debugger(board).dumpDisasm(Chip8::ProgramStartLocation, 4);
}

TEST_P(Chip8Test_RegsAnd8bitVals, ADD_Vx_nn) {
  const uint8_t reg = getReg();
  const uint8_t nn = getVal();
  const uint8_t nn_orig = getOrigVal();
  board()->LoadBinary({std::uint8_t(0x70 | reg), nn});
  ASSERT_EQ(Chip8::ResultType::Ok, board()->cpu()->setVx(reg, nn_orig));
  board()->step();
  uint8_t out;
  uint8_t expected = (nn + nn_orig);
  ASSERT_EQ(Chip8::ResultType::Ok, board()->cpu()->Vx(reg, out));
  EXPECT_EQ(expected, out);
}

INSTANTIATE_TEST_CASE_P(
    Chip8Test_RegsAnd8bitValsInstance, Chip8Test_RegsAnd8bitVals,
    ::testing::Combine(::testing::Range<uint8_t>(0, 0xf + 1),
                       ::testing::Range<uint16_t>(0, 0xff + 1),
                       ::testing::Range<uint16_t>(0, 0xff + 1)));

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
