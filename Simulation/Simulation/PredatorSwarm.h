#ifndef PREDATOR_SWARM_H
#define PREDATOR_SWARM_H

#include <Eigen/Dense>

class Distances;
class PreySwarm;
class LSTM;

class PredatorSwarm
{
public:
	static int population_size;
	static int brain_cells;
	static int observations_size;
	static int actions_size;

	static float vision_range;
	static float vision_range_squared;
	static float vision_angle;
	static float vision_angle_half;
	static int vision_cells;
	static float vision_cell_angle;

	static float hear_range;
	static float hear_range_squared;
	static int hear_cells;
	static float hear_cell_angle;

	static float eat_range;
	static int eat_delay;

	static float move_speed;
	static float turn_speed_rad;

	static float confusion_range;
	static float confusion_range_squared;
	static float confusion_ratio;

public:
	int number_alive;
	bool *alive;

	Eigen::MatrixXf position;
	Eigen::MatrixXf norm;
	Eigen::VectorXf angle;

	LSTM *model;

	float fitness;
	float mean_density;
	float mean_dispersion;
	float mean_attacks;
	float mean_hunts;
	int number_attacks;
	int number_hunts;

	Eigen::VectorXi target_id;
	Eigen::VectorXi eat_delays;

	PreySwarm* prey_swarm;
	Distances* distances;

	PredatorSwarm();
	~PredatorSwarm();

	void update_observations();
	void update_decisions();
	void update_movement();
	void update_fitness();
	void update_stats();
	void try_hunt();
	void reset();
	void set_model(float* predator_genes);

private:
	float calculate_angle(int a, int b);
	float calculate_angle_preys(int a, int b);
};
#endif