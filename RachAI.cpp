/*

Chess AI
Author:		Rachael Judy
Purpose:	Chess AI
Date:		5/16/2020
Notes:
	- Board 1-64
	- coordinates are y, x start in lower left
	- start NW and work way around clockwise for checking possible moves

	- take position strength into account
	- take number of moves opponent has into account - small factor - available/total possible * 2?
	- tune the piece square tables

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
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-20, -30, -30, -40, -40, -30, -30, -20,
		-10, -20, -20, -20, -20, -20, -20, -10,
		 20,  20,   0,   0,   0,   0,  20,  20,
	     20,  30,  10,   0,   0,  10,  30,  20
	}; // array of 64 values
	qPosValues = {
		 20, 35, 35, 40, 40, 35, 35, 20,
		 20, 35, 35, 50, 50, 35, 35, 20,
		 10, 15, 25, 50, 50, 25, 15, 10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-20,-35,-35,-50,-50,-35,-35,-20,
		-10,-15,-25,-50,-50,-25,-15,-10,
		-20,-35,-35,-40,-40,-35,-35,-20,
	};
	bPosValues = {
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-40,-10,-10,-40,-10,-20,
	};
	kPosValues = {
		 -50,-40,-30,-30,-30,-30,-40,-50,
		 -40,-20,  0,  0,  0,  0,-20,-40,
		 -30,  0, 10, 15, 15, 10,  0,-30,
		 -30,  5, 15, 20, 20, 15,  5,-30,
		 -30,  0, 15, 20, 20, 15,  0,-30,
		 -30,  5, 10, 15, 15, 10,  5,-30,
		 -40,-20,  0,  5,  5,  0,-20,-40,
		 -50,-40,-20,-30,-30,-20,-40,-50,
	};
	rPosValues = {
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-20,-10,-40,-10,-10,-40,-10,-20,
	};
	pPosValues = {
		15, 20, 20, 20, 20, 20, 20, 15,
		50, 50, 50, 50, 50, 50, 50, 50,
		10, 10, 20, 30, 30, 20, 10, 10,
	    05, 05, 10, 27, 27, 10, 05, 05,
		00, 00, 00, 25, 25, 00, 00, 00,
		05, -5,-10, 00, 00,-10, -5, 05,
		05, 10, 10,-25,-25, 10, 10, 5,
	   -15,-20,-20,-20,-20,-20,-20,-15
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

	// add players move to the configuration
	if (r.org != r.position)
		executeMove(r, pieces, true);

	// find best move based on recursive depth
	turn = me;
	vector<rMove> moves = getAvailableMoves(pieces); // the AI's available moves
	int strength = 0;
	int currentStrength = 0;
	int depth = 4; // 4 should allow for depth 3 (2^(n-1))
	unordered_map<int, rPiece> map;
	r = moves.at(1);
	for (auto move : moves) { // iterates through the available moves, testing execution on pieces
		map = pieces;
		map = executeMove(move, map); // execute on temporary map
		turn = opp; // set to non AI's turn to get the available moves
		strength = getMoveStrength(depth, getAvailableMoves(map), map);
		if (strength >= currentStrength) // chooses move with max strength
			r = move;
	}

	// convert rMove to Move
	Move myMove = rMoveToMove(r);
	executeMove(r, pieces, true);

	//	return Move // Move of choice or default
	return myMove;
}

// register the move on my piece enemyMap
unordered_map<int, rPiece>& RachAI::executeMove(const rMove& m, unordered_map<int, rPiece>& map, bool real) {
	int shift = -2 * m.note + 5; // -1 for 2, 1 for 3
	// take move and conditions into consideration
	switch (m.note) {
	case 0: // inefficient? maybe
		map[m.position] = map.at(m.org); // replace or create
		if (map[m.position].type == 'r' || map[m.position].type == 'e')
			map[m.position].special = false;
		else if (map[m.position].type == 'p' && real)
			pawnSpecialOff.push(m.position);
		break;
	case 1:
		map[m.position] = map.at(m.org);
		map.erase(m.position + 8 * (map.at(m.position).color == 'w' ? -1 : 1)); // erase captured
		break;
	case 2:
	case 3:
		map[m.org].special = false; // end king speciality
		map[m.position] = map[m.org];
		map[m.position + shift] = map[m.position + (m.note * 3 - 8)];
		map[m.position + shift].special = false; // end king speciality
		map.erase(m.position + (m.note * 3 - 8));
		break;
	case 4:
		map[m.position] = map.at(m.org);
		map[m.position].type = 'q';
		map[m.position].value = RVALUES::QUEEN;
		break;
	}
	map.erase(m.org); // erase original
	return map;
}


//////////////////////// Calculations ////////////////////////////

// determine strength of particularly move based on board config/players
int RachAI::getMoveStrength(int depth, vector<rMove> moves, unordered_map<int, rPiece> thisMap) {
	int value = 0; // value of the move
	
	depth /= 2; // gone a level of depth down and thus should do this iteration
	// looking at enemy's moves
	unordered_map<int, rPiece> enemyMap; // working map for opponent
	unordered_map<int, rPiece> myMap; // working map for AI
	vector<rMove> mmoves;
	vector<rMove> nmoves;
	for (auto move : moves) { // go through full map of moves passed down
		enemyMap = thisMap;
		if (enemyMap.count(move.position))
			value -= enemyMap.at(move.position).value * depth; // potential losses 
		value -= ((getPositionValue(move.position, enemyMap.at(move.org).type) 
			- getPositionValue(move.org, enemyMap.at(move.org).type) * depth));
		enemyMap = executeMove(move, enemyMap);
		turn = me; // get the AIs possible follow up moves
		mmoves = getAvailableMoves(enemyMap);

		if (boardTotal(enemyMap) < -500) {
			value -= 20 * depth;
			continue;
		}

		// look at my next moves
		for (auto mmove : mmoves) {
			myMap = enemyMap; // reset map after every iteration forward
			if (myMap.count(mmove.position)) // capture available
				value += myMap.at(mmove.position).value * depth;
			value += (getPositionValue(move.position, myMap.at(mmove.org).type)
				- getPositionValue(move.org, myMap.at(mmove.org).type) * depth);
			myMap = executeMove(mmove, myMap);
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