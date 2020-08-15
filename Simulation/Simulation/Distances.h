#ifndef DISTANCES_H
#define DISTANCES_H

class PreySwarm;
class PredatorSwarm;

class Distances
{
public:
	static const float deg2rad;
	static const float rad2deg;

	PreySwarm *prey_swarm = nullptr;
	PredatorSwarm *predator_swarm = nullptr;

	float** prey_distances;
	float** predator_distances;
	float** predator_prey_distances;
	float** predator_prey_angles;

	float** prey_plant_distances;
	float** prey_plant_angles;

	Distances();
	Distances(PreySwarm &prey_swarm, PredatorSwarm & predator_swarm);
	~Distances();

	void recalculate_prey_observations();
	void recalculate_prey_predator_observations();
	void prey_observations();
	void predator_observations();
};
#endif
