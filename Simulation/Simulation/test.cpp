#include <iostream>
#include <chrono>
#include "Simulation.h"
#include "EnvManager.h"
#include <Eigen\Dense>


int main()
{	
	srand(time(NULL));

	EnvManager::set_parameters("C:/Users/adek1/source/repos/ArtificalLife/config.json");
	Simulation sim = Simulation(true);

	//std::vector<Simulation> simulations;

	//for (int i = 0; i < 10; i++)
	//	simulations.push_back(Simulation(false));

	//auto start = std::chrono::system_clock::now();

	sim.runSingleEpisode();
	//sim.create_visualization("C:/Users/adek1/source/repos/ArtificalLife/Results/vis_0.txt");

	
	//for (int s = 0; s < 1000; s++)
	//{
	//	for (int i = 0; i < 10; i++)
	//		simulations[i].runInThread();

	//	for (int i = 0; i < 10; i++)
	//		simulations[i].threadJoin();
	//}

	//auto stop = std::chrono::system_clock::now();
	//double elapsed_secs = std::chrono::duration<double>{stop - start}.count();
	//std::cout << "Time taken : " << elapsed_secs << std::endl;
	std::cout << "end";
	std::cin.get();
}