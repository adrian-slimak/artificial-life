#include "Simulation.h"
#include <vector>

#pragma once
class EnvManager
{
private:
	std::vector<Simulation> simulations;
	int num_envs = 10;

	float** prey_genes;
	float** predator_genes;
	int genes_count;

	float* prey_fitness;
	float* predator_fitness;

    float* prey_stats;
	float* predator_stats;

public:
	EnvManager();
	EnvManager(int num_envs);
	~EnvManager();

	static void set_parameters(const char *params_file_path);

	void set_prey_genes(float* prey_genes, int genes_count, int genes_length);
	void set_predator_genes(float* predator_genes, int genes_count, int genes_length);
	void set_fitness_pointers(float* prey_fitness, float* predator_fitness);
	void set_stats_pointers(float* prey_stats, float* predator_stats);

	void run_single_episode();
};
