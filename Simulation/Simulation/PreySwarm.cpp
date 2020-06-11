#include "PreySwarm.h"

#include "Simulation.h"
#include "PredatorSwarm.h"
#include "Distances.h"
#include "LSTM.h"

#include <iostream>

int PreySwarm::population_size = 50;
int PreySwarm::brain_cells = 8;
int PreySwarm::observations_size = 39;
int PreySwarm::actions_size = 2;

float PreySwarm::vision_range = 100.f;
float PreySwarm::vision_range_squared = 100.f*100.f;
float PreySwarm::vision_angle = 182.f;
float PreySwarm::vision_angle_half = 91.f;
int PreySwarm::vision_cells = 13;
float PreySwarm::vision_cell_angle = 14.f;

float PreySwarm::eat_range = 5.f;
int PreySwarm::eat_delay = 10;

float PreySwarm::move_speed = 1.f;
float PreySwarm::turn_speed_rad = 8.f * Distances::deg2rad;

PreySwarm::PreySwarm()
{
	alive = new bool[population_size];

	position = Eigen::MatrixXf(population_size, 2);
	norm = Eigen::MatrixXf(population_size, 2);
	angle = Eigen::VectorXf(population_size);

	target_id = Eigen::VectorXi(population_size);
	eat_delays = Eigen::VectorXi(population_size);

	this->model = new LSTM(observations_size, brain_cells, actions_size, population_size);
	this->model->build();
}

PreySwarm::~PreySwarm()
{
}

void PreySwarm::reset()
{
	number_alive = population_size;
	for (int i = 0; i < population_size; i++)
		alive[i] = true;

	position.setRandom();
	position *= Simulation::world_size_half;
	angle.setRandom();
	angle += Eigen::VectorXf::Constant(population_size, 1.);
	angle *= 3.14;

	fitness = 0.f;
	mean_density = 0.f;
	mean_dispersion = 0.f;

	target_id.setConstant(-1);
	eat_delays.setConstant(0);

	this->model->reset();
}

void PreySwarm::set_model(float* prey_genes)
{
	this->model->build(prey_genes);
}

void PreySwarm::update_observations_preys()
{
	model->x.setConstant(1000000.f);

	for (int self_id = 0; self_id < population_size; self_id++)
	{
		if (alive[self_id])
		{
			// PREYS WITH PREYS
			for (int prey_id = 0; prey_id < population_size; prey_id++)
			{
				if (alive[prey_id] && self_id != prey_id)
				{
					if (distances->prey_distances(self_id, prey_id) < vision_range_squared)
					{
						float angle = calculate_angle(self_id, prey_id);
						if (std::abs(angle) < vision_angle_half)
						{
							int obs_id = int((vision_angle_half - angle) / vision_cell_angle);
							if (distances->prey_distances(self_id, prey_id) < model->x(self_id, obs_id))
								model->x(self_id, obs_id) = distances->prey_distances(self_id, prey_id);
						}
					}
				}
			}

			model->x.row(self_id) = model->x.row(self_id).unaryExpr([this](float elem)
			{
				return elem == 1000000.f ? 0.0f : std::sqrt(elem) / vision_range;
			});
		}
	}
	// Czy da się to przyzpieszyć?? UnaryExpr, biorąc indeks i wykonując powyższe w ten sposób;
}

void PreySwarm::update_observations()
{
	model->x.setConstant(1000000.f);

	float target_closest;

	for (int self_id = 0; self_id < population_size; self_id++)
	{
		if (alive[self_id])
		{
			// PREYS WITH PREYS
			for (int prey_id = 0; prey_id < population_size; prey_id++)
			{
				if (alive[prey_id] && self_id!=prey_id)
				{
					if (distances->prey_distances(self_id, prey_id) < vision_range_squared)
					{
						float angle = calculate_angle(self_id, prey_id);
						if (std::abs(angle) < vision_angle_half)
						{
							int obs_id = int((vision_angle_half - angle) / vision_cell_angle);
							if (distances->prey_distances(self_id, prey_id) < model->x(self_id, obs_id))
								model->x(self_id, obs_id) = distances->prey_distances(self_id, prey_id);
						}
					}
				}
			}

			//PREYS WITH PREDATORS
			for (int predator_id = 0; predator_id < predator_swarm->population_size; predator_id++)
			{
				if (predator_swarm->alive[predator_id])
				{
					if (distances->prey_predator_distances(self_id, predator_id) < vision_range_squared)
					{
						float angle = calculate_angle_predators(self_id, predator_id);
						if (std::abs(angle) < vision_angle_half)
						{
							int obs_id = int((vision_angle_half - angle) / vision_cell_angle) + vision_cells;
							if (distances->prey_predator_distances(self_id, predator_id) < model->x(self_id, obs_id))
								model->x(self_id, obs_id) = distances->prey_predator_distances(self_id, predator_id);
						}
					}
				}
			}

			model->x.row(self_id) = model->x.row(self_id).unaryExpr([this](float elem)
			{
				return elem == 1000000.f ? 0.0f : std::sqrt(elem) / vision_range;
			});
		}
	}
}

float PreySwarm::calculate_angle(int a, int b)
{
	float Ux = position(b, 0) - position(a, 0);
	float Uy = position(b, 1) - position(a, 1);
	return std::atan2((Ux * norm(a, 1)) - (Uy * norm(a, 0)), (Ux * norm(a, 0)) + (Uy * norm(a, 1))) * Distances::rad2deg;
}

float PreySwarm::calculate_angle_predators(int a, int b)
{
	float Ux = predator_swarm->position(b, 0) - position(a, 0);
	float Uy = predator_swarm->position(b, 1) - position(a, 1);
	return std::atan2((Ux * norm(a, 1)) - (Uy * norm(a,0)), (Ux * norm(a, 0)) + (Uy * norm(a, 1))) * Distances::rad2deg;
}

void PreySwarm::update_decisions()
{
	this->model->call();
}

void PreySwarm::update_fitness()
{
	this->fitness += this->number_alive;
}

void PreySwarm::update_stats()
{
	int density = 0;
	float dispersion = 0;
	float temp2;

	for (int self_id = 0; self_id < population_size; self_id++)
	{
		if (alive[self_id])
		{
			temp2 = 1000000.f;

			for (int prey_id = 0; prey_id < population_size; prey_id++)
			{
				if (alive[prey_id] && self_id != prey_id)
				{
					if (this->distances->prey_distances(self_id, prey_id) < 900.f)
						density++;

					if (this->distances->prey_distances(self_id, prey_id) < temp2)
						temp2 = this->distances->prey_distances(self_id, prey_id);
				}
			}

			dispersion += std::sqrt(temp2);
		}
	}

	if (number_alive > 0)
	{
		this->mean_density += (density / number_alive);
		this->mean_dispersion += (dispersion / number_alive);
	}
	//else
	//{
	//	this->mean_density += 0.f;
	//	this->mean_dispersion += 0.f;
	//}
}

void PreySwarm::update_movement()
{
	for (int prey_id = 0; prey_id < population_size; prey_id++)
	{
		if (alive[prey_id])
		{
			angle(prey_id) += model->y(prey_id, 1) * turn_speed_rad;

			angle(prey_id) = angle(prey_id) < 0.0 ? angle(prey_id) + 6.18 : angle(prey_id) > 6.18 ? angle(prey_id) - 6.18 : angle(prey_id);

			float a = angle(prey_id);
			norm(prey_id, 0) = std::cos(a);
			norm(prey_id, 1) = std::sin(a);

			position.row(prey_id) += norm.row(prey_id) * model->y(prey_id, 0) * move_speed;

			position.row(prey_id) = position.row(prey_id).unaryExpr([](float elem)
			{
				return elem < -Simulation::world_size_half ? elem + Simulation::world_size : elem > Simulation::world_size_half ? elem - Simulation::world_size : elem;
			});
		}
	}

	//angle += model->y.col(1) * turn_speed;

	//angle = angle.unaryExpr([](float elem)
	//{
	//	return elem < 0.0 ? elem + 360.0 : elem > 360.0 ? elem - 360.0 : elem;
	//});

	//norm.col(0) = Distances::deg2rad * angle.unaryExpr([](float elem)
	//{
	//	return std::cos(elem);
	//});

	//norm.col(1) = Distances::deg2rad * angle.unaryExpr([](float elem)
	//{
	//	return std::sin(elem);
	//});

	//position.col(0) += norm.col(0).cwiseProduct(model->y.col(0)) * move_speed;
	//position.col(1) += norm.col(1).cwiseProduct(model->y.col(1)) * move_speed;

	//position = position.unaryExpr([](float elem)
	//{
	//	return elem < -Simulation::world_size_half ? elem + Simulation::world_size : elem > Simulation::world_size_half ? elem - Simulation::world_size : elem;
	//});
}
