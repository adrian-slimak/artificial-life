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

float PredatorSwarm::attack_range = 5.f;
float PredatorSwarm::attack_range_squared = 5.f*5.f;
int PredatorSwarm::attack_delay = 10;

float PredatorSwarm::move_speed = 3.f;
float PredatorSwarm::turn_speed_rad = 6.f  * Distances::deg2rad;

float PredatorSwarm::confusion_range = 30.f;
float PredatorSwarm::confusion_range_squared = 30.f*30.f;
float PredatorSwarm::confusion_ratio = 1.f;

PredatorSwarm::PredatorSwarm()
{
	alive = new bool[PredatorSwarm::population_size];

	position = Eigen::MatrixXf(population_size, 2);

	//float* temp2 = new float[PredatorSwarm::population_size * 2];
	//position_2 = new float*[PredatorSwarm::population_size];
	//for (int i = 0; i < PredatorSwarm::population_size; i++)
	//	position_2[i] = temp2+(i*2);

	norm = new float*[PredatorSwarm::population_size];
	for (int i = 0; i < PredatorSwarm::population_size; ++i)
		norm[i] = new float[2];
	angle = new float[PredatorSwarm::population_size];

	attack_delays = new int[PredatorSwarm::population_size];

	if (network_type == NetworkType::_LSTM)
		this->model = new LSTM(observations_size, brain_cells, actions_size, population_size);
	else
		this->model = new RNN(observations_size, brain_cells, actions_size, population_size);

	this->model->build();
}


PredatorSwarm::~PredatorSwarm()
{
	for (int i = 0; i < PredatorSwarm::population_size; ++i)
		delete[] norm[i];
	delete[] norm;

	delete[] angle;

	delete[] attack_delays;
}

void PredatorSwarm::reset()
{
	number_alive = population_size;
	for (int i = 0; i < population_size; i++)
		alive[i] = true;

	position.setRandom();
	position *= Simulation::world_size_half;

	//for (int i = 0; i < PredatorSwarm::population_size; i++)
	//{
	//	position_2[i][0] = ((float)std::rand() / (RAND_MAX)) * Simulation::world_size - Simulation::world_size_half;
	//	position_2[i][1] = ((float)std::rand() / (RAND_MAX)) * Simulation::world_size - Simulation::world_size_half;
	//}

	//for (int i = 0; i < PredatorSwarm::population_size; i++)
	//{
	//	position_2[i][0] = position(i,0);
	//	position_2[i][1] = position(i,0);
	//}
	for (int i = 0; i < PredatorSwarm::population_size; i++)
		angle[i] = ((float)std::rand() / (RAND_MAX)) * 6.18f;

	fitness = 0.f;
	mean_density = 0.f;
	mean_dispersion = 0.f;
	number_attacks = 0;
	number_hunts = 0;

	for (int i = 0; i < PredatorSwarm::population_size; i++)
		attack_delays[i] = 0;

	this->model->reset();
}

void PredatorSwarm::set_model(float* predator_genes)
{
	this->model->build(predator_genes);
}

void PredatorSwarm::update_decisions()
{
	this->model->call();
}

void PredatorSwarm::update_fitness()
{
	this->fitness += this->prey_swarm->population_size - this->prey_swarm->number_alive;
}

void PredatorSwarm::update_stats()
{
	int density = 0;
	float dispersion = 0;
	float min_dist;

	for (int self_id = 0; self_id < 1; self_id++)
	{
		min_dist = 1000000.f;

		for (int predator_id = 0; predator_id < population_size; predator_id++)
		{
			if (self_id != predator_id)
			{
				if (this->distances->predator_distances[self_id][predator_id] < 900.f)
					density++;

				if (this->distances->predator_distances[self_id][predator_id] < min_dist)
					min_dist = this->distances->predator_distances[self_id][predator_id];
			}
		}

		dispersion += std::sqrt(min_dist);
	}

	if (number_alive > 0)
	{
		this->mean_density += (density / (float)number_alive);
		this->mean_dispersion += (dispersion / (float)number_alive);
	}
}


void PredatorSwarm::update_movement()
{
	float a;
	float speed;

	for (int p = 0; p < population_size; p++)
	{
		if (alive[p])
		{
			angle[p] += model->y(p, 1) * turn_speed_rad;

			angle[p] = angle[p] < 0.0f ? angle[p] + 6.18f : angle[p] > 6.18f ? angle[p] - 6.18f : angle[p];

			a = angle[p];
			norm[p][0] = std::cos(a);
			norm[p][1] = std::sin(a);

			speed = model->y(p, 0) * move_speed;

			position(p, 0) += norm[p][0] * speed;
			position(p, 1) += norm[p][1] * speed;

			//position_2[p][0] += norm[p][0] * speed;
			//position_2[p][1] += norm[p][1] * speed;

			//if (position_2[p][0] > Simulation::world_size_half)
			//	position_2[p][0] -= Simulation::world_size;
			//else if (position_2[p][0] < -Simulation::world_size_half)
			//	position_2[p][0] += Simulation::world_size;

			//if (position_2[p][1] > Simulation::world_size_half)
			//	position_2[p][1] -= Simulation::world_size;
			//else if (position_2[p][1] < -Simulation::world_size_half)
			//	position_2[p][1] += Simulation::world_size;

			//position_2[p][0] = position_2[p][0] < -Simulation::world_size_half ? position_2[p][0] + Simulation::world_size : position_2[p][0] > Simulation::world_size_half ? position_2[p][0] - Simulation::world_size : position_2[p][0];
			//position_2[p][1] = position_2[p][1] < -Simulation::world_size_half ? position_2[p][1] + Simulation::world_size : position_2[p][1] > Simulation::world_size_half ? position_2[p][1] - Simulation::world_size : position_2[p][1];
		}
	}

	position = position.unaryExpr([](float elem)
	{
		return elem < -Simulation::world_size_half ? elem + Simulation::world_size : elem > Simulation::world_size_half ? elem - Simulation::world_size : elem;
	});
}

void PredatorSwarm::try_hunt()
{
	int target_id;
	float min_dist;
	int near_target_preys;

	for (int predator_id = 0; predator_id < population_size; predator_id++)
	{
		// If attack delay is greater than 0, predator cant attack yet
		if (attack_delays[predator_id] > 0)
			attack_delays[predator_id] -= 1;
		else
		{
			target_id = -1;
			min_dist = 1000000.f;
			near_target_preys;
			// Predator can attack now, find closest target
			for (int prey_id = 0; prey_id < PreySwarm::population_size; prey_id++)
			{
				if (prey_swarm->alive[prey_id] &&
					distances->predator_prey_distances[predator_id][prey_id] < PredatorSwarm::vision_range_squared &&
					std::abs(distances->predator_prey_angles[predator_id][prey_id]) < PredatorSwarm::vision_angle_half_rad &&
					distances->predator_prey_distances[predator_id][prey_id] < min_dist)
				{
					min_dist = distances->predator_prey_distances[predator_id][prey_id];
					target_id = prey_id;
				}
			}

			if (min_dist > PredatorSwarm::attack_range_squared)
				target_id = -1;

			if (target_id > -1)
			{
				// Try to attack
				this->number_attacks++;

				near_target_preys = 0;
				// Find number of preys near target prey and in predator vision field
				for (int prey_id = 0; prey_id < PreySwarm::population_size; prey_id++)
				{
					if (prey_swarm->alive[prey_id] &&
						distances->prey_distances[target_id][prey_id] < PredatorSwarm::confusion_range_squared &&
						distances->predator_prey_distances[predator_id][prey_id] < PredatorSwarm::vision_range_squared &&
						std::abs(distances->predator_prey_angles[predator_id][prey_id]) < PredatorSwarm::vision_angle_half_rad)

						near_target_preys++;
				}

				// Confusion effect
				if ((float)std::rand() / RAND_MAX < confusion_ratio / near_target_preys)
				{
					// Attack successful
					prey_swarm->alive[target_id] = false;
					prey_swarm->number_alive--;
					this->number_hunts++;
				}

				attack_delays[predator_id] = attack_delay;
			}
		}
	}
}
