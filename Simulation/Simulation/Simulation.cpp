#include "Simulation.h"
#include <iostream>
#include <fstream>

float Simulation::world_size = 512.f;
float Simulation::world_size_half = 256.f;
int Simulation::simulation_steps = 4000;
int Simulation::steps_without_predators = 250;

Simulation::Simulation()
{
	prey_swarm = new PreySwarm();
	predator_swarm = new PredatorSwarm();

	distances = new Distances(*prey_swarm, *predator_swarm);

	this->prey_swarm->predator_swarm = predator_swarm;
	this->prey_swarm->distances = distances;

	this->predator_swarm->prey_swarm = prey_swarm;
	this->predator_swarm->distances = distances;
}

Simulation::Simulation(bool show_visualization)
{
	prey_swarm = new PreySwarm();
	predator_swarm = new PredatorSwarm();

	distances = new Distances(*prey_swarm, *predator_swarm);

	this->prey_swarm->predator_swarm = predator_swarm;
	this->prey_swarm->distances = distances;

	this->predator_swarm->prey_swarm = prey_swarm;
	this->predator_swarm->distances = distances;

	//if (show_visualization) visualization = new Visualization(*prey_swarm, *predator_swarm);
}

Simulation::~Simulation()
{
	//delete prey_swarm;
	//delete predator_swarm;
	//delete distances;
	//delete visualization;
}

void Simulation::reset()
{
	prey_swarm->reset();
	predator_swarm->reset();

	step = 0;
}

void Simulation::setModels(float* prey_genes, float* predator_genes)
{
	this->prey_swarm->set_model(prey_genes);
	this->predator_swarm->set_model(predator_genes);
}

void Simulation::runSingleEpisode()
{
	//if (PreySwarm::food_spawn_method == 2)
	//{
	//	Simulation::world_size = 256.f;
	//	Simulation::world_size_half = 128.f;
	//}
	this->reset();

	//while (step < 100)
	//{
	//	this->distances->recalculate_prey_observations();
	//	this->distances->prey_observations();

	//	this->prey_swarm->update_decisions();

	//	if (visualization) visualization->render();

	//	this->prey_swarm->update_movement();

	//	this->prey_swarm->update_fitness();

	//	step++;
	//}

	//if (PreySwarm::food_spawn_method == 2)
	//{
	//	Simulation::world_size = 512.f;
	//	Simulation::world_size_half = 256.f;
	//}

	// Warmup without predators
	while (step < steps_without_predators)
	{
		this->distances->recalculate_prey_observations();
		if (PreySwarm::food_enabled)
			this->distances->recalculate_prey_food_observations();
		this->distances->prey_observations();

		// Update Density and Dispersion
		this->prey_swarm->update_stats(); // Tutaj bo potem ich ruszam...

		if (PreySwarm::food_enabled)
		{
			this->prey_swarm->try_eat();
			this->prey_swarm->update_food();
		}

		this->prey_swarm->update_decisions();

		//if (visualization) visualization->render();

		this->prey_swarm->update_movement();

		this->prey_swarm->update_fitness();

		step++;
	}

	// Main simulation loop
	while (step < simulation_steps)
	{
		this->distances->recalculate_prey_observations();
		if (PreySwarm::food_enabled)
			this->distances->recalculate_prey_food_observations();
		this->distances->recalculate_prey_predator_observations();
		this->distances->prey_observations();
		this->distances->predator_observations();

		// Update Density and Dispersion
		this->prey_swarm->update_stats();
		this->predator_swarm->update_stats();

		this->predator_swarm->try_hunt(); // Przed update stats???
		if (PreySwarm::food_enabled)
		{
			this->prey_swarm->try_eat();
			this->prey_swarm->update_food();
		}

		prey_swarm->update_decisions();
		predator_swarm->update_decisions();

		//if (visualization) visualization->render();

		this->prey_swarm->update_movement();
		this->predator_swarm->update_movement();

		this->prey_swarm->update_fitness();
		this->predator_swarm->update_fitness();

		step++;
	}

	this->prey_swarm->mean_density /= simulation_steps;
	this->prey_swarm->mean_dispersion /= simulation_steps;
	if (PreySwarm::food_enabled)
		this->prey_swarm->mean_eats /= PreySwarm::population_size;

	int simulation_steps_predators = (simulation_steps - steps_without_predators);
	if (simulation_steps_predators > 0)
	{
		this->predator_swarm->mean_density /= (float)simulation_steps_predators;
		this->predator_swarm->mean_dispersion /= (float)simulation_steps_predators; // Czy na pewno tak i czy to ma sens??
		this->predator_swarm->mean_attacks = this->predator_swarm->number_attacks / PredatorSwarm::population_size;
		this->predator_swarm->mean_hunts = this->predator_swarm->number_hunts / PredatorSwarm::population_size;
	}
}

void Simulation::create_visualization(const char *vis_file_path)
{
	std::ofstream vis_file;
	vis_file.open(vis_file_path);

	if (PreySwarm::food_enabled)
	{
		vis_file << PreySwarm::population_size << "|" << PredatorSwarm::population_size << "|" << PreySwarm::food_amount << "\n";
		vis_file << this->prey_swarm->to_string() << "\n" << this->predator_swarm->to_string() << "\n" << this->prey_swarm->food_to_string() << "\n";
	}
	else
	{
		vis_file << PreySwarm::population_size << "|" << PredatorSwarm::population_size << "\n";
		vis_file << this->prey_swarm->to_string() << "\n" << this->predator_swarm->to_string() << "\n";
	}

	//if (PreySwarm::food_spawn_method == 2)
	//{
	//	Simulation::world_size = 256.f;
	//	Simulation::world_size_half = 128.f;
	//}

	this->reset();

	//while (step < 100)
	//{
	//	this->distances->recalculate_prey_observations();
	//	this->distances->prey_observations();

	//	this->prey_swarm->update_decisions();

	//	this->prey_swarm->update_movement();
	//	vis_file << this->prey_swarm->to_string() << "\n" << this->predator_swarm->to_string() << "\n";
	//	if (PreySwarm::food_enabled)
	//		vis_file << this->prey_swarm->food_to_string() << "\n";

	//	this->prey_swarm->update_fitness();

	//	step++;
	//}

	//if (PreySwarm::food_spawn_method == 2)
	//{
	//	Simulation::world_size = 512.f;
	//	Simulation::world_size_half = 256.f;
	//}


	// Warmup without predators
	while (step < steps_without_predators)
	{
		this->distances->recalculate_prey_observations();
		if (PreySwarm::food_enabled)
			this->distances->recalculate_prey_food_observations();
		this->distances->prey_observations();

		// Update Density and Dispersion
		this->prey_swarm->update_stats(); // Tutaj bo potem ich ruszam...

		if (PreySwarm::food_enabled)
		{
			this->prey_swarm->try_eat();
			this->prey_swarm->update_food();
		}

		this->prey_swarm->update_decisions();


		this->prey_swarm->update_movement();
		vis_file << this->prey_swarm->to_string() << "\n" << this->predator_swarm->to_string() << "\n";
		if(PreySwarm::food_enabled)
			vis_file << this->prey_swarm->food_to_string() << "\n";

		this->prey_swarm->update_fitness();

		step++;
	}

	// Main simulation loop
	while (step < simulation_steps)
	{
		this->distances->recalculate_prey_observations();
		if (PreySwarm::food_enabled)
			this->distances->recalculate_prey_food_observations();
		this->distances->recalculate_prey_predator_observations();
		this->distances->prey_observations();
		this->distances->predator_observations();

		// Update Density and Dispersion
		this->prey_swarm->update_stats();
		this->predator_swarm->update_stats();

		this->predator_swarm->try_hunt(); // Przed update stats???
		if (PreySwarm::food_enabled)
		{
			this->prey_swarm->try_eat();
			this->prey_swarm->update_food();
		}

		prey_swarm->update_decisions();
		predator_swarm->update_decisions();


		this->prey_swarm->update_movement();
		this->predator_swarm->update_movement();
		vis_file << this->prey_swarm->to_string() << "\n" << this->predator_swarm->to_string() << "\n";
		if(PreySwarm::food_enabled)
			vis_file << this->prey_swarm->food_to_string() << "\n";

		this->prey_swarm->update_fitness();
		this->predator_swarm->update_fitness();

		step++;
	}

	vis_file.close();
}

void Simulation::runInThread()
{
	_thread = new std::thread([this] { runSingleEpisode(); });
}

void Simulation::threadJoin()
{
	_thread->join();
	delete _thread;
}
