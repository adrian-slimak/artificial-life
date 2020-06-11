#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

//void calculate_distances(MatrixXf &A, MatrixXf &C)
//{
//	int size = A.rows();
//	int block_size = size-2;
//	int col = 1;
//	for (int row = 0; row < size - 1; row++)
//	{
//		//for (int col = row + 1; col < size; col++)
//		{
//			VectorXf V(Map<VectorXf>(A.block(row, 0, 1, 2).data(), 2));
//			//Eigen::Matrix<float, 2, 2> C = A.block(0, 0, 2, 2).rowwise() - V.transpose();
//			C.block(row, col, 1, block_size) = (A.block(col, 0, block_size, 2).rowwise() - V.transpose()).rowwise().squaredNorm().transpose();
//			//std::cout << k << std::endl;
//			//C.block(1, block_size, row, col) = (A.block(block_size, 2, row, 0).rowwise()-A.block<1, 2>(row, 0)).rowwise().squaredNorm().transpose();
//			col++;
//			block_size--;
//		}
//	}
//}

#include "Simulation.h"
#include "json11.hpp"
#include "EnvManager.h"

int main()
{	
	EnvManager::set_parameters("C:/Users/adek1/source/repos/Simulation/config.json");
	//Simulation sim = Simulation(true);
	//sim.runSingleEpisode();

	//std::vector<Simulation> simulations;

	//for (int i = 0; i < 20; i++)
	//	simulations.push_back(Simulation(false));

	//auto start = std::chrono::system_clock::now();
	//
	//for (int s = 0; s < 5; s++)
	//{
	//	for (int i = 0; i < 20; i++)
	//		simulations[i].runInThread();

	//	for (int i = 0; i < 20; i++)
	//		simulations[i].threadJoin();
	//}

	//auto stop = std::chrono::system_clock::now();
	//double elapsed_secs = std::chrono::duration<double>{stop - start}.count();
	//std::cout << "Time taken : " << elapsed_secs << std::endl;

	std::cin.get();
}