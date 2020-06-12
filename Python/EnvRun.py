from other.live_plot import LivePlot
from other.utils import save_genes
import learning_parameters as _lp
from GA import GeneticAlgorithm
import plots_parameters as _pp
import numpy as np
import EnvManager
from time import time
np.set_printoptions(precision=3)
np.set_printoptions(suppress=True)

def main():
    live_plot = LivePlot(plots=_pp.plot_structure, subplots=_pp.plot_subplots, figsize=_pp.plot_size) if _lp.show_plots else None

    EnvManager.set_parameters(_lp.config_file_path)

    # Get pointers to fitness array and stats array
    prey_fitness = np.zeros(_lp.population_size).astype(np.float32)
    predator_fitness = np.zeros(_lp.population_size).astype(np.float32)
    prey_stats = np.zeros(5).astype(np.float32)
    predator_stats = np.zeros(5).astype(np.float32)
    prey_fitness_pointer = float(prey_fitness.__array_interface__['data'][0])
    predator_fitness_pointer = float(predator_fitness.__array_interface__['data'][0])
    prey_stats_pointer = float(prey_stats.__array_interface__['data'][0])
    predator_stats_pointer = float(predator_stats.__array_interface__['data'][0])

    # Create Genetic Algorithms for prey swarm and predator swarm
    GA_prey = GeneticAlgorithm(_lp.prey_observations_size, _lp.prey_brain_cells, _lp.prey_actions_size, _lp.population_size, "lstm")
    GA_predator = GeneticAlgorithm(_lp.predator_observations_size, _lp.predator_brain_cells, _lp.predator_actions_size, _lp.population_size, "lstm")
    # Initialize Genetic Algorithms with random genes
    GA_prey.initial_population()
    GA_predator.initial_population()

    # Create Environment Manager
    env_manager = EnvManager.EnvManager()
    # Set EnvManager pointers to fitness and stats arrays
    env_manager.set_fitness_pointers(prey_fitness_pointer, predator_fitness_pointer)
    env_manager.set_stats_pointers(prey_stats_pointer, predator_stats_pointer)

    # GAs['prey'].load_population_from_file('genes_9.pkl', 'prey')
    # GAs['predator'].load_population_from_file('genes_9.pkl', 'predator')

    _lp.hotkey_listener.add('<ctrl>+<alt>+a', lambda: save_genes({'prey': GA_prey, 'predator': GA_predator}, _lp.save_ID))

    for generation in range(_lp.number_of_generations):
        prey_genes = GA_prey.to_genes()
        predator_genes = GA_predator.to_genes()

        prey_genes_pointer = float(prey_genes.__array_interface__['data'][0])
        predator_genes_pointer = float(predator_genes.__array_interface__['data'][0])

        # Evaluate genes
        env_manager.set_prey_genes(prey_genes_pointer, prey_genes.shape[0], prey_genes.shape[1])
        env_manager.set_predator_genes(predator_genes_pointer, predator_genes.shape[0], predator_genes.shape[1])
        env_manager.run_single_episode()

        GA_prey.calc_fitness(prey_fitness)
        GA_predator.calc_fitness(predator_fitness)

        if live_plot:
            live_plot.update({'prey1': [np.average(prey_fitness), np.max(prey_fitness), np.min(prey_fitness)],
                              'prey2': [prey_stats[0], prey_stats[2]],
                              'prey3': [prey_stats[1], prey_stats[4]],
                              'predator1': [np.average(predator_fitness), np.max(predator_fitness), np.min(predator_fitness)],
                              'predator2': [predator_stats[1], predator_stats[2], predator_stats[3], predator_stats[4]]
                              })

        # GA_prey.next_generation()
        GA_predator.next_generation()


    if live_plot:
        live_plot.close()


if __name__ == "__main__":
    main()
