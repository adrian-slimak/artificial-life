#ifndef PREY_SWARM_H
#define PREY_SWARM_H

#include "INetwork.h"

class Distances;
class PredatorSwarm;

class PreySwarm
{
public:
	static int population_size;
	static int brain_cells;
	static NetworkType network_type;
	static int observations_size;
	static int actions_size;

	static int vision_size;
	static float vision_range;
	static float vision_range_squared;
	static float vision_angle;
	static float vision_angle_half_rad;
	static int vision_cells;
	static float vision_cell_angle_rad;

	static bool communication_enabled;
	static float hear_range;
	static float hear_range_squared;
	static int hear_cells;
	static float hear_cell_angle_rad;

	static bool food_enabled;
	static int food_amount;
	static int food_spawn_method;
	static float eat_range;
	static float eat_range_squared;
	static int eat_delay;
	static float energy_start;
	static float energy_gain_per_eat;
	static float energy_drain_per_step;

	static float move_speed;
	static float turn_speed_rad;

public:
	int number_alive;
	bool* alive;

	Eigen::ArrayXXf position;
	Eigen::ArrayXXf norm;
	Eigen::ArrayXf angle;

	Eigen::ArrayXXf food_position;
	bool* food_alive;
	Eigen::ArrayXf energy;
	Eigen::ArrayXi eat_delays;

	bool* food_sound_active;
	bool* predator_sound_active;

	float fitness;
	float mean_density;
	float mean_dispersion;
	float mean_eats;

	INetwork *model;

	PredatorSwarm* predator_swarm;
	Distances* distances;

	PreySwarm();
	~PreySwarm();

	void update_decisions();
	void update_movement();
	void update_fitness();
	void update_stats();
	void update_food();
	void try_eat();
	void reset();
	void set_model(float* prey_genes);
	std::string to_string();
	std::string food_to_string();
};
#endif
