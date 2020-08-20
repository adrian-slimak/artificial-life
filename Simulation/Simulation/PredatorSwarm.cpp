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
int PredatorSwarm::observations_size = 26;
int PredatorSwarm::actions_size = 2;

int PredatorSwarm::vision_size = 26;
float PredatorSwarm::vision_range = 200.f;
float PredatorSwarm::vision_range_squared = PredatorSwarm::vision_range * PredatorSwarm::vision_range;
float PredatorSwarm::vision_angle = 182.f;
float PredatorSwarm::vision_angle_half_rad = (PredatorSwarm::vision_angle/2.f) * Distances::deg2rad;
int PredatorSwarm::vision_cells = 13;
float PredatorSwarm::vision_cell_angle_rad = (float)(PredatorSwarm::vision_angle / PredatorSwarm::vision_cells) * Distances::deg2rad;

bool PredatorSwarm::communication_enabled = false;
float PredatorSwarm::hear_range = 200.f;
float PredatorSwarm::hear_range_squared = PredatorSwarm::hear_range * PredatorSwarm::hear_range;
int PredatorSwarm::hear_cells = 12;
float PredatorSwarm::hear_cell_angle_rad = (float)(360.f / PredatorSwarm::hear_cells) * Distances::deg2rad;

float PredatorSwarm::attack_range = 5.f;
float PredatorSwarm::attack_range_squared = PredatorSwarm::attack_range * PredatorSwarm::attack_range;
int PredatorSwarm::attack_delay = 10;

float PredatorSwarm::move_speed = 3.f;
float PredatorSwarm::turn_speed_rad = 6.f  * Distances::deg2rad;

float PredatorSwarm::confusion_range = 30.f;
float PredatorSwarm::confusion_range_squared = PredatorSwarm::confusion_range * PredatorSwarm::confusion_range;
float PredatorSwarm::confusion_ratio = 1.f;

PredatorSwarm::PredatorSwarm()
{
	position = Eigen::ArrayXXf(PredatorSwarm::population_size, 2);

	norm = Eigen::ArrayXXf(PredatorSwarm::population_size, 2);

	angle = Eigen::ArrayXf(PredatorSwarm::population_size);

	attack_delays = new int[PredatorSwarm::population_size];

	sound_active = new bool[PredatorSwarm::population_size];

	if (network_type == NetworkType::_LSTM)
		this->model = new LSTM(PredatorSwarm::observations_size, PredatorSwarm::brain_cells, PredatorSwarm::actions_size, PredatorSwarm::population_size);
	else
		this->model = new RNN(PredatorSwarm::observations_size, PredatorSwarm::brain_cells, PredatorSwarm::actions_size, PredatorSwarm::population_size);

	this->model->build();
}


PredatorSwarm::~PredatorSwarm()
{

	delete[] attack_delays;
}

void PredatorSwarm::reset()
{
	position.setRandom();
	position *= Simulation::world_size_half;

	angle.setRandom();
	angle += 1.f;
	angle *= 3.14f;

	fitness = 0.f;
	mean_density = 0.f;
	mean_dispersion = 0.f;
	number_attacks = 0;
	number_hunts = 0;

	for (int i = 0; i < PredatorSwarm::population_size; i++)
	{
		attack_delays[i] = 0;
		sound_active[i] = false;
	}

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
	float dispersion = 0.f;
	float min_dist;

	for (int self_id = 0; self_id < population_size; self_id++)
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

	this->mean_density += (density / (float)PredatorSwarm::population_size);
	this->mean_dispersion += (dispersion / (float)PredatorSwarm::population_size);
}


void PredatorSwarm::update_movement()
{
	angle += model->y.col(1).array() * turn_speed_rad;

	angle = angle.unaryExpr([](float elem)
	{
		return elem < 0.0f ? elem + Distances::PI_2 : elem > Distances::PI_2 ? elem - Distances::PI_2 : elem;
	});

	norm.col(0) = angle.cos();
	norm.col(1) = angle.sin();

	position += norm.colwise() * model->y.col(0).array() * move_speed;

	position = position.unaryExpr([](float elem)
	{
		return elem < -Simulation::world_size_half ? elem + Simulation::world_size : elem > Simulation::world_size_half ? elem - Simulation::world_size : elem;
	});

	// COMMUNICATION
	if (PredatorSwarm::communication_enabled)
	{
		for (int predator_id = 0; predator_id < PredatorSwarm::population_size; predator_id++)
		{
			this->sound_active[predator_id] = this->model->y(predator_id, 2) > 0?true:false;
		}
	}
}

void PredatorSwarm::try_hunt()
{
	int target_id;
	float min_dist;
	int near_target_preys;

	for (int predator_id = 0; predator_id < PredatorSwarm::population_size; predator_id++)
	{
		// If attack delay is greater than 0, predator cant attack yet
		if (attack_delays[predator_id] > 0)
			attack_delays[predator_id] -= 1;
		else
		{
			target_id = -1;
			//min_dist = 1000000.f;
			min_dist = PredatorSwarm::attack_range_squared;

			// Predator can attack now, find closest target
			for (int prey_id = 0; prey_id < PreySwarm::population_size; prey_id++)
			{
				if (prey_swarm->alive[prey_id] &&
					//distances->predator_prey_distances[predator_id][prey_id] < PredatorSwarm::attack_range_squared &&
					distances->predator_prey_distances[predator_id][prey_id] < min_dist &&
					std::abs(distances->predator_prey_angles[predator_id][prey_id]) < PredatorSwarm::vision_angle_half_rad)
				{
					min_dist = distances->predator_prey_distances[predator_id][prey_id];
					target_id = prey_id;
				}
			}

			if (target_id > -1)
			{
				// Try to attack
				this->number_attacks++;
				attack_delays[predator_id] = attack_delay;

				near_target_preys = 0;
				// Find number of preys near target prey and in predator vision field
				for (int prey_id = 0; prey_id < PreySwarm::population_size; prey_id++)
				{
					if (prey_swarm->alive[prey_id] &&
						distances->prey_distances[target_id][prey_id] < PredatorSwarm::confusion_range_squared &&
						//distances->predator_prey_distances[predator_id][prey_id] < PredatorSwarm::vision_range_squared &&
						std::abs(distances->predator_prey_angles[predator_id][prey_id]) < PredatorSwarm::vision_angle_half_rad)

						near_target_preys++;
				}
				
				// Confusion effect
				if (((float)std::rand() / RAND_MAX) < (1.f / near_target_preys))
				{
					// Attack successful
					prey_swarm->alive[target_id] = false;
					prey_swarm->number_alive--;
					this->number_hunts++;
				}
			}
		}
	}
}

std::string PredatorSwarm::to_string()
{
	std::string string = "";

	for (int i = 0; i < PredatorSwarm::population_size; i++)
	{
		string += "1";
		string += ";";
		string += std::to_string(position(i, 0)) + ";" + std::to_string(position(i, 1));
		string += ";";
		string += std::to_string(angle[i]);
		string += "|";
	}

	return string;
}
