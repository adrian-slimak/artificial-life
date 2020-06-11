#include "EnvManager.h"
#include <iostream>

#include "json11.hpp"

EnvManager::EnvManager()
{
	for (int i = 0; i < num_envs; i++)
		simulations.push_back(Simulation());
}

EnvManager::EnvManager(int num_envs)
{
	this->num_envs = num_envs;

	for (int i = 0; i < num_envs; i++)
		simulations.push_back(Simulation());
}

EnvManager::~EnvManager()
{
}

void EnvManager::set_prey_genes(float* prey_genes, int genes_count, int genes_length)
{
	this->prey_genes = new float*[genes_count];
	for (int i = 0; i < genes_count; i++)
        this->prey_genes[i] = prey_genes + i * genes_length;

	this->prey_fitness = new float[genes_count];
	this->genes_count = genes_count;
}

void EnvManager::set_predator_genes(float* predator_genes, int genes_count, int genes_length)
{
	this->predator_genes = new float*[genes_count];
	for (int i = 0; i < genes_count; i++)
		this->predator_genes[i] = predator_genes + i * genes_length;

	this->predator_fitness = new float[genes_count];
	this->genes_count = genes_count;
}

void EnvManager::set_fitness_pointers(float* prey_fintess, float* predator_fitness)
{
    this->prey_fitness = prey_fintess;
    this->predator_fitness = predator_fitness;
}

void EnvManager::set_stats_pointers(float* prey_stats, float* predator_stats)
{
    this->prey_stats = prey_stats;
    this->predator_stats = predator_stats;
}

void EnvManager::run_single_episode()
{
    // Needed to use random in C++
    srand(time(NULL));

	int cycles = genes_count / num_envs;

    // Evaluate all Genotypes
	for (int c = 0; c < cycles; c++)
	{
		int k = c * num_envs;

        // Build models from Genotypes to evaluate
		for (int n = 0; n < num_envs; n++)
			simulations[n].setModels(prey_genes[k+n], predator_genes[k+n]);

		// Start simulations
		for (int n = 0; n < num_envs; n++)
			simulations[n].runInThread();

		// Wait for all simulations to finish
		for (int n = 0; n < num_envs; n++)
			simulations[n].threadJoin();

		// Save fitness from envs
		for (int n = 0; n < num_envs; n++)
		{
			prey_fitness[k+n] = simulations[n].prey_swarm->fitness;
			predator_fitness[k+n] = simulations[n].predator_swarm->fitness;
		}

		// Save stats from envs
        for (int n = 0; n < num_envs; n++)
		{
			prey_fitness[k+n] = simulations[n].prey_swarm->fitness;
			predator_fitness[k+n] = simulations[n].predator_swarm->fitness;
		}

        // Save stats from envs
        for (int n = 0; n < num_envs; n++)
		{
			prey_stats[0] += simulations[n].prey_swarm->number_alive;
			prey_stats[1] += simulations[n].prey_swarm->mean_density;
			prey_stats[2] += simulations[n].prey_swarm->mean_dispersion;
			//prey_stats[3] += simulations[n].prey_swarm->mean_eats;

			predator_stats[0] += simulations[n].predator_swarm->number_alive;
			predator_stats[1] += simulations[n].predator_swarm->mean_density;
			predator_stats[2] += simulations[n].predator_swarm->mean_dispersion;
			predator_stats[3] += simulations[n].predator_swarm->mean_attacks;
			predator_stats[4] += simulations[n].predator_swarm->mean_hunts;
		}
	}
}

void EnvManager::set_parameters(const char *params_file_path)
{
	std::string err;
	const auto json = json11::Json::parse_file(params_file_path, err, json11::COMMENTS);

	//std::cout << "err: " << err << "\n";
	//std::cout << "k1: " << (float)json["prey"]["count"].number_value() << "\n";

	PreySwarm::population_size = (int)json["prey"]["count"].number_value();
	PreySwarm::brain_cells = 8;
	PreySwarm::observations_size = 39;
	PreySwarm::actions_size = (int)json["prey"]["actions_vector_size"].number_value();

	PreySwarm::vision_range = (float)json["prey"]["vision"]["range"].number_value();
	PreySwarm::vision_range_squared = PreySwarm::vision_range * PreySwarm::vision_range;
	PreySwarm::vision_angle = (float)json["prey"]["vision"]["angle"].number_value();
	PreySwarm::vision_angle_half = PreySwarm::vision_angle / 2.f;
	PreySwarm::vision_cells = (int)json["prey"]["vision"]["cells"].number_value();
	PreySwarm::vision_cell_angle = (float)(PreySwarm::vision_angle / PreySwarm::vision_cells);

	PreySwarm::eat_range = (float)json["prey"]["eat_range"].number_value();
	PreySwarm::eat_delay = (float)json["prey"]["eat_delay"].number_value();

	PreySwarm::move_speed = (float)json["prey"]["move_speed"].number_value();
	PreySwarm::turn_speed_rad = (float)json["prey"]["turn_speed"].number_value() * Distances::deg2rad;

	PredatorSwarm::population_size = (int)json["predator"]["count"].number_value();
	PredatorSwarm::brain_cells = 8;
	PredatorSwarm::observations_size = 39;
	PredatorSwarm::actions_size = (int)json["predator"]["actions_vector_size"].number_value();

	PredatorSwarm::vision_range = (float)json["predator"]["vision"]["range"].number_value();
	PredatorSwarm::vision_range_squared = PredatorSwarm::vision_range * PredatorSwarm::vision_range;
	PredatorSwarm::vision_angle = (float)json["predator"]["vision"]["angle"].number_value();
	PredatorSwarm::vision_angle_half = PredatorSwarm::vision_angle / 2.f;
	PredatorSwarm::vision_cells = (int)json["predator"]["vision"]["cells"].number_value();
	PredatorSwarm::vision_cell_angle = (float)(PredatorSwarm::vision_angle / PredatorSwarm::vision_cells);

	PredatorSwarm::eat_range = (float)json["predator"]["eat_range"].number_value();
	PredatorSwarm::eat_delay = (int)json["predator"]["eat_delay"].number_value();

	PredatorSwarm::move_speed = (float)json["predator"]["move_speed"].number_value();
	PredatorSwarm::turn_speed_rad = (float)json["predator"]["turn_speed"].number_value() * Distances::deg2rad;

	PredatorSwarm::confusion_range = (float)json["predator"]["confusion_effect"]["range"].number_value();
	PredatorSwarm::confusion_range_squared = PredatorSwarm::confusion_range*PredatorSwarm::confusion_range;
	PredatorSwarm::confusion_ratio = (float)json["predator"]["confusion_effect"]["ratio"].number_value();

	Simulation::world_size = (float)json["environment"]["world_size"].number_value();
	Simulation::world_size_half = Simulation::world_size / 2.f;
	Simulation::simulation_steps = (float)json["environment"]["simulation_steps"].number_value();
	Simulation::steps_without_predators = (int)json["environment"]["steps_without_predators"].number_value();
}
