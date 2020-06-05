#pragma once
using namespace std;
#include <vector>
//#include "pos.h"
#include "move.h"

constexpr int P = 1;
constexpr int N = 2;
constexpr int B = 3;
constexpr int R = 4;
constexpr int Q = 5;
constexpr int K = 6;

constexpr int W = 0;
constexpr int D = 8;


class Board {
public:
	Board();
	void reset();
	void makeMove(Move m);
	inline int getPiece(int x, int y) { return board[y][x]; }
private:
	vector<vector<int>> board;
};