#ifndef LATTICE2D_H
#define LATTICE2D_H

class Lattice2D
{
	static const int Q = 9;
	float* f_;
	float* shadowF_;
	int nx_, ny_;

	void makeCollisions();
	void makePropagation();

	void normalize(int& x, int& y) const;
	float& getShadowF(int x, int y, int i);

public:
	Lattice2D(int nx, int ny, float (*loader)(int x, int y, int i));
	virtual ~Lattice2D();

	void step();

	float getF(int x, int y, int i) const;
	float getSumF(int x, int y) const;
};

inline float Lattice2D::getF(int x, int y, int i) const
{
	normalize(x, y);
	return f_[i + (x + y * nx_) * Q ];
}

inline float Lattice2D::getSumF(int x, int y) const
{
	normalize(x, y);
	float accum = 0.0;
	float* base = f_ + (x + y * nx_) * Q;
	for (int i = 0; i < Q; i++)
		accum += *base++;
	return accum;
}

inline float& Lattice2D::getShadowF(int x, int y, int i)
{
	normalize(x, y);
	return shadowF_[i + (x + y * nx_) * Q];
}

#endif
