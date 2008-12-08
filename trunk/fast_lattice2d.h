#ifndef FAST_LATTICE2D_H
#define FAST_LATTICE2D_H

#include "lattice2d.h"

class FastLattice2D : public Lattice2D
{
	static const int Q = 9;
	static const int DIM = 2;

	float* f_;
	int nx_, ny_;
	int offset_[Q];
	mutable bool freshAggrData_;
	float* aggrData_;

	void updAggrData() const;
	void makeCollisions();
	void makePropagation();

public:
	FastLattice2D(int nx, int ny, float (*loader)(int x, int y, int i));
	virtual ~FastLattice2D();

	const float* getData() const;

	void step();
};

#endif
