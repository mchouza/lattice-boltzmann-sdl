#ifndef LATTICE2D_H
#define LATTICE2D_H

class Lattice2D
{
	static const int Q = 9;
	float* f_;
	float* buffer_;
	int nx_, ny_;

	void makeCollisions();
	void makePropagation();

public:
	Lattice2D(int nx, int ny, float (*loader)(int x, int y, int i));
	virtual ~Lattice2D();

	void getSummedFs(float* buffer) const;

	void step();
};

#endif
