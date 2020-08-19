#include "LSTM.h"

LSTM::LSTM()
{
}

LSTM::LSTM(int input_dim, int lstm_units, int output_dim, int batch_size)
{
	this->input_dim = input_dim;
	this->lstm_units = lstm_units;
	this->output_dim = output_dim;
	this->batch_size = batch_size;

	this->h = Matrix(batch_size, lstm_units);
	this->c = Matrix(batch_size, lstm_units);
	this->z = Matrix(batch_size, lstm_units * 4);

	this->x = Matrix(batch_size, input_dim);
	this->y = Matrix(batch_size, output_dim);
}

LSTM::~LSTM()
{
}

void LSTM::build()
{
	this->W = Matrix::Random(input_dim, lstm_units * 4);
	this->U = Matrix::Random(lstm_units, lstm_units * 4);
	this->b = Vector::Random(lstm_units * 4);

	this->Wy = Matrix::Random(lstm_units, output_dim);
	this->by = Vector::Random(output_dim);
}

void LSTM::build(float* genes)
{
    //this->W.resize(0,0);
    //this->U.resize(0,0);
    //this->b.resize(0,0);
    //this->Wy.resize(0,0);
    //this->by.resize(0,0);

	this->W = Eigen::Map<Eigen::MatrixXf>(genes, input_dim, lstm_units * 4);
	genes += W.size();
	this->U = Eigen::Map<Eigen::MatrixXf>(genes, lstm_units, lstm_units * 4);
	genes += U.size();
	this->b = Eigen::Map<Eigen::VectorXf>(genes, lstm_units * 4);
	genes += b.size();

	this->Wy = Eigen::Map<Eigen::MatrixXf>(genes, lstm_units, output_dim);
	genes += Wy.size();
	this->by = Eigen::Map<Eigen::VectorXf>(genes, output_dim);
}

void LSTM::reset()
{
	this->h.setZero();
	this->c.setZero();
	this->z.setZero();

	this->x.setZero();
	this->y.setZero();
}

inline float logistic(const float x) 
{
	return 1.0f / (1.0f + ::expf(-x));
}

void LSTM::call()
{
	z = (x * W + h * U).rowwise() + b.transpose();
	//z.block(0, 0, batch_size, lstm_units * 3) = z.block(0, 0, batch_size, lstm_units * 3).unaryExpr((float(*)(const float))logistic);
	z.block(0, 0, batch_size, lstm_units * 3) = 1.f/(1.f+(-z.block(0, 0, batch_size, lstm_units * 3)).array().exp());
	//z.block(0, 3 * lstm_units, batch_size, lstm_units) = z.block(0, 3 * lstm_units, batch_size, lstm_units).unaryExpr(std::ptr_fun(::tanhf));
	z.block(0, 3 * lstm_units, batch_size, lstm_units) = z.block(0, 3 * lstm_units, batch_size, lstm_units).array().tanh();

	c = z.block(0, 2 * lstm_units, batch_size, lstm_units).cwiseProduct(c) + z.block(0, 0, batch_size, lstm_units).cwiseProduct(z.block(0, 3 * lstm_units, batch_size, lstm_units));

	//h = z.block(0, lstm_units, batch_size, lstm_units).cwiseProduct(c.unaryExpr(std::ptr_fun(::tanhf)));
	h = z.block(0, lstm_units, batch_size, lstm_units).cwiseProduct(c.array().tanh().matrix());

	y = (h * Wy).rowwise() + by.transpose();
	//y = y.unaryExpr(std::ptr_fun(::tanhf));
	y = y.array().tanh();
}
