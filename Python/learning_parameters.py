config_file_path = r"C:/Users/adek1/source/repos/ArtificalLife/config.json"
results_save_path = r"C:/Users/adek1/source/repos/ArtificalLife/Results/"
learning_file_path = r"C:/Users/adek1/source/repos/ArtificalLife/Python/learning_parameters.py"

import json
with open(config_file_path, 'r') as f:
    config = json.loads(f.read())

# ENVIRONMENT PARAMETERS
show_plots = True

number_of_generations = 1

prey_brain_cells = config['prey']['brain_cells']
predator_brain_cells = config['predator']['brain_cells']

network_type = config['prey']['brain_type'].lower()

prey_observations_size = config["prey"]["vision"]["cells"] * 3 if config["environment"]["food"]["enabled"] else 2
predator_observations_size = config["predator"]["vision"]["cells"] * 3 if config["environment"]["food"]["enabled"] else 2
prey_actions_size = 2
predator_actions_size = 3 if config["predator"]["communication"]["enabled"] else 2

# GENETIC ALGORITHM PARAMETERS
# Population
population_size = 100

# Random Init
init_min_genes = 0.7
init_max_genes = 0.9
init_loc = 0.
init_scale = 2.

# Selection
selection_method = 'Roulette Wheel'
# selection_method = 'Fittest Half'
# selection_method = 'Tournament'

# Crossover
# mating_method = 'None'
# mating_method = 'Two Points Per Part'
mating_method = 'Two Points'

# Pairing
pairing_method = 'Fittest'
# pairing_method = 'Random'

# Mutation
gen_mutation_chance = 0.01
gen_duplication_chance = 0.05
gen_deletion_chance = 0.02
