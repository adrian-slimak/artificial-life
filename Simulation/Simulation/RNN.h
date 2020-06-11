#include <Eigen\Dense>

typedef Eigen::MatrixXf Matrix;
typedef Eigen::Matrix<float, -1, -1, Eigen::RowMajor> MatrixRW;
typedef Eigen::VectorXf Vector;

#pragma once
class RNN
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

	Matrix x;
	Matrix y;

public:
	RNN();
	RNN(int input_dim, int lstm_units, int output_dim, int batch_size);
	~RNN();

	void build();
	void build(float* genes);

	void reset();
	void call();
};

