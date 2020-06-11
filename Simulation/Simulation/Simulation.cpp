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

	if (show_visualization) visualization = new Visualization(*prey_swarm, *predator_swarm);
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

	while (step < steps_without_predators)
	{
		this->distances->recalculate_prey_distances();
		this->prey_swarm->update_observations_preys();
		this->prey_swarm->update_decisions();

		if (visualization) visualization->render();

		this->prey_swarm->update_movement();

		this->prey_swarm->update_fitness();
		this->prey_swarm->update_stats();

		step++;
	}

 	while (step < simulation_steps)
	{
		updateObservations();

		prey_swarm->update_decisions();
		predator_swarm->update_decisions();

		if (visualization) visualization->render();

		this->prey_swarm->update_movement(); // ROZKMINIc, CZY NA PEWNO dobra kolejnosc, jak mieli w innych pracach
		this->predator_swarm->update_movement();
        this->predator_swarm->try_hunt(); // Tutaj czy potem??

		this->prey_swarm->update_fitness();
		this->predator_swarm->update_fitness();
		this->prey_swarm->update_stats();
		this->predator_swarm->update_stats();

		step++;
	}

    this->prey_swarm->mean_density /= simulation_steps;
	this->prey_swarm->mean_dispersion /= simulation_steps;

    float simulation_steps_predators = (simulation_steps - steps_without_predators);
	this->predator_swarm->mean_density /= simulation_steps_predators;
	this->predator_swarm->mean_dispersion /= simulation_steps_predators;
	this->predator_swarm->mean_attacks /= simulation_steps_predators;  // Czy na pewno tak i czy to ma sens??
	this->predator_swarm->mean_hunts /= simulation_steps_predators;
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

void Simulation::updateObservationsPreys()
{
	this->distances->recalculate_prey_distances();
	this->prey_swarm->update_observations_preys();
}

void Simulation::updateObservations()
{
	this->distances->recalculate_prey_distances();
	this->distances->recalculate_predator_distances();

	this->prey_swarm->update_observations();
	this->predator_swarm->update_observations();
}

void Simulation::updateMovement()
{
	this->predator_swarm->try_hunt();

	this->prey_swarm->update_movement();
	this->predator_swarm->update_movement();
}
