#ifndef FAST_LATTICE2D_H
#define FAST_LATTICE2D_H

#include "lattice2d.h"

class FastLattice2D : public Lattice2D
{
	static const int Q = 9;
	static const int DIM = 2;

	int offsets_[Q];

	real_t* f_;
	real_t* accumBuffer_;

	mutable bool accumBufferUpdated_;

	int n_;

	void makeCollisions();
	void makePropagation();
	void updAccumBuffer() const;

public:
	FastLattice2D(int n, 
		void (*loader)(int x, int y, real_t& rho, real_t& ux, real_t& uy));
	virtual ~FastLattice2D();

	virtual const real_t* getData() const;
	virtual void step();
};

#endif
