#include "RNN.h"
//#include <omp.h>
#include <iostream>
RNN::RNN()
{
}

RNN::RNN(int input_dim, int lstm_units, int output_dim, int batch_size)
{
	this->input_dim = input_dim;
	this->lstm_units = lstm_units;
	this->output_dim = output_dim;
	this->batch_size = batch_size;

	this->h = Matrix(batch_size, lstm_units);

	this->x = Matrix(batch_size, input_dim);
	this->y = Matrix(batch_size, output_dim);
}

RNN::~RNN()
{
}

void RNN::build()
{
	this->W = Matrix::Random(input_dim, lstm_units);
	this->U = Matrix::Random(lstm_units, lstm_units);
	this->b = Vector::Random(lstm_units);

	this->Wy = Matrix::Random(lstm_units, output_dim);
	this->by = Vector::Random(output_dim);
}

void RNN::build(float* genes)
{
	//this->W.resize(0,0);
	//this->U.resize(0,0);
	//this->b.resize(0,0);
	//this->Wy.resize(0,0);
	//this->by.resize(0,0);

	this->W = Eigen::Map<Eigen::MatrixXf>(genes, input_dim, lstm_units);
	genes += W.size();
	this->U = Eigen::Map<Eigen::MatrixXf>(genes, lstm_units, lstm_units);
	genes += U.size();
	this->b = Eigen::Map<Eigen::VectorXf>(genes, lstm_units);
	genes += b.size();

	this->Wy = Eigen::Map<Eigen::MatrixXf>(genes, lstm_units, output_dim);
	genes += Wy.size();
	this->by = Eigen::Map<Eigen::VectorXf>(genes, output_dim);
}

void RNN::reset()
{
	this->h.setZero();

	this->x.setZero();
	this->y.setZero();
}

inline float logistic(const float x)
{
	return 1.0f / (1.0f + ::expf(-x));
}

void RNN::call()
{
	//h = ((x * W + h * U).rowwise() + b.transpose()).unaryExpr(std::ptr_fun(::tanhf));
	h = ((x * W + h * U).rowwise() + b.transpose()).array().tanh();

	y = (h * Wy).rowwise() + by.transpose();
	y = y.array().tanh();
	//y = y.unaryExpr(std::ptr_fun(::tanhf));
}
