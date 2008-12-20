#ifndef SLOW_LATTICE2D_H
#define SLOW_LATTICE2D_H

#include "lattice2d.h"

class SlowLattice2D : public Lattice2D
{
	static const int Q = 9;
	static const int DIM = 2;

	real_t* f_;
	real_t* fTmp_;
	real_t* accumBuffer_;

	int n_;

	void makeCollisions();
	void makePropagation();
	void updAccumBuffer();

public:
	SlowLattice2D(int n, 
		void (*loader)(int x, int y, real_t& rho, real_t& ux, real_t& uy));
	virtual ~SlowLattice2D();

	virtual const real_t* getData() const;
	virtual void step();
};

#endif
