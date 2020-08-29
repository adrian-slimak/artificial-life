#include "EnvManager.h"
#include <iostream>

#include "json11.hpp"
#include "INetwork.h"

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

	//this->prey_fitness = new float[genes_count];
	this->genes_count = genes_count;
}

void EnvManager::set_predator_genes(float* predator_genes, int genes_count, int genes_length)
{
	this->predator_genes = new float*[genes_count];
	for (int i = 0; i < genes_count; i++)
		this->predator_genes[i] = predator_genes + i * genes_length;

	//this->predator_fitness = new float[genes_count];
	this->genes_count = genes_count;
}

void EnvManager::set_fitness_pointers(float* prey_fitess, float* predator_fitness)
{
    this->prey_fitness = prey_fitess;
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

	// Reset stats
	for (int i = 0; i < 5; i++)
	{
		prey_stats[i] = 0.f;
		predator_stats[i] = 0.f;
	}

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
			this->prey_fitness[k+n] = simulations[n].prey_swarm->fitness;
			this->predator_fitness[k+n] = simulations[n].predator_swarm->fitness;
		}

        // Save stats from envs
        for (int n = 0; n < num_envs; n++)
		{
			prey_stats[0] += simulations[n].prey_swarm->number_alive;
			prey_stats[1] += simulations[n].prey_swarm->mean_density;
			prey_stats[2] += simulations[n].prey_swarm->mean_dispersion;
			prey_stats[3] += simulations[n].prey_swarm->mean_eats;

			predator_stats[1] += simulations[n].predator_swarm->mean_density;
			predator_stats[2] += simulations[n].predator_swarm->mean_dispersion;
			predator_stats[3] += simulations[n].predator_swarm->mean_attacks;
			predator_stats[4] += simulations[n].predator_swarm->mean_hunts;
		}
	}

	for (int i = 0; i < 5; i++)
	{
		prey_stats[i] /= this->genes_count;
		predator_stats[i] /= this->genes_count;
	}
}

void EnvManager::create_visualization(const char *vis_file_path)
{
	// Needed to use random in C++
	srand(time(NULL));

	Simulation simulation = Simulation();

	// Build models from Genotypes to evaluate
	simulation.setModels(prey_genes[0], predator_genes[0]);

	// Start simulations
	simulation.create_visualization(vis_file_path);
}

void EnvManager::set_parameters(const char *params_file_path)
{
	std::string err;
	const auto json = json11::Json::parse_file(params_file_path, err, json11::COMMENTS);

	//std::cout << "k1: " << (float)json["prey"]["count"].number_value() << "\n";

	PreySwarm::population_size = (int)json["prey"]["count"].number_value();
	PredatorSwarm::population_size = (int)json["predator"]["count"].number_value();

	PreySwarm::brain_cells = (int)json["prey"]["brain_cells"].number_value();
	PredatorSwarm::brain_cells = (int)json["predator"]["brain_cells"].number_value();
	PreySwarm::network_type = json["prey"]["brain_type"].string_value() == "LSTM"? NetworkType::_LSTM : NetworkType::_RNN;
	PredatorSwarm::network_type = json["predator"]["brain_type"].string_value() == "LSTM" ? NetworkType::_LSTM : NetworkType::_RNN;

	PreySwarm::move_speed = (float)json["prey"]["move_speed"].number_value();
	PreySwarm::turn_speed_rad = (float)json["prey"]["turn_speed"].number_value() * Distances::deg2rad;
	PredatorSwarm::move_speed = (float)json["predator"]["move_speed"].number_value();
	PredatorSwarm::turn_speed_rad = (float)json["predator"]["turn_speed"].number_value() * Distances::deg2rad;

	PreySwarm::vision_range = (float)json["prey"]["vision"]["range"].number_value();
	PreySwarm::vision_range_squared = PreySwarm::vision_range * PreySwarm::vision_range;
	PreySwarm::vision_angle = (float)json["prey"]["vision"]["angle"].number_value();
	PreySwarm::vision_angle_half_rad = (PreySwarm::vision_angle / 2.f) * Distances::deg2rad;
	PreySwarm::vision_cells = (int)json["prey"]["vision"]["cells"].number_value();
	PreySwarm::vision_cell_angle_rad = (float)(PreySwarm::vision_angle / PreySwarm::vision_cells) * Distances::deg2rad;

	PredatorSwarm::vision_range = (float)json["predator"]["vision"]["range"].number_value();
	PredatorSwarm::vision_range_squared = PredatorSwarm::vision_range * PredatorSwarm::vision_range;
	PredatorSwarm::vision_angle = (float)json["predator"]["vision"]["angle"].number_value();
	PredatorSwarm::vision_angle_half_rad = (PredatorSwarm::vision_angle / 2.f) * Distances::deg2rad;
	PredatorSwarm::vision_cells = (int)json["predator"]["vision"]["cells"].number_value();
	PredatorSwarm::vision_cell_angle_rad = (float)(PredatorSwarm::vision_angle / PredatorSwarm::vision_cells) * Distances::deg2rad;

	PreySwarm::communication_enabled = json["prey"]["communication"]["enabled"].bool_value();
	PredatorSwarm::communication_enabled = json["predator"]["communication"]["enabled"].bool_value();

	PreySwarm::hear_range = (float)json["prey"]["communication"]["hear_range"].number_value();
	PreySwarm::hear_range_squared = PreySwarm::hear_range * PreySwarm::hear_range;
	PreySwarm::hear_cells = (int)json["prey"]["communication"]["hear_cells"].number_value();
	PreySwarm::hear_cell_angle_rad = (float)(360.f / PreySwarm::hear_cells) * Distances::deg2rad;

	PredatorSwarm::hear_range = (float)json["predator"]["communication"]["hear_range"].number_value();
	PredatorSwarm::hear_range_squared = PredatorSwarm::hear_range * PredatorSwarm::hear_range;
	PredatorSwarm::hear_cells = (int)json["predator"]["communication"]["hear_cells"].number_value();
	PredatorSwarm::hear_cell_angle_rad = (float)(360.f / PredatorSwarm::hear_cells) * Distances::deg2rad;

	PreySwarm::food_enabled = json["environment"]["food"]["enabled"].bool_value();
	PreySwarm::food_amount = (int)json["environment"]["food"]["amount"].number_value();
	PreySwarm::food_spawn_method = (int)json["environment"]["food"]["spawn_method"].number_value();
	//PreySwarm::vision_size = PreySwarm::vision_cells * (PreySwarm::food_enabled ? 3 : 2);
	PreySwarm::vision_size = PreySwarm::vision_cells * 3; // HEREEEEEEEEEEEEEEEEEEEEEEEEEEE
	//PredatorSwarm::vision_size = PredatorSwarm::vision_cells * (PreySwarm::food_enabled ? 3 : 2);
	PredatorSwarm::vision_size = PredatorSwarm::vision_cells * 2;

	PreySwarm::observations_size = PreySwarm::vision_size;
	if (PreySwarm::communication_enabled)
	{
		PreySwarm::observations_size += PreySwarm::hear_cells * 2;
		//if (PreySwarm::food_enabled)
		//	PreySwarm::observations_size += PreySwarm::hear_cells;
	}
	PredatorSwarm::observations_size = PredatorSwarm::vision_size;
	if (PredatorSwarm::communication_enabled)
		PredatorSwarm::observations_size += PredatorSwarm::hear_cells;
		

	PreySwarm::actions_size = (int)json["prey"]["actions_size"].number_value();
	PredatorSwarm::actions_size = (int)json["predator"]["actions_size"].number_value();
	if (PredatorSwarm::communication_enabled)
		PredatorSwarm::actions_size += 1;

	PreySwarm::eat_range = (float)json["prey"]["eat_range"].number_value();
	PreySwarm::eat_range_squared = PreySwarm::eat_range * PreySwarm::eat_range;
	PreySwarm::eat_delay = (int)json["prey"]["eat_delay"].number_value();
	PredatorSwarm::attack_range = (float)json["predator"]["eat_range"].number_value();
	PredatorSwarm::attack_range_squared = PredatorSwarm::attack_range * PredatorSwarm::attack_range;
	PredatorSwarm::attack_delay = (int)json["predator"]["eat_delay"].number_value();

	PreySwarm::energy_start = (float)json["prey"]["energy"]["start_value"].number_value();
	PreySwarm::energy_gain_per_eat = (float)json["prey"]["energy"]["gain_per_eat"].number_value();
	PreySwarm::energy_drain_per_step = (float)json["prey"]["energy"]["drain_per_step"].number_value();

	PredatorSwarm::confusion_range = (float)json["predator"]["confusion_effect"]["range"].number_value();
	PredatorSwarm::confusion_range_squared = PredatorSwarm::confusion_range*PredatorSwarm::confusion_range;
	PredatorSwarm::confusion_ratio = (float)json["predator"]["confusion_effect"]["ratio"].number_value();


	Simulation::world_size = (float)json["environment"]["world_size"].number_value();
	Simulation::world_size_half = Simulation::world_size / 2.f;
	Simulation::simulation_steps = (int)json["environment"]["simulation_steps"].number_value();
	Simulation::steps_without_predators = (int)json["environment"]["steps_without_predators"].number_value();
}
