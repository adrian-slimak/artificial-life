#include "INetwork.h"

#pragma once
class LSTM : public INetwork
{
public:
	Matrix W;
	Matrix U;
	Vector b;

	Matrix Wy;
	Vector by;

	Matrix h;
	Matrix c;
	Matrix z;

public:
	LSTM();
	LSTM(int input_dim, int lstm_units, int output_dim, int batch_size);
	~LSTM();

	void build();
	void build(float* genes);

	void reset();
	void call();
};

