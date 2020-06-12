#include <Eigen\Dense>

typedef Eigen::MatrixXf Matrix;
typedef Eigen::Matrix<float, -1, -1, Eigen::RowMajor> MatrixRW;
typedef Eigen::VectorXf Vector;

enum NetworkType {_LSTM, _RNN};

#pragma once
class INetwork
{
public:
	int input_dim;
	int lstm_units;
	int output_dim;
	int batch_size;

	Matrix x;
	Matrix y;

public:
	virtual void build() = 0;
	virtual void build(float* genes) = 0;

	virtual void reset() = 0;
	virtual void call() = 0;
};

