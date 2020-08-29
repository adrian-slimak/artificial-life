#include "Distances.h"
#include "PreySwarm.h"
#include "PredatorSwarm.h"
#include "Simulation.h"

#include <iostream>

const float Distances::PI = 3.14159265f;
const float Distances::PI_2 = Distances::PI * 2.f;

const float Distances::deg2rad = Distances::PI / 180.f;
const float Distances::rad2deg = 180.f / Distances::PI;

Distances::Distances()
{
}

Distances::Distances(PreySwarm &prey_swarm, PredatorSwarm &predator_swarm)
{
	this->prey_swarm = &prey_swarm;
	this->predator_swarm = &predator_swarm;

	//Initialize prey->prey distances table
	prey_distances = new float*[PreySwarm::population_size];
	for (int i = 0; i < PreySwarm::population_size; ++i)
		prey_distances[i] = new float[PreySwarm::population_size];

	//Initialize predator->predator distances table
	predator_distances = new float*[PredatorSwarm::population_size];
	for (int i = 0; i < PredatorSwarm::population_size; ++i)
		predator_distances[i] = new float[PredatorSwarm::population_size];

	//Initialize predator->prey distances table
	//Initialize predator->prey angles table
	predator_prey_distances = new float*[PredatorSwarm::population_size];
	predator_prey_angles = new float*[PredatorSwarm::population_size];
	for (int i = 0; i < PredatorSwarm::population_size; ++i)
	{
		predator_prey_distances[i] = new float[PreySwarm::population_size];
		for (int j = 0; j < PreySwarm::population_size; j++)
			predator_prey_distances[i][j] = 0.f;
		predator_prey_angles[i] = new float[PreySwarm::population_size];
	}

	//Initialize prey->food distances table
	//Initialize prey->food angles table
	prey_food_distances = new float*[PreySwarm::population_size];
	prey_food_angles = new float*[PreySwarm::population_size];
	for (int i = 0; i < PreySwarm::population_size; ++i)
	{
		prey_food_distances[i] = new float[PreySwarm::food_amount];
		prey_food_angles[i] = new float[PreySwarm::food_amount];
	}
}

Distances::~Distances()
{
	// Delete prey->prey distances table
	for (int i = 0; i < PreySwarm::population_size; ++i)
		delete[] prey_distances[i];
	delete[] prey_distances;

	// Delete predator->predator distances table
	for (int i = 0; i < PredatorSwarm::population_size; ++i)
		delete[] predator_distances[i];
	delete[] predator_distances;

	// Delete predator->prey distances table
	// Delete predator->prey angles table
	for (int i = 0; i < PredatorSwarm::population_size; ++i)
	{
		delete[] predator_prey_distances[i];
		delete[] predator_prey_angles[i];
	}
	delete[] predator_prey_distances;
	delete[] predator_prey_angles;

	for (int i = 0; i < PreySwarm::population_size; ++i)
	{
		delete[] prey_food_distances[i];
		delete[] prey_food_angles[i];
	}
	delete[] prey_food_distances;
	delete[] prey_food_angles;
}

void Distances::recalculate_prey_observations()
{
	this->prey_swarm->model->x.leftCols(PreySwarm::vision_size).setConstant(1000000.f);
	if(PreySwarm::communication_enabled)
		this->prey_swarm->model->x.block(0, PreySwarm::vision_size, PreySwarm::population_size, PreySwarm::hear_cells).setConstant(0.f);
	if (PreySwarm::communication_enabled && PreySwarm::food_enabled)
		this->prey_swarm->model->x.block(0, PreySwarm::vision_size + PreySwarm::hear_cells, PreySwarm::population_size, PreySwarm::hear_cells).setConstant(0.f);

	float Ux;
	float Uy;
	float norm_a_x;
	float norm_a_y;
	float distance;
	float angle;
	int obs_id;

	for (int a = 0; a < PreySwarm::population_size; a++)
	{
		norm_a_x = this->prey_swarm->norm(a, 0);
		norm_a_y = this->prey_swarm->norm(a, 1);

		if (this->prey_swarm->alive[a])
		{
			// Update Preys with Preys
			for (int b = a + 1; b < PreySwarm::population_size; b++)
			{
				if (this->prey_swarm->alive[b])
				{
					Ux = this->prey_swarm->position(b, 0) - this->prey_swarm->position(a, 0);
					Uy = this->prey_swarm->position(b, 1) - this->prey_swarm->position(a, 1);

					if (Ux > Simulation::world_size_half)
						Ux -= Simulation::world_size;
					else if (Ux < -Simulation::world_size_half)
						Ux += Simulation::world_size;
					if (Uy > Simulation::world_size_half)
						Uy -= Simulation::world_size;
					else if (Uy < -Simulation::world_size_half)
						Uy += Simulation::world_size;

					distance = Ux * Ux + Uy * Uy;

					this->prey_distances[a][b] = distance;
					this->prey_distances[b][a] = distance;

					if (distance < PreySwarm::vision_range_squared)
					{
						angle = std::atan2((Ux * norm_a_y) - (Uy * norm_a_x), (Ux * norm_a_x) + (Uy * norm_a_y));

						if (std::abs(angle) < PreySwarm::vision_angle_half_rad)
						{
							obs_id = int((PreySwarm::vision_angle_half_rad - angle) / PreySwarm::vision_cell_angle_rad);
							if (distance < this->prey_swarm->model->x(a, obs_id))
								this->prey_swarm->model->x(a, obs_id) = distance;
						}

						//Communication
						if (PreySwarm::communication_enabled)
						{
							// Update hearing observations for specific hear cell
							obs_id = int((Distances::PI - angle) / PreySwarm::hear_cell_angle_rad) * 2 + PreySwarm::vision_size;
							if (obs_id >= PreySwarm::observations_size) // Because of some stupid issue with rounded PI value... greater precision doesn't work
								obs_id = PreySwarm::observations_size - 2;

							if (this->prey_swarm->predator_sound_active[b])
								this->prey_swarm->model->x(a, obs_id) = 1.f;

							if (PreySwarm::food_enabled)
							{
								if (this->prey_swarm->food_sound_active[b])
									this->prey_swarm->model->x(a, obs_id + 1) = 1.f;
							}
						}

						Ux *= -1;
						Uy *= -1;

						angle = std::atan2((Ux * this->prey_swarm->norm(b, 1)) - (Uy * this->prey_swarm->norm(b, 0)), (Ux * this->prey_swarm->norm(b, 0)) + (Uy * this->prey_swarm->norm(b, 1)));

						if (std::abs(angle) < PreySwarm::vision_angle_half_rad)
						{
							obs_id = int((PreySwarm::vision_angle_half_rad - angle) / PreySwarm::vision_cell_angle_rad);
							if (distance < this->prey_swarm->model->x(b, obs_id))
								this->prey_swarm->model->x(b, obs_id) = distance;
						}

						//Communication
						if (PreySwarm::communication_enabled)
						{
							// Update hearing observations for specific hear cell
							obs_id = int((Distances::PI - angle) / PreySwarm::hear_cell_angle_rad) * 2 + PreySwarm::vision_size;
							if (obs_id >= PreySwarm::observations_size) // Because of some stupid issue with rounded PI value... greater precision doesn't work
								obs_id = PreySwarm::observations_size - 2;

							if (this->prey_swarm->predator_sound_active[a])
								this->prey_swarm->model->x(b, obs_id) = 1.f;

							if (PreySwarm::food_enabled)
							{
								if (this->prey_swarm->food_sound_active[a])
									this->prey_swarm->model->x(b, obs_id + 1) = 1.f;
							}
						}
					}
				}
			}
		}
	}
}

void Distances::recalculate_prey_food_observations()
{
	float Ux;
	float Uy;
	float norm_a_x;
	float norm_a_y;
	float distance;
	float angle;
	int obs_id;

	for (int p = 0; p < PreySwarm::population_size; p++)
	{
		norm_a_x = this->prey_swarm->norm(p, 0);
		norm_a_y = this->prey_swarm->norm(p, 1);

		if (this->prey_swarm->alive[p])
		{
			// Update Preys with Food
			for (int f = 0; f < PreySwarm::food_amount; f++)
			{
				if (this->prey_swarm->food_alive[f])
				{
					Ux = this->prey_swarm->food_position(f, 0) - this->prey_swarm->position(p, 0);
					Uy = this->prey_swarm->food_position(f, 1) - this->prey_swarm->position(p, 1);

					if (Ux > Simulation::world_size_half)
						Ux -= Simulation::world_size;
					else if (Ux < -Simulation::world_size_half)
						Ux += Simulation::world_size;
					if (Uy > Simulation::world_size_half)
						Uy -= Simulation::world_size;
					else if (Uy < -Simulation::world_size_half)
						Uy += Simulation::world_size;

					distance = Ux * Ux + Uy * Uy;

					this->prey_food_distances[p][f] = distance;

					if (distance < PreySwarm::vision_range_squared)
					{
						angle = std::atan2((Ux * norm_a_y) - (Uy * norm_a_x), (Ux * norm_a_x) + (Uy * norm_a_y));

						prey_food_angles[p][f] = angle;

						if (std::abs(angle) < PreySwarm::vision_angle_half_rad)
						{
							 obs_id = int((PreySwarm::vision_angle_half_rad - angle) / PreySwarm::vision_cell_angle_rad) + PreySwarm::vision_cells * 2;
							//obs_id = int((PreySwarm::vision_angle_half_rad - angle) / PreySwarm::vision_cell_angle_rad) + PreySwarm::vision_cells; // HEREEEEEEEEEEEEEEEEEEEEEEEEEEE
							if (distance < this->prey_swarm->model->x(p, obs_id))
								this->prey_swarm->model->x(p, obs_id) = distance;
						}
					}
				}
			}
		}
	}
}

void Distances::prey_observations()
{
	this->prey_swarm->model->x.leftCols(PreySwarm::vision_size) = this->prey_swarm->model->x.leftCols(PreySwarm::vision_size).unaryExpr([](float elem)
	{
		return elem == 1000000.f ? 0.0f : std::sqrt(elem) / PreySwarm::vision_range;
	});
}

void Distances::recalculate_prey_predator_observations()
{
	this->predator_swarm->model->x.leftCols(PredatorSwarm::vision_size).setConstant(1000000.f);
	if(PredatorSwarm::communication_enabled)
		this->predator_swarm->model->x.block(0,PredatorSwarm::vision_size, PredatorSwarm::population_size, PredatorSwarm::hear_cells).setConstant(0.f);

	float Ux;
	float Uy;
	float distance;
	float angle;
	float norm_a_x;
	float norm_a_y;
	int obs_id;

	// Update Preys with Predators
	// For each prey a...
	for (int a = 0; a < PreySwarm::population_size; a++)
	{
		if (this->prey_swarm->alive[a])
		{
			norm_a_x = this->prey_swarm->norm(a, 0);
			norm_a_y = this->prey_swarm->norm(a, 1);

			// Update each predator p...
			for (int p = 0; p < PredatorSwarm::population_size; p++)
			{
				Ux = this->predator_swarm->position(p, 0) - this->prey_swarm->position(a, 0);
				Uy = this->predator_swarm->position(p, 1) - this->prey_swarm->position(a, 1);

				if (Ux > Simulation::world_size_half)
					Ux -= Simulation::world_size;
				else if (Ux < -Simulation::world_size_half)
					Ux += Simulation::world_size;
				if (Uy > Simulation::world_size_half)
					Uy -= Simulation::world_size;
				else if (Uy < -Simulation::world_size_half)
					Uy += Simulation::world_size;

				distance = Ux * Ux + Uy * Uy;

				predator_prey_distances[p][a] = distance;

				if (distance < PreySwarm::vision_range_squared)
				{
					angle = std::atan2((Ux * norm_a_y) - (Uy * norm_a_x), (Ux * norm_a_x) + (Uy * norm_a_y));
					
					if (std::abs(angle) < PreySwarm::vision_angle_half_rad)
					{
						obs_id = int((PreySwarm::vision_angle_half_rad - angle) / PreySwarm::vision_cell_angle_rad) + PreySwarm::vision_cells;
						if (distance < prey_swarm->model->x(a, obs_id))
							prey_swarm->model->x(a, obs_id) = distance;
					}
				}

				Ux *= -1;
				Uy *= -1;

				if (distance < PredatorSwarm::vision_range_squared)
				{
					angle = std::atan2((Ux * this->predator_swarm->norm(p, 1)) - (Uy * this->predator_swarm->norm(p, 0)), (Ux * this->predator_swarm->norm(p, 0)) + (Uy * this->predator_swarm->norm(p, 1)));

					predator_prey_angles[p][a] = angle;

					if (std::abs(angle) < PredatorSwarm::vision_angle_half_rad)
					{
						obs_id = int((PredatorSwarm::vision_angle_half_rad - angle) / PredatorSwarm::vision_cell_angle_rad);
						if (distance < this->predator_swarm->model->x(p, obs_id))
							this->predator_swarm->model->x(p, obs_id) = distance;
					}
				}
			}
		}
	}

	// Update Predators with Predators
	// For each predator a...
	for (int a = 0; a < PredatorSwarm::population_size; a++)
	{
		// Get a direction vector (normalized)
		norm_a_x = this->predator_swarm->norm(a, 0);
		norm_a_y = this->predator_swarm->norm(a, 1);

		// For each predator b...
		for (int b = a + 1; b < PredatorSwarm::population_size; b++)
		{
			// Vector between a and b
			Ux = this->predator_swarm->position(b, 0) - this->predator_swarm->position(a, 0);
			Uy = this->predator_swarm->position(b, 1) - this->predator_swarm->position(a, 1);

			// Do this because of toroidal environment (looped on edges)
			// This way its possible for agents to sense other agents beyond environment edges
			if (Ux > Simulation::world_size_half)
				Ux -= Simulation::world_size;
			else if (Ux < -Simulation::world_size_half)
				Ux += Simulation::world_size;
			if (Uy > Simulation::world_size_half)
				Uy -= Simulation::world_size;
			else if (Uy < -Simulation::world_size_half)
				Uy += Simulation::world_size;

			// Calculate distance between a and b
			distance = Ux * Ux + Uy * Uy;

			// Save distances for later stats calculation
			this->predator_distances[a][b] = distance;
			this->predator_distances[b][a] = distance;

			// Check if a and b are in each other vision range
			// If so, update observations
			if (distance < PredatorSwarm::vision_range_squared)
			{
				// Calculate angle between a and b
				angle = std::atan2((Ux * norm_a_y) - (Uy * norm_a_x), (Ux * norm_a_x) + (Uy * norm_a_y));

				// Chech if b is in vision angle of a
				if (std::abs(angle) < PredatorSwarm::vision_angle_half_rad)
				{
					// Update observation for specific vision cell
					obs_id = int((PredatorSwarm::vision_angle_half_rad - angle) / PredatorSwarm::vision_cell_angle_rad) + PredatorSwarm::vision_cells;
					if (distance < this->predator_swarm->model->x(a, obs_id))
						this->predator_swarm->model->x(a, obs_id) = distance;
				}

				//Communication
				if (PredatorSwarm::communication_enabled)
				{
					// Update hearing observations for specific hear cell
					obs_id = int((Distances::PI - angle) / PredatorSwarm::hear_cell_angle_rad) + PredatorSwarm::vision_size;
					if (obs_id > 48) // Because of some stupid issue with rounded PI value... greater precision doens't work
						obs_id = 48;
					if (this->predator_swarm->sound_active[b])
						this->predator_swarm->model->x(a, obs_id) = 1.f;
				}

				// Because Vector between a->b is opposite to b->a
				Ux *= -1;
				Uy *= -1;

				// Calculate angle between b and a
				angle = std::atan2((Ux * this->predator_swarm->norm(b, 1)) - (Uy * this->predator_swarm->norm(b, 0)), (Ux * this->predator_swarm->norm(b, 0)) + (Uy * this->predator_swarm->norm(b, 1)));

				// Chech if a is in vision angle of b
				if (std::abs(angle) < PredatorSwarm::vision_angle_half_rad)
				{
					// Update observation for specific vision cell
					obs_id = int((PredatorSwarm::vision_angle_half_rad - angle) / PredatorSwarm::vision_cell_angle_rad) + PredatorSwarm::vision_cells;
					if (distance < this->predator_swarm->model->x(b, obs_id))
						this->predator_swarm->model->x(b, obs_id) = distance;
				}

				//Communication
				if (PredatorSwarm::communication_enabled)
				{
					// Update hearing observations for specific hear cell
					obs_id = int((Distances::PI - angle) / PredatorSwarm::hear_cell_angle_rad) + PredatorSwarm::vision_size;
					if (obs_id > 48) // Because of some stupid issue with rounded PI value... greater precision doens't work
						obs_id = 48;
					if (this->predator_swarm->sound_active[a])
						this->predator_swarm->model->x(b, obs_id) = 1.f;
				}
			}
		}
	}
}

void Distances::predator_observations()
{
	this->predator_swarm->model->x.leftCols(PredatorSwarm::vision_size) = this->predator_swarm->model->x.leftCols(PredatorSwarm::vision_size).unaryExpr([](float elem)
	{
		return elem == 1000000.f ? 0.0f : std::sqrt(elem) / PredatorSwarm::vision_range;
	});
}