#ifndef DISTANCES_H
#define DISTANCES_H

#include <Eigen/Dense>

class PreySwarm;
class PredatorSwarm;

class Distances
{
public:
	static const float deg2rad;
	static const float rad2deg;

	PreySwarm *prey_swarm = nullptr;
	PredatorSwarm *predator_swarm = nullptr;

	Eigen::MatrixXf prey_distances;
	Eigen::MatrixXf predator_distances;
	Eigen::MatrixXf prey_predator_distances;

	Distances();
	Distances(PreySwarm &prey_swarm, PredatorSwarm & predator_swarm);
	~Distances();

	void recalculate_prey_distances();
	void recalculate_predator_distances();
	void reset();

	static float calculate_angle(float xa, float ya, float a, float xb, float yb);

private:
	static void calculate_distances(Eigen::MatrixXf & A, Eigen::MatrixXf & B, Eigen::MatrixXf & C);
};

#endif
