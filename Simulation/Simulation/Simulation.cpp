#include "Simulation.h"
#include <iostream>

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
	distances->reset();
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
	this->reset();


	// Warmup without predators
	while (step < steps_without_predators)
	{
		this->distances->recalculate_prey_distances_observations();

		this->prey_swarm->update_decisions();

		this->prey_swarm->update_stats(); // Tutaj bo potem ich ruszam...

		//if (visualization) visualization->render();

		this->prey_swarm->update_movement();

		this->prey_swarm->update_fitness();

		step++;
	}

	// Main simulation loop
	while (step < simulation_steps)
	{
		this->distances->recalculate_prey_predator_distances_observations();
		this->predator_swarm->try_hunt(); // Przed update stats???

		prey_swarm->update_decisions();
		predator_swarm->update_decisions();

		this->prey_swarm->update_stats();
		this->predator_swarm->update_stats();

		//if (visualization) visualization->render();

		this->predator_swarm->update_movement();
		this->prey_swarm->update_movement();

		this->prey_swarm->update_fitness();
		this->predator_swarm->update_fitness();

		step++;
	}

	this->prey_swarm->mean_density /= simulation_steps;
	this->prey_swarm->mean_dispersion /= simulation_steps;

	int simulation_steps_predators = (simulation_steps - steps_without_predators);
	this->predator_swarm->mean_density /= simulation_steps_predators;
	this->predator_swarm->mean_dispersion /= simulation_steps_predators; // Czy na pewno tak i czy to ma sens??
	this->predator_swarm->mean_attacks = this->predator_swarm->number_attacks / (float)this->predator_swarm->number_alive;
	this->predator_swarm->mean_hunts = this->predator_swarm->number_hunts / (float)this->predator_swarm->number_alive;
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
