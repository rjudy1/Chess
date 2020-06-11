/*
Class Name:	RachAI
Author:		Rachael Judy
Purpose:	Chess AI
Date:		5/16/2020
Notes:
	- 
	- 
Updated:	5/29/2020
*/

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>

#include "player.h"
#include "board.h"
#include "Structures.h"

using namespace std;

class RachAI : Player {
public:
	void Init(bool color) override;
	Move GetMove(Move lastMove) override;
	unordered_map<int, rPiece>& executeMove(const rMove& m, unordered_map<int, rPiece>&, bool real = false);

private:
	int getMoveStrength(int, vector<rMove>, unordered_map<int, rPiece>);
	vector<rMove> getAvailableMoves(const unordered_map<int, rPiece>&, bool castleCheck = false);

	int boardTotal(const unordered_map<int, rPiece>& map);
	void toggleTurn();
	bool inCastlingDanger(const unordered_map<int, rPiece>& map, const int& side, const int& kingPos);
	bool checkShift(const int& pos, const int& shift);
	bool checkJump(const int& pos, const int& future);
	void getShiftMoves(const unordered_map<int, rPiece>& map, vector<rMove>& moves, int pos, const int& shift);
	int getPositionValue(int& pos, char&);
	Move rMoveToMove(rMove&);
	rMove MovetoRMove(Move&);

	std::unordered_map<int, rPiece> pieces; // key will be between 0 and 63 = position
	std::queue<int> pawnSpecialOff;

	std::vector<int> rShifts = { RSHIFT::rNW, RSHIFT::rN, RSHIFT::rNE, RSHIFT::rE, RSHIFT::rSE, RSHIFT::rS, RSHIFT::rSW, RSHIFT::rW };
	std::vector<int> rJumps = { -10, -17, -15, -6, 10, 17, 15, 6 };

	int key; //saved key between functions of focus might not need?
	char me;
	char opp;
	char turn;
	int high;
	vector<int> ePosValues;
	vector<int> qPosValues;
	vector<int> bPosValues;
	vector<int> kPosValues;
	vector<int> rPosValues;
	vector<int> pPosValues;
};