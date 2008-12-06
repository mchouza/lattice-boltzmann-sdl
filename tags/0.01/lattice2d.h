#ifndef LATTICE2D_H
#define LATTICE2D_H

class Lattice2D
{
	static const int Q = 9;
	float* f_;
	int nx_, ny_;
	int offset[Q];

	void makeCollisions();
	void makePropagation();

public:
	Lattice2D(int nx, int ny, float (*loader)(int x, int y, int i));
	virtual ~Lattice2D();

	const float* getSummedFs() const;

	void step();
};

#endif
