// Structures for the chess AI - Rachael

#include <vector>
#include <unordered_map>
#include <queue>

#ifndef STRUCTURES_CPP
#define STRUCTURES_CPP

enum RVALUES {
	PAWN = 1,
	ROOK = 5,
	KNIGHT = 3, //maybe 4?
	BISHOP = 3,
	QUEEN = 9, // maybe 8 or 10?
	KING = 500
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

#endif