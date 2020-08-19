config_file_path = r"C:/Users/adek1/source/repos/ArtificalLife/config.json"
results_save_path = r"C:/Users/adek1/source/repos/ArtificalLife/Results/"
learning_file_path = r"C:/Users/adek1/source/repos/ArtificalLife/Python/learning_parameters.py"

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

if config["prey"]["communication"]["enabled"]:
    prey_observations_size += config["prey"]["communication"]["hear_cells"] * 2

print(f'prey {prey_observations_size}')

if config["predator"]["communication"]["enabled"]:
    predator_observations_size += config["predator"]["communication"]["hear_cells"]

print(f'predator {predator_observations_size}')
print(f'predator {predator_actions_size}')

# GENETIC ALGORITHM PARAMETERS
# Population
population_size = 100

# Random Init
init_min_genes = 0.98
init_max_genes = 0.99
init_loc = 0.
init_scale = 1.

# Selection
selection_method = 'Fittest Half'
# selection_method = 'Roulette Wheel'
# selection_method = 'Tournament'
tournament_size = 30

# Pairing
pairing_method = 'Fittest'

# Crossover
mating_method = 'None'
# mating_method = 'Single Point'
# mating_method = 'Two Points'
# mating_method = 'Single Point Per Part'
# mating_method = 'Two Points Per Part'

# Mutation
gen_mutation_chance = 0.01
gen_deletion_chance = 0.0
duplication_chance = 0.02
deletion_chance = 0.02
fill_chance = 0.0
