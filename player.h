#pragma once
#include "move.h"

class Player {
public:
	virtual void Init(bool isWhite) {};
	virtual Move GetMove(Move opMove) { return Move(0, 0, 0, 0); };
};