#include "PreySwarm.h"

#include "Simulation.h"
#include "PredatorSwarm.h"
#include "Distances.h"
#include "LSTM.h"
#include "RNN.h"

#include <iostream>

int PreySwarm::population_size = 50;
int PreySwarm::brain_cells = 8;
NetworkType PreySwarm::network_type = NetworkType::_LSTM;
int PreySwarm::observations_size = 39;
int PreySwarm::actions_size = 2;

float PreySwarm::vision_range = 100.f;
float PreySwarm::vision_range_squared = 100.f*100.f;
float PreySwarm::vision_angle = 182.f;
float PreySwarm::vision_angle_half_rad = 91.f * Distances::deg2rad;
int PreySwarm::vision_cells = 13;
float PreySwarm::vision_cell_angle_rad = 14.f * Distances::deg2rad;

bool PreySwarm::hear_enabled = false;
float PreySwarm::hear_range = 150.f;
float PreySwarm::hear_range_squared = 150.f*150.f;
int PreySwarm::hear_cells = 12;
float PreySwarm::hear_cell_angle_rad = 30.f * Distances::deg2rad;

bool PreySwarm::communication_enabled = false;

float PreySwarm::eat_range = 5.f;
int PreySwarm::eat_delay = 10;

float PreySwarm::move_speed = 1.f;
float PreySwarm::turn_speed_rad = 8.f * Distances::deg2rad;

PreySwarm::PreySwarm()
{
	alive = new bool[PreySwarm::population_size];

	position = Eigen::ArrayXXf(PreySwarm::population_size, 2);

	norm = Eigen::ArrayXXf(PreySwarm::population_size, 2);

	angle = Eigen::ArrayXf(PreySwarm::population_size);

	eat_delays = new int[PreySwarm::population_size];

	if (network_type == NetworkType::_LSTM)
		this->model = new LSTM(observations_size, brain_cells, actions_size, population_size);
	else
		this->model = new RNN(observations_size, brain_cells, actions_size, population_size);

	this->model->build();
}

PreySwarm::~PreySwarm()
{

	delete[] eat_delays;
}

void PreySwarm::reset()
{
	number_alive = population_size;
	for (int i = 0; i < population_size; i++)
		alive[i] = true;

	position.setRandom();
	position *= Simulation::world_size_half;

	//for (int i = 0; i < PreySwarm::population_size; i++)
	//	angle[i] = ((float)std::rand() / (RAND_MAX)) * 6.18f;
	angle.setRandom();
	angle += 1.f;
	angle *= 3.14;

	fitness = 0.f;
	mean_density = 0.f;
	mean_dispersion = 0.f;

	for (int i = 0; i < PreySwarm::population_size; i++)
		eat_delays[i] = 0;

	this->model->reset();
}

void PreySwarm::set_model(float* prey_genes)
{
	this->model->build(prey_genes);
}

void PreySwarm::update_decisions()
{
	this->model->call();
	//this->model->y.setZero();
}

void PreySwarm::update_fitness()
{
	this->fitness += this->number_alive;
}

void PreySwarm::update_stats()
{
	int density = 0;
	float dispersion = 0.f;
	float min_dist;

	// Tutaj te¿ da siê przyspieszyæ, poniewa¿ a->b oraz b->a to ten sam dystans...
	for (int self_id = 0; self_id < population_size; self_id++)
	{
		if (alive[self_id])
		{
			min_dist = 1000000.f;

			for (int prey_id = 0; prey_id < population_size; prey_id++)
			{
				if (alive[prey_id] && self_id != prey_id)
				{
					if (this->distances->prey_distances[self_id][prey_id] < 900.f)
						density++;

					if (this->distances->prey_distances[self_id][prey_id] < min_dist)
						min_dist = this->distances->prey_distances[self_id][prey_id];
				}
			}

			dispersion += std::sqrt(min_dist);
		}
	}

	if (number_alive > 0)
	{
		this->mean_density += (density / (float)number_alive);
		this->mean_dispersion += (dispersion / (float)number_alive);
	}
}

void PreySwarm::update_movement()
{
	//for (int p = 0; p < population_size; p++)
	//{
	//	if (alive[p])
	//	{
	//		angle[p] += model->y(p, 1) * turn_speed_rad;

	//		angle[p] = angle[p] < 0.0f ? angle[p] + 6.18f : angle[p] > 6.18f ? angle[p] - 6.18f : angle[p];

	//		norm(p, 0) = std::cos(angle[p]);
	//		norm(p, 1) = std::sin(angle[p]);

	//		position.row(p) += norm.row(p) * model->y(p, 0) * move_speed;
	//	}
	//}


	angle += model->y.col(1).array() * turn_speed_rad;

	angle = angle.unaryExpr([](float elem)
	{
		return elem < 0.0f ? elem + 6.18f : elem > 6.18f ? elem - 6.18f : elem;
	});

	norm.col(0) = angle.cos();
	norm.col(1) = angle.sin();

	position += norm.colwise() * model->y.col(0).array() * move_speed;

	position = position.unaryExpr([](float elem)
	{
		return elem < -Simulation::world_size_half ? elem + Simulation::world_size : elem > Simulation::world_size_half ? elem - Simulation::world_size : elem;
	});
}
