#include "PredatorSwarm.h"

#include "Simulation.h"
#include "PreySwarm.h"
#include "Distances.h"
#include "LSTM.h"
#include "RNN.h"

#include <iostream>

int PredatorSwarm::population_size = 4;
int PredatorSwarm::brain_cells = 8;
NetworkType PredatorSwarm::network_type = NetworkType::_LSTM;
int PredatorSwarm::observations_size = 39;
int PredatorSwarm::actions_size = 2;

float PredatorSwarm::vision_range = 100.f;
float PredatorSwarm::vision_range_squared = 100.f*100.f;
float PredatorSwarm::vision_angle = 182.f;
float PredatorSwarm::vision_angle_half_rad = 91.f * Distances::deg2rad;
int PredatorSwarm::vision_cells = 13;
float PredatorSwarm::vision_cell_angle_rad = 14.f * Distances::deg2rad;

bool PredatorSwarm::hear_enabled = false;
float PredatorSwarm::hear_range = 150.f;
float PredatorSwarm::hear_range_squared = 150.f*150.f;
int PredatorSwarm::hear_cells = 12;
float PredatorSwarm::hear_cell_angle_rad = 30.f * Distances::deg2rad;

bool PredatorSwarm::communication_enabled = false;

float PredatorSwarm::eat_range = 5.f;
float PredatorSwarm::eat_range_squared = 5.f*5.f;
int PredatorSwarm::eat_delay = 10;

float PredatorSwarm::move_speed = 3.f;
float PredatorSwarm::turn_speed_rad = 6.f  * Distances::deg2rad;

float PredatorSwarm::confusion_range = 30.f;
float PredatorSwarm::confusion_range_squared = 30.f*30.f;
float PredatorSwarm::confusion_ratio = 1.f;

PredatorSwarm::PredatorSwarm()
{
	alive = new bool[population_size];

	position = Eigen::MatrixXf(population_size, 2);
	norm = Eigen::MatrixXf(population_size, 2);
	angle = Eigen::VectorXf(population_size);

	target_id = Eigen::VectorXi(population_size);
	eat_delays = Eigen::VectorXi(population_size);

	if(network_type == NetworkType::_LSTM)
		this->model = new LSTM(observations_size, brain_cells, actions_size, population_size);
	else
		this->model = new RNN(observations_size, brain_cells, actions_size, population_size);

	this->model->build();
}


PredatorSwarm::~PredatorSwarm()
{
}

void PredatorSwarm::reset()
{
	number_alive = population_size;
	for (int i = 0; i < population_size; i++)
		alive[i] = true;

	position.setRandom();
	position *= Simulation::world_size_half;
	angle.setRandom();
	angle += Eigen::VectorXf::Constant(population_size, 1.);
	angle *= 3.14f;

	fitness = 0.f;
	mean_density = 0.f;
	mean_dispersion = 0.f;
	number_attacks = 0;
	number_hunts = 0;

	target_id.setConstant(-1);
	eat_delays.setConstant(0);

	this->model->reset();
}

void PredatorSwarm::set_model(float* predator_genes)
{
	this->model->build(predator_genes);
}

void PredatorSwarm::update_observations()
{
	model->x.setConstant(1000000.f);

	float target_closest;

	for (int self_id = 0; self_id < population_size; self_id++)
	{
		if (alive[self_id])
		{
			// PREDATORS WITH PREDATORS
			for (int predator_id = 0; predator_id < population_size; predator_id++)
			{
				if (alive[predator_id] && self_id != predator_id)
				{
					if (distances->predator_distances(self_id, predator_id) < vision_range_squared)
					{
						float angle = calculate_angle(self_id, predator_id);
						if (std::abs(angle) < vision_angle_half_rad)
						{
							int obs_id = int((vision_angle_half_rad - angle) / vision_cell_angle_rad) + vision_cells;
							if (distances->predator_distances(self_id, predator_id) < model->x(self_id, obs_id))
								model->x(self_id, obs_id) = distances->predator_distances(self_id, predator_id);
						}
					}
				}
			}

			target_closest = 1000000.f;
			target_id[self_id] = -1;
			// PREDATORS WITH PREYS
			for (int prey_id = 0; prey_id < prey_swarm->population_size; prey_id++)
			{
				if (prey_swarm->alive[prey_id])
				{
					if (distances->prey_predator_distances(prey_id, self_id) < vision_range_squared)
					{
						float angle = calculate_angle_preys(self_id, prey_id);
						if (std::abs(angle) < vision_angle_half_rad)
						{
							int obs_id = int((vision_angle_half_rad - angle) / vision_cell_angle_rad);
							if (distances->prey_predator_distances(prey_id, self_id) < model->x(self_id, obs_id))
								model->x(self_id, obs_id) = distances->prey_predator_distances(prey_id, self_id);

							if (distances->prey_predator_distances(prey_id, self_id) < target_closest)
							{
								target_closest = distances->prey_predator_distances(prey_id, self_id);
								target_id[self_id] = prey_id;
							}
						}
					}
				}
			}

			if (target_closest > eat_range_squared)
				target_id[self_id] = -1;

			model->x.row(self_id) = model->x.row(self_id).unaryExpr([this](float elem)
			{
				return elem == 1000000.f ? 0.0f : std::sqrt(elem) / vision_range;
			});
		}
	}
}

float PredatorSwarm::calculate_angle(int a, int b)
{
	float Ux = position(b,0) - position(a,0);
	float Uy = position(b,1) - position(a,1);

	if (Ux > Simulation::world_size_half)
		Ux -= Simulation::world_size;
	else if (Ux < -Simulation::world_size_half)
		Ux += Simulation::world_size;
	if (Uy > Simulation::world_size_half)
		Uy -= Simulation::world_size;
	else if (Uy < -Simulation::world_size_half)
		Uy += Simulation::world_size;

	return std::atan2((Ux * norm(a, 1)) - (Uy * norm(a, 0)), (Ux * norm(a, 0)) + (Uy * norm(a, 1)));
}

float PredatorSwarm::calculate_angle_preys(int a, int b)
{
	float Ux = prey_swarm->position(b, 0) - position(a, 0);
	float Uy = prey_swarm->position(b, 1) - position(a, 1);

	if (Ux > Simulation::world_size_half)
		Ux -= Simulation::world_size;
	else if (Ux < -Simulation::world_size_half)
		Ux += Simulation::world_size;
	if (Uy > Simulation::world_size_half)
		Uy -= Simulation::world_size;
	else if (Uy < -Simulation::world_size_half)
		Uy += Simulation::world_size;

	return std::atan2((Ux * norm(a, 1)) - (Uy * norm(a, 0)), (Ux * norm(a, 0)) + (Uy * norm(a, 1)));
}

void PredatorSwarm::update_decisions()
{
	this->model->call();
}

void PredatorSwarm::update_movement()
{
	for (int predator_id = 0; predator_id < population_size; predator_id++)
	{
		if (alive[predator_id])
		{
			angle(predator_id) += model->y(predator_id, 1) * turn_speed_rad;

			angle(predator_id) = angle(predator_id) < 0.0f ? angle(predator_id) + 6.18f : angle(predator_id) > 6.18f ? angle(predator_id) - 6.18f : angle(predator_id);

			float a = angle(predator_id);
			norm(predator_id, 0) = std::cos(a);
			norm(predator_id, 1) = std::sin(a);

			position.row(predator_id) += norm.row(predator_id) * model->y(predator_id, 0) * move_speed;

			position.row(predator_id) = position.row(predator_id).unaryExpr([](float elem)
			{
				return elem < -Simulation::world_size_half ? elem + Simulation::world_size : elem > Simulation::world_size_half ? elem - Simulation::world_size : elem;
			});
		}
	}
}

void PredatorSwarm::update_fitness()
{
	this->fitness += this->prey_swarm->population_size - this->prey_swarm->number_alive;
}

void PredatorSwarm::update_stats()
{
	int density = 0;
	float dispersion = 0;
	float temp2;

	for (int self_id = 0; self_id < 1; self_id++)
	{
		temp2 = 1000000.f;

		for (int predator_id = 0; predator_id < population_size; predator_id++)
		{
			if (self_id != predator_id)
			{
				if (this->distances->predator_distances(self_id, predator_id) < 900.f)
					density++;

				if (this->distances->predator_distances(self_id, predator_id) < temp2)
					temp2 = this->distances->predator_distances(self_id, predator_id);
			}
		}

		dispersion += std::sqrt(temp2);
	}

	if (number_alive > 0)
	{
		this->mean_density += (density / (float)number_alive);
		this->mean_dispersion += (dispersion / (float)number_alive);
		//this->mean_attacks += (number_attacks / (float)number_alive);
		//this->mean_hunts += (number_hunts / (float)number_alive);
	}

	//number_attacks = 0;
	//number_hunts = 0;
	//else
	//{
	//	this->mean_density += 0.f;
	//	this->mean_dispersion += 0.f;
	//}
}

void PredatorSwarm::try_hunt()
{
	int near_target_preys;

	for (int predator_id = 0; predator_id < population_size; predator_id++)
	{
		if (eat_delays[predator_id] > 0)
			eat_delays[predator_id] -= 1;
		else if (target_id[predator_id] != -1)
		{
			// Try to attack
			this->number_attacks++;

			// Confusion effect
			near_target_preys = 0;
			for (int prey_id = 0; prey_id < prey_swarm->population_size; prey_id++)
			{
				if (distances->prey_distances(target_id[predator_id], prey_id) < confusion_range_squared)
					near_target_preys++;
			}

			if ((float)std::rand() / RAND_MAX < confusion_ratio / near_target_preys)
			{
				prey_swarm->alive[target_id[predator_id]] = false; // Attack successful
				prey_swarm->number_alive--;
				this->number_hunts++;
			}

			eat_delays[predator_id] = eat_delay;
		}
	}
}
