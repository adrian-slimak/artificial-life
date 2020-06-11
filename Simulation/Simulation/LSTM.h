#include <Eigen\Dense>

typedef Eigen::MatrixXf Matrix;
typedef Eigen::Matrix<float, -1, -1, Eigen::RowMajor> MatrixRW;
typedef Eigen::VectorXf Vector;

#pragma once
class LSTM
{
public:
	int input_dim;
	int lstm_units;
	int output_dim;
	int batch_size;

	Matrix W;
	Matrix U;
	Vector b;

	Matrix Wy;
	Vector by;

	Matrix h;
	Matrix c;
	Matrix z;

	Matrix x;
	Matrix y;

public:
	LSTM();
	LSTM(int input_dim, int lstm_units, int output_dim, int batch_size);
	~LSTM();

	void build();
	void build(float* genes);

	void reset();
	void call();
};

