#include "INetwork.h"

#pragma once
class RNN : public INetwork
{
public:
	Matrix W;
	Matrix U;
	Vector b;

	Matrix Wy;
	Vector by;

	Matrix h;

public:
	RNN();
	RNN(int input_dim, int lstm_units, int output_dim, int batch_size);
	~RNN();

	void build();
	void build(float* genes);

	void reset();
	void call();
};

