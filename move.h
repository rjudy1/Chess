#pragma once
using namespace std;

class Move {
public:
	// Soure location (x, y) and Destination location (x, y)
	// Use ints 0 through 7 rather than A through H
	int xs, ys, xd, yd;
	Move(int xs, int ys, int xd, int yd) : xs(xs), ys(ys), xd(xd), yd(yd) {};
};