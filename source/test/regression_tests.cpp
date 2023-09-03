#include <catch2/catch_test_macros.hpp>

#include "game.hpp"
#include "load_save.hpp"

TEST_CASE("black_promote", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    //A  B  C  D  E  F  G  H
      E, E, E, Q, r, E, E, E,  // 1
      E, E, E, E, E, n, E, K,  // 2
      E, P, E, E, E, p, P, P,  // 3
      E, E, E, E, E, E, E, E,  // 4
      E, E, E, Q, E, E, N, E,  // 5 
      P, E, E, E, E, q, k, E,  // 6
      E, E, E, E, E, E, E, R,  // 7
      E, E, E, E, E, E, E, E}; // 8
  // clang-format on

  const chess::Game game = chess::loadGame("dat/black_promote.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("bug", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
      R, N, B, K, R, E, E, E,
      E, E, P, P, E, P, P, P,
      P, E, E, E, E, E, E, N,
      P, E, E, E, q, E, E, E,
      E, E, E, E, E, E, E, E,
      p, p, E, p, E, E, p, p,
      E, E, p, E, b, p, E, E,
      r, n, E, E, k, E, n, Q};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/bug.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("burro", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
     R, N, B, E, K, B, N, R,
     P, P, P, P, E, P, P, P,
     E, E, E, E, E, E, E, E,
     E, E, E, E, P, E, E, E,
     E, E, E, E, E, E, E, E,
     E, E, E, E, E, E, p, E,
     p, p, p, p, q, p, E, p,
     r, n, b, E, k, b, n, Q };
  // clang-format on

  const chess::Game game = chess::loadGame("dat/burro.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("castling_both", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, E, E, E, E, R, K, E, 
    P, P, P, E, B, P, P, P, 
    E, E, N, P, E, Q, E, N, 
    E, E, E, E, P, E, b, E, 
    E, E, E, E, p, E, B, E, 
    E, E, n, p, E, q, E, n, 
    p, p, p, E, b, p, p, p, 
    E, E, k, r, E, E, E, r};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/castling_both.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("check", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, N, B, Q, K, B, N, R,
    E, P, P, P, E, P, P, P,
    P, E, E, E, E, E, E, E,
    E, E, E, E, E, E, E, E,
    E, E, E, E, q, E, E, E,
    E, E, E, E, E, E, E, E,
    p, p, p, E, p, p, p, p,
    r, n, b, E, k, b, n, r};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/check.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("checkmate", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, N, B, E, K, B, N, R, 
    E, P, P, P, E, P, P, P, 
    P, E, E, E, E, E, E, E, 
    E, E, E, E, P, E, E, E, 
    E, E, E, E, E, p, E, Q, 
    E, E, E, E, E, E, p, p, 
    p, p, p, p, p, E, E, E, 
    r, n, b, q, k, b, n, r};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/checkmate.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("impossible", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, N, B, Q, K, B, N, R, 
    P, P, P, P, P, P, P, P, 
    E, E, E, E, E, E, E, E, 
    E, E, E, E, E, E, E, E, 
    E, E, E, E, E, E, E, E, 
    E, E, E, E, E, E, E, E, 
    p, p, p, p, p, p, p, p, 
    r, n, b, q, k, b, n, r};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/impossible.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("kasparov_2", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, E, E, E, E, R, K, E, 
    E, P, E, E, Q, P, P, E, 
    P, E, E, E, B, E, E, P, 
    E, E, E, P, E, E, E, E, 
    b, N, E, E, N, E, E, E, 
    E, E, n, E, p, n, E, E, 
    p, p, E, E, q, p, p, p, 
    r, E, E, E, E, r, k, E};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/kasparov_2.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("KasparovVSdeepblue_game_1", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    E, E, E, E, r, E, E, E, 
    E, E, E, E, E, n, E, K, 
    P, P, E, E, E, p, P, P, 
    E, E, E, p, E, E, E, E, 
    E, E, E, Q, E, E, N, E, 
    E, E, E, E, E, q, E, k, 
    E, E, E, E, E, E, E, R, 
    E, E, E, E, E, E, E, E};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/KasparovVSdeepblue_game_1.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("king_side", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, N, B, Q, E, R, K, E, 
    P, P, P, P, E, P, P, P, 
    E, E, E, E, E, N, E, E, 
    E, E, B, E, P, E, E, E, 
    E, E, b, E, p, E, E, E, 
    E, E, E, E, E, n, E, E, 
    p, p, p, p, E, p, p, p, 
    r, n, b, q, E, r, k, E};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/king_side.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("open_castling", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, N, B, Q, E, R, K, E, 
    P, P, P, P, E, E, P, P, 
    E, E, E, E, E, P, E, N, 
    E, E, E, E, P, E, E, E, 
    E, E, E, p, E, E, E, E, 
    B, E, E, E, E, E, p, p, 
    E, p, p, E, p, p, E, E, 
    r, n, b, q, k, b, n, r};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/open_castling.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("passant_check", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, N, B, Q, E, B, N, R, 
    P, P, P, E, E, P, P, P, 
    E, E, E, E, P, K, E, E,
    E, E, E, E, E, E, E, E, 
    E, E, E, P, E, E, E, E, 
    E, p, E, E, E, E, p, p, 
    p, b, p, p, p, p, E, E, 
    r, n, E, q, k, b, n, r};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/passant_check.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("passant_done", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, N, B, Q, K, B, N, R, 
    P, P, P, P, E, P, P, P, 
    E, E, E, E, E, E, E, E, 
    E, E, E, E, E, E, E, E, 
    E, E, E, E, E, E, E, E, 
    p, E, E, E, E, P, E, E, 
    E, p, p, p, p, E, p, p, 
    r, n, b, q, k, b, n, r};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/passant_done.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("passant", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    R, N, B, Q, K, B, N, R, 
    P, P, P, P, E, P, P, P, 
    E, E, E, E, E, E, E, E, 
    E, E, E, E, E, E, E, E, 
    E, E, E, E, P, p, E, E, 
    p, E, E, E, E, E, E, E, 
    E, p, p, p, p, E, p, p, 
    r, n, b, q, k, b, n, r};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/passant.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("queen_side", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    E, E, K, R, E, B, N, R, 
    P, P, P, E, E, P, P, P, 
    E, E, N, P, E, Q, E, E, 
    E, E, E, E, P, E, b, E, 
    E, E, E, E, p, E, B, E, 
    E, E, n, p, E, q, E, E, 
    p, p, p, E, E, p, p, p, 
    E, E, k, r, E, b, n, r};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/queen_side.dat");

  CHECK(game.board().boardState() == expected);
}

TEST_CASE("white_promote", "[regression]") {
  using namespace chess::pieces;
  // clang-format off
  constexpr chess::Board::BoardArray expected{
    E, E, E, E, r, E, E, E, 
    E, E, E, p, E, n, E, K, 
    E, P, E, E, E, p, P, P, 
    E, E, E, E, E, E, E, E, 
    E, E, E, Q, E, E, N, E, 
    E, E, E, E, E, q, k, E,
    E, E, E, E, E, E, E, R, 
    R, E, E, E, E, E, E, E};
  // clang-format on

  const chess::Game game = chess::loadGame("dat/white_promote.dat");

  CHECK(game.board().boardState() == expected);
}
