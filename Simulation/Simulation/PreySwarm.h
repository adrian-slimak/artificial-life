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

	static float vision_range;
	static float vision_range_squared;
	static float vision_angle;
	static float vision_angle_half_rad;
	static int vision_cells;
	static float vision_cell_angle_rad;

	static bool hear_enabled;
	static float hear_range;
	static float hear_range_squared;
	static int hear_cells;
	static float hear_cell_angle_rad;

	static bool communication_enabled;

	static float eat_range;
	static int eat_delay;

	static float move_speed;
	static float turn_speed_rad;

public:
	int number_alive;
	bool *alive;

	Eigen::ArrayXXf position;
	Eigen::ArrayXXf norm;
	Eigen::ArrayXf angle;

	float fitness;
	float mean_density;
	float mean_dispersion;

	int* eat_delays;

	INetwork *model;

	PredatorSwarm* predator_swarm;
	Distances* distances;

	PreySwarm();
	~PreySwarm();

	void update_decisions();
	void update_movement();
	void update_fitness();
	void update_stats();
	void reset();
	void set_model(float* prey_genes);
};
#endif
