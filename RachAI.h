/*
Class Name:	RachAI
Author:		Rachael Judy
Purpose:	Chess AI
Date:		5/16/2020
Notes:
	- 
	- 
Updated:	6/20/2020
*/

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "wx/wxprec.h"

#include "player.h"
#include "board.h"


#ifndef RACHAI_H
#define RACHAI_H

using namespace std;

//------------- Structures for the AI ----------------//
enum RVALUES {
	PAWN = 1,
	ROOK = 5,
	KNIGHT = 3, //maybe 4?
	BISHOP = 3,
	QUEEN = 9, // maybe 8 or 10?
	KING = 1000
};

enum RSHIFT {
	rNW = -9,
	rN = -8,
	rNE = -7,
	rE = 1,
	rSE = 9,
	rS = 8,
	rSW = 7,
	rW = -1
};

struct rPiece {
	char color;
	char type; // e, q, b, k, r, p
	int value; // based on enum
	bool special; // can castle or en passant now an option
};

struct rMove {// place own piece as second if no capture, king as secondary on castle, enemy with pos 0 if capture
	int org; // key (original location/key)
	int position; // to position
	int note; // 0 for normal move/capture, 1 for en passant, 2 for queenside castle, 3 for kingside castle, 4 promote 
};

class rRestorePoint {
public:
	rRestorePoint(rPiece rP, int rPos, bool add) {
		r = rP;
		rPosition = rPos;
		rAdd = add;
	}

	void restore(unordered_map<int, rPiece>& map) {
		if (rAdd) {
			map[rPosition] = r;
		}
		else {
			map.erase(rPosition);
		}
	}

	rPiece r;
	int rPosition;
	bool rAdd;
};


//-------------- AI Header -------------------------//
class RachAI : Player {
public:
	void Init(bool color) override;
	Move GetMove(Move lastMove) override;
	void executeMove(const rMove& m, unordered_map<int, rPiece>&, int& mods, bool real = false);

private:
	int getMoveStrength(int, unordered_map<int, rPiece>&);
	void getAvailableMoves(const unordered_map<int, rPiece>&, vector<rMove>&, bool castleCheck = false);

	void toggleTurn();
	bool tookKing(const unordered_map<int, rPiece>& map);
	bool inCastlingDanger(const unordered_map<int, rPiece>& map, const int& side, const int& kingPos);
	bool checkShift(const int& pos, const int& shift);
	bool checkJump(const int& pos, const int& future);
	void getShiftMoves(const unordered_map<int, rPiece>& map, vector<rMove>& moves, int pos, const int& shift);
	int getPositionValue(int& pos, char&);
	Move rMoveToMove(rMove&);
	rMove MovetoRMove(Move&);
	int getDepthVariable();

	std::unordered_map<int, rPiece> pieces; // key will be between 0 and 63 = position
	std::queue<int> pawnSpecialOff;
	std::stack<rRestorePoint> restoreStack;

	std::vector<int> rShifts = { RSHIFT::rNW, RSHIFT::rN, RSHIFT::rNE, RSHIFT::rE, RSHIFT::rSE, RSHIFT::rS, RSHIFT::rSW, RSHIFT::rW };
	std::vector<int> rJumps = { -10, -17, -15, -6, 10, 17, 15, 6 };

	int key; //saved key between functions of focus might not need?
	char me;
	char opp;
	char turn;
	int high;
	int TARGET_DEPTH = 4; // minimum 2
	int WEIGHT = 40;

	// for values of position
	vector<int> ePosValues;
	vector<int> qPosValues;
	vector<int> bPosValues;
	vector<int> kPosValues;
	vector<int> rPosValues;
	vector<int> pPosValues;
};

#endif