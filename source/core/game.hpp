#pragma once
#include "chess.hpp"

#include <array>
#include <deque>
#include <exception>
#include <map>
#include <vector>

namespace chess {

std::pair<Position, Position> parseMove(const std::string &move);

class GameException : public std::runtime_error {
public:
  GameException(const std::string &err) : std::runtime_error(err) {}
};

class Game {
public:
  void movePiece(Position present, Position future, EnPassant &S_enPassant,
                 Castling &S_castling, Promotion &S_promotion);

  void undoLastMove();

  bool undoIsPossible() const;

  bool castlingAllowed(BoardSide iSide, Side iColor) const;

  SquareState getPieceAtPosition(Position pos) const;

  SquareState getPieceConsiderMove(
      Position pos,
      const std::optional<IntendedMove> &intended_move = std::nullopt) const;

  /// @param pos Position at which to check if it is under attack
  /// @param iColor Side which would be under attack (defending side)
  /// @param intended_move A potential move to consider
  UnderAttack isUnderAttack(
      Position pos, Side iColor,
      const std::optional<IntendedMove> &intended_move = std::nullopt) const;

  bool isReachable(Position pos, Side iColor) const;

  bool isSquareOccupied(Position pos) const;

  bool isPathFree(Position startingPos, Position finishingPos,
                  Direction iDirection) const;

  bool canBeBlocked(Position startingPos, Position finishinPos,
                    Direction iDirection) const;

  bool isCheckMate();

  bool isKingInCheck(Side iColor,
                     const std::optional<IntendedMove> &intended_move) const;

  bool playerKingInCheck(
      const std::optional<IntendedMove> &intended_move = std::nullopt) const;

  bool wouldKingBeInCheck(PieceWithSide chPiece, Position present,
                          Position future, EnPassant &S_enPassant) const;

  Position findKing(Side iColor) const;

  void changeTurns();

  bool isFinished() const;

  Side getCurrentTurn() const;

  Side getOpponentColor() const;

  std::tuple<Position, Position, SquareState>
  parseMoveWithPromotion(const std::string &move) const;

  void logMove(std::string &to_record);

  std::string getLastMove() const;

  void deleteLastMove();

  // Save all the moves
  struct Round {
    std::string white_move;
    std::string black_move;
  };

  std::deque<Round> rounds;

  // Save the captured pieces
  std::vector<PieceWithSide> white_captured;
  std::vector<PieceWithSide> black_captured;

private:
  void capturePiece(PieceWithSide piece);

private:
  chess::Board board;

  // Undo is possible?
  struct Undo {
    bool bCanUndo = false;
    bool bCapturedLastMove = false;

    bool bCastlingKingSideAllowed = false;
    bool bCastlingQueenSideAllowed = false;

    std::optional<EnPassant> en_passant;
    std::optional<Castling> castling;
    std::optional<Promotion> promotion;
  } m_undo;

  // Castling requirements
  std::map<Side, bool> m_bCastlingKingSideAllowed{{Side::kWhite, true},
                                                  {Side::kBlack, true}};
  std::map<Side, bool> m_bCastlingQueenSideAllowed{{Side::kWhite, true},
                                                   {Side::kBlack, true}};

  // Holds the current turn
  Side m_CurrentTurn = Side::kWhite;

  // Has the game finished already?
  bool m_bGameFinished = false;
};

void makeTheMove(chess::Game &current_game, chess::Position present,
                 chess::Position future, chess::EnPassant &S_enPassant,
                 chess::Castling &S_castling, chess::Promotion &S_promotion);

} // namespace chess
