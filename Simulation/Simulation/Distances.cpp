#include "Distances.h"
#include "PreySwarm.h"
#include "PredatorSwarm.h"

const float Distances::deg2rad = 3.14f / 180.f;
const float Distances::rad2deg = 180.f / 3.14f;

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
	calculate_distances(prey_swarm->position, prey_swarm->position, prey_distances);
}

void Distances::recalculate_predator_distances()
{
	calculate_distances(predator_swarm->position, predator_swarm->position, predator_distances);
	calculate_distances(prey_swarm->position, predator_swarm->position, prey_predator_distances);
}

void Distances::reset()
{
	prey_distances.setZero();
	predator_distances.setZero();
	prey_predator_distances.setZero();
}

void Distances::calculate_distances(Eigen::MatrixXf & A, Eigen::MatrixXf & B, Eigen::MatrixXf & C)
{
	C = ((-2 * A * B.transpose()).rowwise() + B.rowwise().squaredNorm().transpose()).colwise() + A.rowwise().squaredNorm();
}

float Distances::calculate_angle(float xa, float ya, float a, float xb, float yb)
{
	float Ux = xb - xa;
	float Uy = yb - ya;
	a = a * (3.14f / 180.f);
	float Vx = std::cos(a);
	float Vy = std::sin(a);
	return std::atan2((Ux * Vy) - (Uy * Vx), (Ux * Vx) + (Uy * Vy)) * (180.f/3.14f);
}
