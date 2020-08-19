config_file_path = r"E:/Repos/ArtificalLife/config.json"
results_save_path = r"E:/Repos/ArtificalLife/Results/"
learning_file_path = r"E:/Repos/ArtificalLife/Python/learning_parameters.py"

import json
with open(config_file_path, 'r') as f:
    config = json.loads(f.read())

# ENVIRONMENT PARAMETERS
show_plots = True

number_of_generations = 1000

prey_brain_cells = config['prey']['brain_cells']
predator_brain_cells = config['predator']['brain_cells']

prey_network = config['prey']['brain_type'].lower()
predator_network = config['predator']['brain_type'].lower()

prey_observations_size = config["prey"]["vision"]["cells"] * (3 if config["environment"]["food"]["enabled"] else 2)
predator_observations_size = config["predator"]["vision"]["cells"] * (3 if config["environment"]["food"]["enabled"] else 2)
prey_actions_size = 2
predator_actions_size = 3 if config["predator"]["communication"]["enabled"] else 2

# GENETIC ALGORITHM PARAMETERS
# Population
population_size = 100

# Random Init
init_genes = 1.
init_loc = 0.
init_scale = 1.

# Selection
selection_method = 'Fittest Half'
# selection_method = 'Roulette Wheel'
# selection_method = 'Tournament'
tournament_size = 30
elite_size = 0

# Pairing
pairing_method = 'Fittest'

# Crossover
mating_method = 'None'
# mating_method = 'Single Point'
# mating_method = 'Two Points'
# mating_method = 'Single Point Per Part'
# mating_method = 'Two Points Per Part'
crossover_probability = 0.5

# Mutation
gen_mutation_chance = 0.01
gen_deletion_chance = 0.0
duplication_chance = 0.0
deletion_chance = 0.0
fill_chance = 0.0
