#ifndef LATTICE2D_H
#define LATTICE2D_H

class Lattice2D
{
public:
	virtual const float* getData() const = 0;
	virtual void step() = 0;
};

#endif
