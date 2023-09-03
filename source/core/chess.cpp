#include "chess.hpp"

namespace chess {

Side getPieceColor(const PieceWithSide piece) { return piece.mSide; }

bool isWhitePiece(const PieceWithSide piece) {
  return getPieceColor(piece) == Side::kWhite;
}

bool isBlackPiece(const PieceWithSide piece) {
  return getPieceColor(piece) == Side::kBlack;
}

std::string describePiece(const PieceWithSide piece) {
  std::string description = isWhitePiece(piece) ? "White " : "Black ";
  switch (piece.mPiece) {
  case Piece::kPawn: {
    description += "pawn";
  } break;

  case Piece::kKnight: {
    description += "knight";
  } break;

  case Piece::kBishop: {
    description += "bishop";
  } break;

  case Piece::kRook: {
    description += "rook";
  } break;

  case Piece::kQueen: {
    description += "queen";
  } break;

  case Piece::kKing: {
    description += "king";
  } break;
  }

  return description;
}
} // namespace chess
