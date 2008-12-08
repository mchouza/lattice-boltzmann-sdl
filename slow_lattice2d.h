#ifndef SLOW_LATTICE2D_H
#define SLOW_LATTICE2D_H

#include "lattice2d.h"

class SlowLattice2D : public Lattice2D
{
	static const int Q = 9;
	static const int DIM = 2;

	float* f_;
	float* fTmp_;
	float* accumBuffer_;

	int nx_, ny_;

	void makeCollisions();
	void makePropagation();

public:
	SlowLattice2D(int nx, int ny, float (*loader)(int x, int y, int i));
	virtual ~SlowLattice2D();

	virtual const float* getData() const;
	virtual void step();
};

#endif
