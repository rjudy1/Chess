/*
Chess AI
Author:		Rachael Judy
Purpose:	Chess AI
Date:		5/16/2020
Updated:	7/5/2020
Notes:
	- Board 0-63
	- coordinates are y, x start in lower left
	- start NW and work way around clockwise for checking possible moves

	- tune the piece square tables and weights
	- still not getting through enough moves - small optimizations - particularly in getAvailableMoves - run timing check
	- sacrifices wrong piece to defend the king

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


#include "RachAI.h"

using namespace std;


// sets initial positions
void RachAI::Init(bool isWhite) {
	me = (isWhite ? 'w' : 'b'); // me is the AI
	opp = (isWhite ? 'b' : 'w'); // sets opponent and turn to me
	high = (int)isWhite - (int)!isWhite; // white is +1

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

	// sets values for positions
	ePosValues = {
		-3, -5, -4, -5, -5, -4, -5, -3,
		-3, -4, -4, -5, -5, -4, -4, -3,
		-4, -4, -4, -5, -5, -4, -4, -4,
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
		 -4, -2,  0,  1,  1,  0, -2, -4,
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
	 	12, 12, 12, 12, 12, 12, 12, 12,
		 7,  7,  7,  7,  7,  7,  7,  7,
		 2,  2,  4,  6,  6,  4,  2,  2,
		 1,  1,  2,  6,  6,  2,  1,  1,
		 0,  0,  0,  5,  5,  0,  0,  0,
		 1, -1,  1,  1,  1,  1, -1,  1,
		 1,  2,  2, -5, -5,  2,  2,  1,
		 0,  0,  0,  0,  0,  0,  0,  0
	};
	
	if (me == 'b') { // flip the tables for playing as black
		reverse(ePosValues.begin(), ePosValues.end());
		reverse(qPosValues.begin(), qPosValues.end());
		reverse(bPosValues.begin(), bPosValues.end());
		reverse(kPosValues.begin(), kPosValues.end());
		reverse(rPosValues.begin(), rPosValues.end());
		reverse(pPosValues.begin(), pPosValues.end());
	}
}

// return a calculated Move to the game
Move RachAI::GetMove(Move opMove) {
	auto start = chrono::high_resolution_clock::now();

	// convert to rMove type
	rMove r = MovetoRMove(opMove);

	// turn off special for en passant, queued pawns
	while (!pawnSpecialOff.empty()) {
		int top = pawnSpecialOff.front();
		pieces.at(top).special = false;
		pawnSpecialOff.pop();
	}

	// add players move to the configuration
	int temp = 0;
	if (r.org != r.position)
		executeMove(r, pieces, temp, true);

	// set turn to this player
	turn = me;
	high = me == 'w' ? 1 : -1;

	// generate the available moves, then search to requested depth
	vector<rMove> moves;
	moves.reserve(50);
	unordered_map<int, rPiece> map = pieces; // do we need this duplicate, or could we restore it?
	getAvailableMoves(map, moves); // the AI's available moves
	double strength = 0;
	double currentStrength = -5000.0;
	double depth = getDepthVariable(); 

	// iterates through the available moves, testing execution on pieces
	r = moves[0];
	int counter = 0; // for the restoreStack
	double weight = 0; // weight the following moves based on first move
	for (auto& move : moves) {
		weight = 1.0;
		strength = 0.0;
		turn = me;
		high = 1;

		if (map.find(move.position) != map.end()) { // possible capture
			strength += static_cast<int64_t>(map.at(move.position).value) * depth * 1.5;
			weight += strength / (30 * depth);
		}
		strength += ((getPositionValue(move.position, map.at(move.org).type)
			- getPositionValue(move.org, map.at(move.org).type)) * high * depth) + move.note * depth;
		weight += strength / (180 * depth);
		executeMove(move, map, counter); // execute on temporary map
		
		strength -= getMoveStrength(depth, map) * weight; // view return moves

		if (strength >= currentStrength) { // chooses move with max strength
			currentStrength = strength;
			r = move;
		}

		// restore map
		while (counter > 0) {
			restoreStack.top().restore(map);
			restoreStack.pop();
			counter--;
		}

		// end search if time has been exceeded
		if (chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start).count() > 14.98)
			break;
	}

	// convert rMove to Move
	Move myMove = rMoveToMove(r);
	executeMove(r, pieces, temp, true);

	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = finish - start;
//	wxMessageBox(to_string(elapsed.count()), "Timing (s)"); // display timing to see how long move deliberation is taking

	// Move of choice or default
	return myMove;
}

// register the move on the working piece map
void RachAI::executeMove(const rMove& m, unordered_map<int, rPiece>& map, int& mods, bool real) {
	// add the piece we just moved back
	restoreStack.push(rRestorePoint(map.at(m.org), m.org, true)); // readd the original
	mods++;
	int shift;

	// take move and conditions into consideration
	switch (m.note) {
	case 0:
		if (map.find(m.position) != map.end()) // if there's a capture
			restoreStack.push(rRestorePoint(map.at(m.position), m.position, true)); // readd the captured
		else
			restoreStack.push(rRestorePoint(map.at(m.org), m.position, false)); // remove the new piece
		mods++;

		map[m.position] = map.at(m.org); // replace or create
		if (map[m.position].type == 'r' || map[m.position].type == 'e')
			map[m.position].special = false;
		else if (map[m.position].type == 'p' && real)
			pawnSpecialOff.push(m.position);
		break;
	case 1:
		restoreStack.push(rRestorePoint(map.at(m.org), m.position, false)); // remove the new
		mods++;

		map[m.position] = map.at(m.org);

		restoreStack.push(rRestorePoint(map.at(m.position + 8 * (map.at(m.position).color == 'w' ? 1 : -1)),
			m.position + 8 * (map.at(m.position).color == 'w' ? 1 : -1), true)); // restore the captured
		mods++;

		map.erase(m.position + 8 * (map.at(m.position).color == 'w' ? 1 : -1)); // erase captured
		break;
	case 2:
	case 3:
		shift = 2 * m.note - 5; // -1 for 2, 1 for 3
		restoreStack.push(rRestorePoint(map.at(m.org), m.position, false)); // remove the new rook
		restoreStack.push(rRestorePoint(map.at(m.position - shift), m.position - shift, true)); // re add original king
		restoreStack.push(rRestorePoint(map.at(m.position - shift), m.position + shift, false)); // remove the new king
		mods += 3;

		map[m.org].special = false; // end rook speciality
		map[m.position] = map[m.org];
		map[m.position + shift] = map[m.position - shift];
		map[m.position + shift].special = false; // end king speciality
		map.erase(m.position - shift);
		break;
	case 4:
		if (map.find(m.position)!= map.end()) // if there's a capture
			restoreStack.push(rRestorePoint(map.at(m.position), m.position, true)); // readd the captured
		else
			restoreStack.push(rRestorePoint(map.at(m.org), m.position, false)); // remove the new queen
		mods++;

		map[m.position] = map.at(m.org);
		map[m.position].type = 'q';
		map[m.position].value = RVALUES::QUEEN;
		break;
	}

	if (real) {
		while (!restoreStack.empty())
			restoreStack.pop();
		mods = 0;
	}
	map.erase(m.org); // erase original
}


//////////////////////// Calculations ////////////////////////////

// determine strength of particularly move based on board config/players
double RachAI::getMoveStrength(double depth, unordered_map<int, rPiece>& thisMap) {
	double value = 0; // value of the move vector
	double strength = 0; // value of this specific move
	int stackCounter = 0; // count how many undos to do
	depth /= WEIGHT; // gone a level of depth down and thus should do this iteration
	double moveWeight = 0;

	toggleTurn();
	vector<rMove> moves;
	moves.reserve(50);
	getAvailableMoves(thisMap, moves);
	for (auto& move : moves) { // go through full map of moves
		moveWeight = 1;
		strength = 0;
		if (thisMap.find(move.position) != thisMap.end()) {
			strength += static_cast<int64_t>(thisMap.at(move.position).value) * depth * 1.5;  // potential losses 
			moveWeight += strength / depth / 29;
		}
		strength += ((getPositionValue(move.position, thisMap.at(move.org).type)
			- getPositionValue(move.org, thisMap.at(move.org).type))) * high * depth + move.note *depth;

		moveWeight += (strength / depth) / 175;
		value += strength;
		executeMove(move, thisMap, stackCounter);

		while (stackCounter > 0) {
			restoreStack.top().restore(thisMap);
			restoreStack.pop();
			stackCounter--;
		}
		if (value / (depth * 1.5) > 75) { // king loss?
			value += (double)WEIGHT / 15 * depth * ((int)(turn == opp) + 1);
			break;
		}
		if (value / depth > 20 && turn == opp)
		{
			value += 500 * depth;
			break;
		}

		if (depth > 4e-4)
			value -= getMoveStrength(depth, thisMap) * moveWeight;
		else if (TARGET_DEPTH % 2 == 1)
			toggleTurn();
	}
	return value;
}

// gets a vector of possible moves
void RachAI::getAvailableMoves(const unordered_map<int, rPiece>& map, vector<rMove>& moves, bool castleCheck) {
	moves.clear();
	for (auto& p : map) {
		if (p.second.color == turn) {
			int pos = p.first;
			switch (p.second.type) {
			case 'e': // king, consider castling as rook moves
				// one square any way
				for (auto& x : rShifts) {
					if (checkShift(pos, x)) {
						if (map.find(pos + x) != map.end()) { // capture available 
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
				for (auto& x : rJumps) {
					if (checkJump(pos, x)) {
						if (map.find(pos + x) != map.end()) { // possible capture available
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
					int king = 4 + 56 * (turn == 'w');
					int note = 2 + (p.first % 8 == 7); // queenside or kingside
					bool open = ((note == 3 && (map.find(p.first - 2) == map.end()) && (map.find(p.first - 1) == map.end())) ||
						(note == 2 && map.find(p.first + 1) == map.end() && map.find(p.first + 2) == map.end() && map.find(p.first + 3) == map.end()));
					pos = king + 1 - 2 * (p.first % 8 == 0); // position rook would move to
					if (open && map.find(king) != map.end() && map.at(king).special && !inCastlingDanger(map, note, king))
						moves.push_back(rMove{ p.first, pos, note });
				}
				break;

			case'p': // en passant capture, always forward, capture/move diff
				int dir = (turn == 'w' ? -1 : 1);
				int note = 4 * (pos + 8 * dir >= 57 || pos + 8 * dir <= 8); // promote
				// forward
				if (!castleCheck && map.find(pos + 8 * dir) == map.end()) {// forward 
					moves.push_back(rMove{ p.first, pos + 8 * dir, note });
					if (map.find(pos + 16 * dir) == map.end() && p.second.special) // beginning of next turn, remove speciality, add to queue on execution
						moves.push_back(rMove{ p.first, pos + 16 * dir, note });
				}

				// forward and diagonal to capture /////////// need to cehck if pawn, not king or rook, and also precedence
				if ((map.find(pos + 8 * dir + 1) != map.end() && map.at(pos + 8 * dir + 1).color != map.at(pos).color
					|| map.find(pos + 1) != map.end() && map.at(pos + 1).special && map.at(pos + 1).color != map.at(pos).color)
					&& checkShift(pos, 8 * dir + 1)) { // capture available
					moves.push_back(rMove{ p.first, pos + 8 * dir + 1, note });
				}
				if ((map.find(pos + 8 * dir - 1) != map.end() && map.at(pos + 8 * dir - 1).color != map.at(pos).color  
					|| map.find(pos - 1) != map.end() && map.at(pos - 1).special && map.at(pos-1).color != map.at(pos).color)
					&& checkShift(pos, 8 * dir - 1) && pos < 40 && pos >= 24) {// capture available
					moves.push_back(rMove{ p.first, pos + 8 * dir - 1, note });
				
				}
				break;
			}	
		}
	}
}


///////////////////// HELPER Functions to check limits ////////////////////////

void RachAI::toggleTurn() {
	turn = turn == me ? opp : me;
	high *= (turn == me ? 1 : -1);
}

// check if the path of the king is threatened
bool RachAI::inCastlingDanger(const unordered_map<int, rPiece>& map, const int& side, const int& kingPos) {
	// check king square and square beside king
	toggleTurn(); // set turn temporarily to other player
	vector<rMove> dangers;
	dangers.reserve(30);
	bool danger = false;
	getAvailableMoves(map, dangers, true); // this is an expensive choice
	for (auto& d : dangers) {
		if (d.position == kingPos || d.position == kingPos + (2 * side - 5))
			danger = true;
	}
	toggleTurn(); // return turn back to current player
	return danger;
}

long long RachAI::getPositionValue(int& pos, char& piece) {
	switch (piece) {
	case 'e':
		return static_cast<long long>(ePosValues.at(pos));
	case 'q':
		return static_cast<long long>(qPosValues.at(pos));
	case 'b':
		return static_cast<long long>(bPosValues.at(pos));
	case 'k':
		return static_cast<long long>(kPosValues.at(pos));
	case 'r':
		return static_cast<long long>(rPosValues.at(pos));
	case 'p':
		return static_cast<long long>(pPosValues.at(pos));
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
		if (map.find(pos) != map.end()) { // capture available
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
	Move m(0, 0, 0, 0);
	if (myMove.note == 2) {
		m = { (myMove.org + 4) % 8, 7 - (myMove.org + 4) / 8, (myMove.position - 1) % 8, 7 - (myMove.position - 1) / 8 };
	}
	else if (myMove.note == 3)	{
		m = { (myMove.org - 3) % 8, 7 - (myMove.org - 3) / 8, (myMove.position + 1) % 8, 7 - (myMove.position + 1) / 8 };
	}
	else
		m = { myMove.org % 8, 7 - (myMove.org / 8), myMove.position % 8, 7 - (myMove.position / 8) };
	return m;
}

// might need to upgrade to find out what its special note is
rMove RachAI::MovetoRMove(Move& opMove) {
	int special = 0;
	int source = (7 - opMove.ys) * 8 + opMove.xs;
	int dest = (7 - opMove.yd) * 8 + opMove.xd;
	if (pieces.at(source).type == 'e' && abs(opMove.xd - opMove.xs) == 2)
		if (opMove.xd - opMove.xs == 2) {// do this for all moves or my exclusive move 
			special = 3;
			source += 3;
			dest -= 1;
		}
		else {
			special = 2;
			source -= 4;
			dest += 1;
		}
	else if (pieces.find(dest) == pieces.end() && opMove.xs != opMove.xd
		&& pieces.at(source).type == 'p')
		special = 1;
	else if (pieces.at(source).type == 'p' && (dest >= 56 || dest < 8))
		special = 4;

	rMove m = { source, dest, special };
	return m;
}

double RachAI::getDepthVariable() {
	double d = WEIGHT;
	for (int i = 0; i < TARGET_DEPTH - 2; i++)
		d *= WEIGHT;
	return 3.98e-4*d;
}