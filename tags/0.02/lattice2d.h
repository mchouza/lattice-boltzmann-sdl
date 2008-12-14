#ifndef LATTICE2D_H
#define LATTICE2D_H

typedef float real_t;

class Lattice2D
{
public:
	virtual const real_t* getData() const = 0;
	virtual void step() = 0;
};

#endif
