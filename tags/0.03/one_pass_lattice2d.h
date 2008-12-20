#ifndef ONE_PASS_LATTICE2D_H
#define ONE_PASS_LATTICE2D_H

#include "lattice2d.h"

class OnePassLattice2D : public Lattice2D
{
	static const int Q = 9;
	static const int DIM = 2;

	int offsets_[Q];

	real_t* f_;
	real_t* accumBuffer_;

	int n_;

	void makeCollisions();
	void makePropagation();

public:
	OnePassLattice2D(int n, 
		void (*loader)(int x, int y, real_t& rho, real_t& ux, real_t& uy));
	virtual ~OnePassLattice2D();

	virtual const real_t* getData() const;
	virtual void step();
};

#endif
