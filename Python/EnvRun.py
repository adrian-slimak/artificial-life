from mlagents.multi_unity_environment import MultiUnityEnvironment
import configs.learning_parameters as _lp
from networks.GA import GeneticAlgorithm
import configs.plots_parameters as _pp
from other.live_plot import LivePlot
import numpy as np
from other.utils import save_genes


def main():
    live_plot = LivePlot(plots=_pp.plot_structure, subplots=_pp.plot_subplots, figsize=_pp.plot_size) if _lp.show_plots else None

    unity_environment = MultiUnityEnvironment(file_path=_lp.unity_environment_path,
                                              number_of_environments=_lp.number_of_environments,
                                              engine_configuration=_lp.engine_configuration,
                                              environment_parameters=_lp.environment_parameters,
                                              batch_mode=True)

    _brains = unity_environment.external_brains
    GAs = {}
    GAs['prey'] = GeneticAlgorithm(_brains['prey'].observations_vector_size, _lp.units, _brains['prey'].actions_vector_size, _brains['prey'].agents_count, model_name=_lp.NetworkModel.name, use_bias=_lp.use_bias)
    GAs['predator'] = GeneticAlgorithm(_brains['predator'].observations_vector_size, _lp.units, _brains['predator'].actions_vector_size, _brains['predator'].agents_count * _lp.number_of_environments, model_name=_lp.NetworkModel.name, use_bias=_lp.use_bias)

    GAs['prey'].initial_population()
    GAs['predator'].initial_population()

    # GAs['prey'].load_population_from_file('genes_9.pkl', 'prey')
    # GAs['predator'].load_population_from_file('genes_9.pkl', 'predator')

    _lp.hotkey_listener.add('<ctrl>+<alt>+a', lambda: save_genes(GAs, _lp.save_ID))

    for generation in range(_lp.number_of_generations):
        models = {}
        models['prey'] = _lp.NetworkModel(_brains['prey'].observations_vector_size, _lp.units, _brains['prey'].actions_vector_size, _brains['prey'].agents_count, batch_size=_lp.number_of_environments, use_bias=_lp.use_bias)
        prey_weights = GAs['prey'].to_model()
        models['prey'].build(prey_weights)

        models['predator'] = _lp.NetworkModel(_brains['predator'].observations_vector_size, _lp.units, _brains['predator'].actions_vector_size, _brains['predator'].agents_count * _lp.number_of_environments, batch_size=1, use_bias=_lp.use_bias)
        predator_weights = GAs['predator'].to_model()
        models['predator'].build(predator_weights)

        all_fitness = unity_environment.run_single_episode(models, _lp.number_of_steps, live_plot=live_plot)

        for brain_name in _lp.brains:
            if brain_name == 'prey':
                fitness = np.mean(all_fitness[brain_name], axis=0)
            else:
                fitness = all_fitness[brain_name]

            GAs[brain_name].calc_fitness(fitness)

            avg, max, min = np.average(fitness), np.max(fitness), np.min(fitness)
            if live_plot:
                live_plot.update({f'{brain_name}1': [avg, max, min], f'{brain_name}2': None})

        GAs['prey'].next_generation()
        GAs['predator'].next_generation()

    # unity_environment.close()
    if live_plot:
        live_plot.close()


if __name__ == "__main__":
    main()
