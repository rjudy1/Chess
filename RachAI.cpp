/*

Chess AI
Author:		Rachael Judy
Purpose:	Chess AI
Date:		5/16/2020
Notes:
	- Board 0-63
	- coordinates are y, x start in lower left
	- start NW and work way around clockwise for checking possible moves

	- take number of moves opponent has into account - small factor - available/total possible * 2??
	- tune the piece square tables
	- not protecting king, values too high??
	- value compounding is out of control
	- need to increase depth to >=50 to make next move most significant
	- push the item removed onto a queue and push it back into map after that section is complete?

Board:
70 71 72 73
60 61 62
50 51
40
		-  -  -  -  -  -  -  -
	7 | 0  1  2  3  4  5  6  7  |
	6 | B  W  B  W  B  W  B  W  |
	5 | W  B  W  B  W  B  W  B  |
	4 | B  W  B  W  B  W  B  W  |
	3 | W  B  W  B  W  B  W  B  |
	2 | B  W  B  W  B  W  B  W  |
	1 | W  B  W  B  W  B  W  B  |
	0 | 56 57 58 59 60 61 62 63 |
		-  -  -  -  -  -  -  -
		0  1  2  3  4  5  6  7

*/

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

#include "RachAI.h"

using namespace std;


// sets initial positions
void RachAI::Init(bool isWhite) {
	me = (isWhite ? 'w' : 'b'); // me is the AI
	opp = (isWhite ? 'b' : 'w');
	turn = me;
	high = (int)isWhite - (int)!isWhite;

	pieces = {
		{4,  rPiece{ 'b', 'e', RVALUES::KING,   true }},
		{3,  rPiece{ 'b', 'q', RVALUES::QUEEN,  false}},
		{2,  rPiece{ 'b', 'b', RVALUES::BISHOP, false}},
		{5,  rPiece{ 'b', 'b', RVALUES::BISHOP, false}},
		{1,  rPiece{ 'b', 'k', RVALUES::KNIGHT, false}},
		{6,  rPiece{ 'b', 'k', RVALUES::KNIGHT, false}},
		{0,  rPiece{ 'b', 'r', RVALUES::ROOK,   true }},
		{7,  rPiece{ 'b', 'r', RVALUES::ROOK,   true }},
		{9,  rPiece{ 'b', 'p', RVALUES::PAWN,   true }},
		{13, rPiece{ 'b', 'p', RVALUES::PAWN,   true }},
		{10, rPiece{ 'b', 'p', RVALUES::PAWN,   true }},
		{14, rPiece{ 'b', 'p', RVALUES::PAWN,   true }},
		{11, rPiece{ 'b', 'p', RVALUES::PAWN,   true }},
		{15, rPiece{ 'b', 'p', RVALUES::PAWN,   true }},
		{12, rPiece{ 'b', 'p', RVALUES::PAWN,   true }},
		{8,  rPiece{ 'b', 'p', RVALUES::PAWN,   true }},

		{60, rPiece{ 'w', 'e', RVALUES::KING,   true }},
		{59, rPiece{ 'w', 'q', RVALUES::QUEEN,  false}},
		{58, rPiece{ 'w', 'b', RVALUES::BISHOP, false}},
		{61, rPiece{ 'w', 'b', RVALUES::BISHOP, false}},
		{62, rPiece{ 'w', 'k', RVALUES::KNIGHT, false}},
		{57, rPiece{ 'w', 'k', RVALUES::KNIGHT, false}},
		{56, rPiece{ 'w', 'r', RVALUES::ROOK,   true }},
		{63, rPiece{ 'w', 'r', RVALUES::ROOK,   true }},
		{48, rPiece{ 'w', 'p', RVALUES::PAWN,   true }},
		{49, rPiece{ 'w', 'p', RVALUES::PAWN,   true }},
		{50, rPiece{ 'w', 'p', RVALUES::PAWN,   true }},
		{51, rPiece{ 'w', 'p', RVALUES::PAWN,   true }},
		{52, rPiece{ 'w', 'p', RVALUES::PAWN,   true }},
		{53, rPiece{ 'w', 'p', RVALUES::PAWN,   true }},
		{54, rPiece{ 'w', 'p', RVALUES::PAWN,   true }},
		{55, rPiece{ 'w', 'p', RVALUES::PAWN,   true }}
	};

	ePosValues = {
		-3, -4, -4, -5, -5, -4, -4, -3,
		-3, -4, -4, -5, -5, -4, -4, -3,
		-3, -4, -4, -5, -5, -4, -4, -3,
		-3, -4, -4, -5, -5, -4, -4, -3,
		-2, -3, -3, -4, -4, -3, -3, -2,
		-1, -2, -2, -2, -2, -2, -2, -2,
		 2,  2,  0,  0,  0,  0,  2,  2,
		 2,  3,  1,  0,  0,  1,  3,  2
	}; // array of 64 values
	qPosValues = {
		-3, -2, -2, -1, -1, -2, -2, -3,
		-2,  0,  0,  0,  0,  0,  0, -2,
		-2,  0,  1,  1,  1,  1,  0, -2,
		-1,  0,  1,  1,  1,  1,  0, -1,
		 0,  0,  1,  1,  1,  1,  0,  0,
		-2,  1,  1,  1,  1,  1,  0, -2,
		-2,  0,  1,  0,  0,  0,  0, -2,
		-3, -2, -2, -1, -1, -2, -2, -3
	};
	bPosValues = {
		-3, -2, -2, -1, -1, -2, -2, -3,
		-2,  0,  0,  0,  0,  0,  0, -2,
		-2,  0,  1,  2,  2,  1,  0, -2,
		-2,  1,  1,  2,  2,  1,  0, -2,
		-2,  0,  2,  2,  2,  2,  0, -2,
		-2,  2,  2,  2,  2,  2,  2, -2,
		-2,  1,  1,  0,  0,  0,  1, -2,
		-3, -2, -2, -2, -2, -2, -2, -3
	};
	kPosValues = {
		 -5, -4, -3, -3, -3, -3, -4, -5,
		 -4, -2,  0,  0,  0,  0, -2, -4,
		 -3,  0,  1,  2,  2,  1,  0, -3,
		 -3,  1,  2,  2,  2,  2,  1, -3,
		 -3,  0,  1,  2,  2,  1,  0, -3,
		 -3,  1,  1,  2,  2,  1,  1, -3,
		 -4, -2,  0,  1,  1,  0, -2  -4,
		 -5, -4, -3, -3, -3, -3, -4, -5
	};
	rPosValues = {
		  0,  0,  0,  0,  0,  0,  0,  0,
		  1,  1,  1,  1,  1,  1,  1,  1,
		 -1,  0,  0,  0,  0,  0,  0, -1,
		 -1,  0,  0,  0,  0,  0,  0, -1,
		 -1,  0,  0,  0,  0,  0,  0, -1,
		 -1,  0,  0,  0,  0,  0,  0, -1,
		 -1,  0,  0,  0,  0,  0,  0, -1,
		  0,  0,  0,  1,  1,  0,  0,  0

	};
	pPosValues = {
		  2,  2,  2,  2,  2,  2,  2,  2,
		  5,  5,  5,  5,  5,  5,  5,  5,
		  1,  1,  2,  3,  3,  2,  1,  1,
		  1,  1,  1,  3,  3,  1,  1,  1,
		  0,  0,  0,  2,  2,  0,  0,  0,
		  1, -1, -1,  0,  0, -1, -1,  1,
		  1,  1,  1, -2, -2,  1,  1,  1,
		 -1, -1, -1, -1, -1, -1, -1, -1
	};
}

// return a calculated Move to the game
Move RachAI::GetMove(Move opMove) {
	// convert to my move
	rMove r = MovetoRMove(opMove);

	// turn off special for en passant, queued pawns
	while (!pawnSpecialOff.empty()) {
		int top = pawnSpecialOff.front();
		pieces.at(top).special = false;
		pawnSpecialOff.pop();
	}

	queue<rRestorePoint> q = {};

	// add players move to the configuration
	if (r.org != r.position)
		executeMove(r, pieces, q, true);

	// find best move based on recursive depth
	turn = me;
	vector<rMove> moves = getAvailableMoves(pieces); // the AI's available moves
	int strength = 0;
	int currentStrength = -5000;
	int depth = 50; // 50 should allow for depth 2
	unordered_map<int, rPiece> map = pieces;
	r = moves.at(0);
	queue<rRestorePoint> mainQ;
	for (auto move : moves) { // iterates through the available moves, testing execution on pieces
		while (!mainQ.empty()) {
			mainQ.front().restore(map);
			mainQ.pop();
		}
		strength = 0;
//		map = pieces; // restore point
		if (map.count(move.position))
			strength += map.at(move.position).value * depth; // potential losses 
		strength += (getPositionValue(move.position, map.at(move.org).type)
			- getPositionValue(move.org, map.at(move.org).type) * high * depth);
		executeMove(move, map, mainQ); // execute on temporary map?
		turn = opp; // set to non AI's turn to get the available moves
		strength += getMoveStrength(depth, getAvailableMoves(map), map);
		if (strength >= currentStrength) {// chooses move with max strength
			currentStrength = strength;
			r = move;
		}
	}

	// convert rMove to Move
	Move myMove = rMoveToMove(r);
	executeMove(r, pieces, q, true);

	//	return Move // Move of choice or default
	return myMove;
}

// register the move on my piece enemyMap
void RachAI::executeMove(const rMove& m, unordered_map<int, rPiece>& map, queue<rRestorePoint>& qR, bool real) {
	int shift = -2 * m.note + 5; // -1 for 2, 1 for 3

	// add the piece we just moved back in
	qR.push(rRestorePoint(map.at(m.org), m.org, true)); // readd the original

	// take move and conditions into consideration
	switch (m.note) {
	case 0:
		if (map.count(m.position)) // if there's a capture
			qR.push(rRestorePoint(map.at(m.position), m.position, true)); // readd the captured
		else
			qR.push(rRestorePoint(map.at(m.org), m.position, false)); // remove the new piece

		map[m.position] = map.at(m.org); // replace or create
		if (map[m.position].type == 'r' || map[m.position].type == 'e')
			map[m.position].special = false;
		else if (map[m.position].type == 'p' && real)
			pawnSpecialOff.push(m.position);
		break;
	case 1:
		qR.push(rRestorePoint(map.at(m.org), m.position, false)); // remove the new

		map[m.position] = map.at(m.org);

		qR.push(rRestorePoint(map.at(m.position + 8 * (map.at(m.position).color == 'w' ? -1 : 1)),
			m.position + 8 * (map.at(m.position).color == 'w' ? -1 : 1), true)); // restore the captured

		map.erase(m.position + 8 * (map.at(m.position).color == 'w' ? -1 : 1)); // erase captured
		break;
	case 2:
	case 3:
		qR.push(rRestorePoint(map.at(m.org), (m.position), false)); // remove the new king
		qR.push(rRestorePoint(map.at(m.position + (m.note * 3 - 8)), m.position + shift, false));
		qR.push(rRestorePoint(map.at(m.position + (m.note * 3 - 8)), m.position + (m.note * 3 - 8), true)); // re add original rook

		map[m.org].special = false; // end king speciality
		map[m.position] = map[m.org];
		map[m.position + shift] = map[m.position + (m.note * 3 - 8)];
		map[m.position + shift].special = false; // end king speciality
		map.erase(m.position + (m.note * 3 - 8));
		break;
	case 4:
		if (map.count(m.position)) // if there's a capture
			qR.push(rRestorePoint(map.at(m.position), m.position, true)); // readd the captured
		else
			qR.push(rRestorePoint(map.at(m.org), m.position, false)); // remove the new queen

		map[m.position] = map.at(m.org);
		map[m.position].type = 'q';
		map[m.position].value = RVALUES::QUEEN;
		break;
	}
	map.erase(m.org); // erase original
}


//////////////////////// Calculations ////////////////////////////

// determine strength of particularly move based on board config/players
int RachAI::getMoveStrength(int depth, vector<rMove> moves, unordered_map<int, rPiece> thisMap) {
	int value = 0; // value of the move
	
	depth /= 50; // gone a level of depth down and thus should do this iteration
	// looking at enemy's moves
	unordered_map<int, rPiece> enemyMap; // working map for opponent
	unordered_map<int, rPiece> myMap; // working map for AI
	vector<rMove> mmoves;
	vector<rMove> nmoves;
	queue<rRestorePoint> enemyQ;
	queue<rRestorePoint> myQ;
	for (auto move : moves) { // go through full map of moves passed down
		while (!enemyQ.empty()) {
			enemyQ.front().restore(enemyMap);
			enemyQ.pop();
		}
//		enemyMap = thisMap;
		if (enemyMap.count(move.position))
			value -= enemyMap.at(move.position).value * depth; // potential losses 
		value -= ((getPositionValue(move.position, enemyMap.at(move.org).type) 
			- getPositionValue(move.org, enemyMap.at(move.org).type) * high * depth));
		executeMove(move, enemyMap, enemyQ);
		turn = me; // get the AIs possible follow up moves
		mmoves = getAvailableMoves(enemyMap);

		if (boardTotal(enemyMap) < -500) {
			value -= 1000 * depth;
			break;
		}

		// look at my next moves
		for (auto mmove : mmoves) {
			while (!myQ.empty()) {
				myQ.front().restore(enemyMap);
				myQ.pop();
			}
//			myMap = enemyMap; // reset map after every iteration forward
			if (myMap.count(mmove.position)) // capture available
				value += myMap.at(mmove.position).value * depth;
			value += (getPositionValue(move.position, myMap.at(mmove.org).type)
				- getPositionValue(move.org, myMap.at(mmove.org).type) * high * depth);
			executeMove(mmove, myMap, enemyQ);
			turn = opp; // get possible opponents moves
			nmoves = getAvailableMoves(myMap);

			if (depth != 1)
				value += getMoveStrength(depth, nmoves, myMap);
		}
	}
	return value;

}

// gets a vector of possible moves
vector<rMove> RachAI::getAvailableMoves(const unordered_map<int, rPiece>& map, bool castleCheck) {
	vector<rMove> moves = {};
	for (auto p : map) {
		if (p.second.color == turn) {
			int pos = p.first;
			switch (p.second.type) {
			case 'e': // king, consider castling as rook moves
				// one square any way
				for (auto x : rShifts) {
					if (checkShift(pos, x)) {
						if (map.count(pos + x)) { // capture available 
							if (map.at(pos + x).color != turn)
								moves.push_back(rMove{ p.first, pos + x, 0 });
						}
						else // empty square
							moves.push_back(rMove{ p.first, pos + x, 0 });
					}
				}
				break;

			case 'q':
				getShiftMoves(map, moves, pos, RSHIFT::rNW);
				getShiftMoves(map, moves, pos, RSHIFT::rN);
				getShiftMoves(map, moves, pos, RSHIFT::rNE);
				getShiftMoves(map, moves, pos, RSHIFT::rE);
				getShiftMoves(map, moves, pos, RSHIFT::rSE);
				getShiftMoves(map, moves, pos, RSHIFT::rS);
				getShiftMoves(map, moves, pos, RSHIFT::rSW);
				getShiftMoves(map, moves, pos, RSHIFT::rW);
				break;

			case 'b':
				getShiftMoves(map, moves, pos, RSHIFT::rNW); // rNW
				getShiftMoves(map, moves, pos, RSHIFT::rNE); // rNE
				getShiftMoves(map, moves, pos, RSHIFT::rSE); // rSE
				getShiftMoves(map, moves, pos, RSHIFT::rSW); // rSW
				break;

			case 'k': // can jump, dual path options L
				for (auto x : rJumps) {
					if (checkJump(pos, x)) {
						if (map.count(pos + x)) { // possible capture available
							if (map.at(pos + x).color != turn)
								moves.push_back(rMove{ p.first, pos + x, 0 });
						}
						else // empty square
							moves.push_back(rMove{ p.first, pos + x, 0 });
					}
				}
				break;

			case 'r': // must consider movement for castling // store king
				getShiftMoves(map, moves, pos, RSHIFT::rN); // rN
				getShiftMoves(map, moves, pos, RSHIFT::rE); // rE
				getShiftMoves(map, moves, pos, RSHIFT::rS); // rS
				getShiftMoves(map, moves, pos, RSHIFT::rW);	// rW
				
				// castle
				if (p.second.special && !castleCheck) { // queenside 2 or kingside 3 
					int king = 5 + 56 * (p.second.color == 'b');
					int note = 2 + (p.first % 8 == 0); // queenside or kingside
					bool open = ((note == 3 && !map.count(p.first - 2) && !map.count(p.first - 1)) ||
						(note == 2 && !map.count(p.first + 1) && !map.count(p.first + 2) && !map.count(p.first + 3)));
					pos = king + 1 - 2 * (p.first % 8 == 1); // position piece would move to
					if (map.count(king) && map.at(king).special && !inCastlingDanger(map, note, king) && open)
						moves.push_back(rMove{ p.first, pos, note });
				}
				break;

			case'p': // en passant capture, always forward, capture/move diff
				int dir = (turn == 'w' ? -1 : 1);
				int note = 4 * (pos + 8 * dir >= 57 || pos + 8 * dir <= 8); // promote
				// forward
				if (!castleCheck && !map.count(pos + 8 * dir)) {// forward 
					moves.push_back(rMove{ p.first, pos + 8 * dir, note });
					if (!map.count(pos + 16 * dir) && p.second.special) // beginning of next turn, remove speciality, add to queue on execution
						moves.push_back(rMove{ p.first, pos + 16 * dir, note });
				}

				// forward and diagonal to capture /////////// need to cehck if pawn, not king or rook, and also precedence
				if ((map.count(pos + 8 * dir + 1) && map.at(pos + 8 * dir + 1).color != map.at(pos).color
					|| map.count(pos + 1) && map.at(pos + 1).special && map.at(pos + 1).color != map.at(pos).color)
					&& checkShift(pos, 8 * dir + 1)) {// capture available
					moves.push_back(rMove{ p.first, pos + 8 * dir + 1, note });
				}
				if ((map.count(pos + 8 * dir - 1) && map.at(pos + 8 * dir - 1).color != map.at(pos).color  
					|| map.count(pos - 1) && map.at(pos - 1).special && map.at(pos-1).color != map.at(pos).color)
					&& checkShift(pos, 8 * dir - 1) && pos < 40 && pos >= 24) {// capture available
					moves.push_back(rMove{ p.first, pos + 8 * dir - 1, note });
				
				}
				break;
			}
		}
	}
	return moves;
}


///////////////////// HELPER Functions to check limits ////////////////////////

int RachAI::boardTotal(const unordered_map<int, rPiece>& map) {
	int value = 0;
	for (auto p : map) {
		value += p.second.value;
	}
	return value;
}

// switches actual turn
void RachAI::toggleTurn() { 
	turn = ((turn == 'w') ? 'b' : 'w'); 
}

// check if the path of the king is threatened
bool RachAI::inCastlingDanger(const unordered_map<int, rPiece>& map, const int& side, const int& kingPos) {
	// check king square and square beside king
	toggleTurn(); // set turn temporarily to other player
	vector<rMove> dangers = getAvailableMoves(map, true); // this is an expensive choice
	for (auto d : dangers) {
		if (d.position == kingPos || d.position == kingPos + (2 * side - 5))
			return true;
	}
	toggleTurn(); // return turn back to current player
	return false;
}

int RachAI::getPositionValue(int& pos, char& piece) {
	switch (piece) {
	case 'e':
		return ePosValues.at(pos); // multiply by direction or player?
	case 'q':
		return qPosValues.at(pos);
	case 'b':
		return bPosValues.at(pos);
	case 'k':
		return kPosValues.at(pos);
	case 'r':
		return rPosValues.at(pos);
	case 'p':
		return pPosValues.at(pos);
	}

}

// check edges for shifts
bool RachAI::checkShift(const int& pos, const int& shift) {
	switch (shift) {
	case RSHIFT::rNW:
		return (pos % 8 != 0 && pos > 7);
	case RSHIFT::rN:
		return (pos > 7);
	case RSHIFT::rNE:
		return (pos % 8 != 7 && pos > 7);
	case RSHIFT::rE:
		return (pos % 8 != 7);
	case RSHIFT::rSE:
		return (pos % 8 != 7 && pos < 56);
	case RSHIFT::rS:
		return (pos < 56);
	case RSHIFT::rSW:
		return (pos % 8 != 0 && pos < 56);
	case RSHIFT::rW:
		return (pos % 8 != 0);
	}
}

// check edges for jumps
bool RachAI::checkJump(const int& pos, const int& dir) {
	switch (dir) {
	case (-10): // one up, two left
		return (pos > 7 && pos % 8 != 0 && pos % 8 != 1);
	case -17: // two up, one left
		return (pos > 15 && pos % 8 != 0);
	case -15: // two up, one right
		return (pos > 15 && pos % 8 != 7);
	case -6: // one up, two right
		return (pos > 7 && pos % 8 != 6 && pos % 8 != 7);
	case 10: // one down, two right
		return (pos < 56 && pos % 8 != 6 && pos % 8 != 7);
	case 17: // two down, one right
		return (pos < 48 && pos % 8 != 7);
	case 15: // two down, one left
		return (pos < 48 && pos % 8 != 0);
	case 6: // one down, two left
		return (pos < 56 && pos % 8 != 0 && pos % 8 != 1);
	}
}

// get all moves from  shifting the pieces in the indicated direction of shift
void RachAI::getShiftMoves(const unordered_map<int, rPiece>& map, vector<rMove>& moves, int pos, const int& shift) {
	int p = pos;
	while (checkShift(pos, shift)) {
		pos += shift;
		if (map.count(pos)) {// capture available
			if (map.at(pos).color != turn) {
				moves.push_back(rMove{ p, pos, 0 });
			}
			break;
		}
		else // empty square
			moves.push_back(rMove{ p, pos, 0 });
	}
}

Move RachAI::rMoveToMove(rMove& myMove) {
	Move m(myMove.org % 8, 7 - (myMove.org / 8), myMove.position % 8, 7 - (myMove.position / 8));
	return m;
}

// might need to upgrade to find out what its special note is
rMove RachAI::MovetoRMove(Move& opMove) {
	int special = 0;
	int source = (7 - opMove.ys) * 8 + opMove.xs;
	int dest = (7 - opMove.yd) * 8 + opMove.xd;
	if (pieces.at(source).type == 'e' && abs(opMove.xd - opMove.xs) == 2)
		if (opMove.xd - opMove.xs == 2) // do this for all moves or my exclusive move
			special = 3;
		else
			special = 2;
	else if (!pieces.count(dest) && opMove.xs != opMove.xd
		&& pieces.at(source).type == 'p')
		special = 1;
	else if (pieces.at(source).type == 'p' && (dest > 56 || dest < 8))
		special = 4;

	rMove m = { source, dest, special };
	return m;
}