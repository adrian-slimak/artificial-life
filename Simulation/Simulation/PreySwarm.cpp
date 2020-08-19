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
int PreySwarm::observations_size = 26;
int PreySwarm::actions_size = 2;

int PreySwarm::vision_size = 26;
float PreySwarm::vision_range = 100.f;
float PreySwarm::vision_range_squared = PreySwarm::vision_range * PreySwarm::vision_range;
float PreySwarm::vision_angle = 182.f;
float PreySwarm::vision_angle_half_rad = (PreySwarm::vision_angle / 2.f) * Distances::deg2rad;
int PreySwarm::vision_cells = 13;
float PreySwarm::vision_cell_angle_rad = (float)(PreySwarm::vision_angle / PreySwarm::vision_cells) * Distances::deg2rad;

bool PreySwarm::communication_enabled = false;
int PreySwarm::food_sound_trigger = 5;
float PreySwarm::hear_range = 100.f;
float PreySwarm::hear_range_squared = PreySwarm::hear_range * PreySwarm::hear_range;
int PreySwarm::hear_cells = 12;
float PreySwarm::hear_cell_angle_rad = (float)(360.f / PreySwarm::hear_cells) * Distances::deg2rad;

float PreySwarm::eat_range = 5.f;
float PreySwarm::eat_range_squared = PreySwarm::eat_range * PreySwarm::eat_range;
int PreySwarm::eat_delay = 10;
float PreySwarm::energy_start = 100.f;
float PreySwarm::energy_gain_per_eat = 25.f;
float PreySwarm::energy_drain_per_step = 0.1f;

float PreySwarm::move_speed = 1.f;
float PreySwarm::turn_speed_rad = 8.f * Distances::deg2rad;

PreySwarm::PreySwarm()
{
	alive = new bool[PreySwarm::population_size];

	position = Eigen::ArrayXXf(PreySwarm::population_size, 2);

	norm = Eigen::ArrayXXf(PreySwarm::population_size, 2);

	angle = Eigen::ArrayXf(PreySwarm::population_size);

	food_sound_active = new bool[PreySwarm::population_size];
	predator_sound_active = new bool[PreySwarm::population_size];

	//eat_delays = Eigen::ArrayXi(PreySwarm::population_size);
	//plants_alive = new bool[PreySwarm::population_size];

	if (network_type == NetworkType::_LSTM)
		this->model = new LSTM(PreySwarm::observations_size, PreySwarm::brain_cells, PreySwarm::actions_size, PreySwarm::population_size);
	else
		this->model = new RNN(PreySwarm::observations_size, PreySwarm::brain_cells, PreySwarm::actions_size, PreySwarm::population_size);

	this->model->build();
}

PreySwarm::~PreySwarm()
{

	delete[] alive;
	//delete[] plants_alive;
}

void PreySwarm::reset()
{
	number_alive = population_size;
	for (int i = 0; i < population_size; i++)
	{
		alive[i] = true;
		food_sound_active[i] = false;
		predator_sound_active[i] = false;
	}

	//for (int i = 0; i < population_size; i++)
	//	plants_alive[i] = true;

	position.setRandom();
	position *= Simulation::world_size_half;

	angle.setRandom();
	angle += 1.f;
	angle *= 3.14f;

	fitness = 0.f;
	mean_density = 0.f;
	mean_dispersion = 0.f;
	number_eats = 0;

	//energy.setConstant(energy_start);
	//eat_delays.setZero();

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

	// Tutaj te� da si� przyspieszy�, poniewa� a->b oraz b->a to ten sam dystans...
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
		int temp = 0;
		for (int self_id = 0; self_id < PreySwarm::population_size; self_id++)
		{
			if (this->alive[self_id])
			{
				//this->food_sound_active[self_id] = (this->model->x.block(self_id,PreySwarm::vision_cells,1,PreySwarm::vision_cells).array() > 0.0f).sum()>=PreySwarm::food_sound_trigger;
				this->predator_sound_active[self_id] = (this->model->x.block(self_id,PreySwarm::vision_cells,1,PreySwarm::vision_cells).array() > 0.0f).any();
			}
		}
	}
}

//void PreySwarm::try_eat()
//{
//	int target_id;
//	float min_dist;
//
//	for (int prey_id = 0; prey_id < population_size; prey_id++)
//	{
//		// If attack delay is greater than 0, prey cant eat yet
//		if (eat_delays[prey_id] > 0)
//			eat_delays[prey_id] -= 1;
//		else
//		{
//			target_id = -1;
//			min_dist = 1000000.f;
//			// Prey can eat now, find closest plant
//			for (int plant_id = 0; plant_id < PreySwarm::population_size; plant_id++)
//			{
//				if (this->plants_alive[prey_id] &&
//					std::abs(distances->prey_plant_angles[prey_id][plant_id]) < PreySwarm::vision_angle_half_rad &&
//					distances->prey_plant_distances[prey_id][plant_id] < min_dist)
//				{
//					min_dist = distances->prey_plant_distances[prey_id][plant_id];
//					target_id = prey_id;
//				}
//			}
//
//			if (min_dist > PreySwarm::eat_range)
//				target_id = -1;
//
//			if (target_id > -1)
//			{
//				this->number_eats++;
//				this->energy(prey_id) += energy_gain_per_eat;
//				eat_delays[prey_id] = eat_delay;
//				this->plants_alive[target_id] = false;
//			}
//		}
//	}
//}

