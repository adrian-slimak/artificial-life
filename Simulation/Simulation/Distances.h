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
	float** prey_angles;
	float** predator_distances;
	float** predator_prey_distances;
	float** predator_prey_angles;

	Distances();
	Distances(PreySwarm &prey_swarm, PredatorSwarm & predator_swarm);
	~Distances();

	void reset();

	void recalculate_prey_distances_observations();
	void recalculate_prey_predator_distances_observations();
};

#endif
