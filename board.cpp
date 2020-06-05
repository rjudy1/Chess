#include "board.h"
#include "move.h"

Board::Board() {
	reset();
}

void Board::reset() {
	board =
	{ {W + R, W + N, W + B, W + Q, W + K, W + B, W + N, W + R},
	  {W + P, W + P, W + P, W + P, W + P, W + P, W + P, W + P},
	  {0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    },
	  {0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    },
	  {0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    },
	  {0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    },
	  {D + P, D + P, D + P, D + P, D + P, D + P, D + P, D + P},
	  {D + R, D + N, D + B, D + Q, D + K, D + B, D + N, D + R} };
}

void Board::makeMove(Move m) {
	if ((m.yd == 0 || m.yd == 7) && board[m.ys][m.xs] % 8 == P) // promotion
		board[m.ys][m.xs] = int(board[m.ys][m.xs] / 8) * 8 + Q;
	else if (board[m.ys][m.xs] % 8 == P && m.xs != m.xd && board[m.yd][m.xd] == 0) // en passant
		board[m.yd + m.ys - m.yd][m.xd] = 0;
	else if (board[m.ys][m.xs] % 8 == K && abs(m.xd - m.xs) == 2) { // castling
		board[m.yd][m.xd + (m.xs - m.xd) / 2] = int(board[m.ys][m.xs] / 8) * 8 + R;
		if (m.xd > m.xs)
			board[m.yd][m.xd + 1] = 0;
		else
			board[m.yd][m.xd - 2] = 0;
	}

	board[m.yd][m.xd] = board[m.ys][m.xs];
	board[m.ys][m.xs] = 0;
}
