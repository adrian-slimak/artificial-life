#ifndef SIMULATION_H
#define SIMULATION_H


#include "PreySwarm.h"
#include "PredatorSwarm.h"
#include "Distances.h"
//#include "Visualization.h"

#include <thread>

#pragma once
class Simulation
{
public:
	static float world_size;
	static float world_size_half;

	static int simulation_steps;

	static int steps_without_predators;

private:
	std::thread* _thread;

public:
	int step;
	float* stats;
	PreySwarm* prey_swarm = nullptr;
	PredatorSwarm* predator_swarm = nullptr;

	Distances* distances = nullptr;

	//Visualization* visualization = nullptr;

	Simulation();
	Simulation(bool show_visualization);
	~Simulation();

	void reset();
	void setModels(float* prey_genes, float* predator_genes);
	void runSingleEpisode();

	void runInThread();
	void threadJoin();

	void create_visualization(const char *vis_file_path);
};
#endif
