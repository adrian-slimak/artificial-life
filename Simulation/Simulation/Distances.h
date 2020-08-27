#ifndef DISTANCES_H
#define DISTANCES_H

class PreySwarm;
class PredatorSwarm;

class Distances
{
public:
	static const float deg2rad;
	static const float rad2deg;
	static const float PI;
	static const float PI_2;

	PreySwarm *prey_swarm = nullptr;
	PredatorSwarm *predator_swarm = nullptr;

	float** prey_distances;
	float** predator_distances;
	float** predator_prey_distances;
	float** predator_prey_angles;

	float** prey_food_distances;
	float** prey_food_angles;

	Distances();
	Distances(PreySwarm &prey_swarm, PredatorSwarm & predator_swarm);
	~Distances();

	void recalculate_prey_observations();
	void recalculate_prey_food_observations();
	void recalculate_prey_predator_observations();
	void recalculate_predator_food_observations();
	void prey_observations();
	void predator_observations();
};
#endif
