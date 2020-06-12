#include "Distances.h"
#include "PreySwarm.h"
#include "PredatorSwarm.h"
#include "Simulation.h"

const float Distances::deg2rad = 3.14f / 180.f;
const float Distances::rad2deg = 180.f / 3.14f;

void calculate_distances(Eigen::MatrixXf & A, Eigen::MatrixXf & B, Eigen::MatrixXf & C)
{
	C = ((-2 * A * B.transpose()).rowwise() + B.rowwise().squaredNorm().transpose()).colwise() + A.rowwise().squaredNorm();
}

void calculate_distances_2(Eigen::MatrixXf & A, Eigen::MatrixXf & C)
{
	float dx;
	float dy;
	float d;
	for (int i = 0; i < PreySwarm::population_size; i++)
	{
		for (int j = i + 1; j < PreySwarm::population_size; j++)
		{
			dx = std::abs(A(i, 0) - A(j, 0));
			if (dx > Simulation::world_size_half)
				dx -= Simulation::world_size;
			dy = std::abs(A(i, 1) - A(j, 1));
			if (dy > Simulation::world_size_half)
				dy -= Simulation::world_size;
			d = dx * dx + dy * dy;
			C(i, j) = d;
			C(j, i) = d;
		}
	}
}

Distances::Distances()
{
}

Distances::Distances(PreySwarm &prey_swarm, PredatorSwarm &predator_swarm)
{
	this->prey_swarm = &prey_swarm;
	this->predator_swarm = &predator_swarm;

	prey_distances = Eigen::MatrixXf(prey_swarm.population_size, prey_swarm.population_size);
	predator_distances = Eigen::MatrixXf(predator_swarm.population_size, predator_swarm.population_size);
	prey_predator_distances = Eigen::MatrixXf(prey_swarm.population_size, predator_swarm.population_size);
}

Distances::~Distances()
{
	prey_distances.resize(0, 0);
	predator_distances.resize(0, 0);
	prey_predator_distances.resize(0, 0);
}

void Distances::recalculate_prey_distances()
{
	//calculate_distances(prey_swarm->position, prey_swarm->position, prey_distances);
	//calculate_distances_2(prey_swarm->position, prey_distances);
	float dx;
	float dy;
	float d;
	for (int i = 0; i < PreySwarm::population_size; i++)
	{
		for (int j = i + 1; j < PreySwarm::population_size; j++)
		{
			dx = std::abs(prey_swarm->position(i, 0) - prey_swarm->position(j, 0));
			if (dx > Simulation::world_size_half)
				dx -= Simulation::world_size;
			dy = std::abs(prey_swarm->position(i, 1) - prey_swarm->position(j, 1));
			if (dy > Simulation::world_size_half)
				dy -= Simulation::world_size;
			d = dx * dx + dy * dy;
			prey_distances(i, j) = d;
			prey_distances(j, i) = d;
		}
	}
}

void Distances::recalculate_prey_predator_distances()
{
	//calculate_distances(predator_swarm->position, predator_swarm->position, predator_distances);
	//calculate_distances(prey_swarm->position, predator_swarm->position, prey_predator_distances);
	float dx;
	float dy;
	float d;
	for (int i = 0; i < PreySwarm::population_size; i++)
	{
		if (prey_swarm->alive[i])
		{
			for (int j = i + 1; j < PreySwarm::population_size; j++)
			{
				if (prey_swarm->alive[j])
				{
					dx = std::abs(prey_swarm->position(i, 0) - prey_swarm->position(j, 0));
					if (dx > Simulation::world_size_half)
						dx -= Simulation::world_size;
					dy = std::abs(prey_swarm->position(i, 1) - prey_swarm->position(j, 1));
					if (dy > Simulation::world_size_half)
						dy -= Simulation::world_size;
					d = dx * dx + dy * dy;
					prey_distances(i, j) = d;
					prey_distances(j, i) = d;
				}
			}

			for (int j = 0; j < PredatorSwarm::population_size; j++)
			{
				dx = std::abs(prey_swarm->position(i, 0) - predator_swarm->position(j, 0));
				if (dx > Simulation::world_size_half)
					dx -= Simulation::world_size;
				dy = std::abs(prey_swarm->position(i, 1) - predator_swarm->position(j, 1));
				if (dy > Simulation::world_size_half)
					dy -= Simulation::world_size;
				prey_predator_distances(i, j) = dx * dx + dy * dy;
			}
		}
	}

	for (int i = 0; i < PredatorSwarm::population_size; i++)
	{
		for (int j = i + 1; j < PredatorSwarm::population_size; j++)
		{
			dx = std::abs(predator_swarm->position(i, 0) - predator_swarm->position(j, 0));
			if (dx > Simulation::world_size_half)
				dx -= Simulation::world_size;
			dy = std::abs(predator_swarm->position(i, 1) - predator_swarm->position(j, 1));
			if (dy > Simulation::world_size_half)
				dy -= Simulation::world_size;
			d = dx * dx + dy * dy;
			predator_distances(i, j) = d;
			predator_distances(j, i) = d;
		}
	}
}

void Distances::reset()
{
	prey_distances.setZero();
	predator_distances.setZero();
	prey_predator_distances.setZero();
}

float calculate_angle(float xa, float ya, float a, float xb, float yb)
{
	float Ux = xb - xa;
	float Uy = yb - ya;
	a = a * (3.14f / 180.f);
	float Vx = std::cos(a);
	float Vy = std::sin(a);
	return std::atan2((Ux * Vy) - (Uy * Vx), (Ux * Vx) + (Uy * Vy)) * (180.f/3.14f);
}
