#ifndef PREDATOR_SWARM_H
#define PREDATOR_SWARM_H

#include "INetwork.h"

class Distances;
class PreySwarm;

class PredatorSwarm
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

	static float attack_range;
	static float attack_range_squared;
	static int attack_delay;

	static float move_speed;
	static float turn_speed_rad;

	static float confusion_range;
	static float confusion_range_squared;
	static float confusion_ratio;

public:
	Eigen::ArrayXXf position;
	Eigen::ArrayXXf norm;
	Eigen::ArrayXf angle;

	INetwork *model;

	float fitness;
	float mean_density;
	float mean_dispersion;
	float mean_attacks;
	float mean_hunts;
	int number_attacks;
	int number_hunts;

	int* attack_delays;
	bool* sound_active;

	PreySwarm* prey_swarm;
	Distances* distances;

	PredatorSwarm();
	~PredatorSwarm();

	void update_decisions();
	void update_movement();
	void update_fitness();
	void update_stats();
	void try_hunt();
	void reset();
	void set_model(float* predator_genes);
	std::string to_string();
};
#endif