from other.hotkey_listener import HotKeyListener
from other.utils import save_parameters, current_save_ID

config_file_path = r"C:/Users/adek1/source/repos/ArtificalLife/config.json"
results_save_path = r"C:/Users/adek1/source/repos/ArtificalLife/Results/"

save_ID = current_save_ID()
save_ID = 0
hotkey_listener = HotKeyListener()
hotkey_listener.add('<ctrl>+<alt>+a', lambda: save_parameters(save_ID))

# ENVIRONMENT PARAMETERS
show_plots = True

number_of_generations = 500
number_of_steps = 4000

prey_brain_cells = 8
predator_brain_cells = 8

prey_observations_size = 26
predator_observations_size = 26
prey_actions_size = 2
predator_actions_size = 2

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
mating_method = 'Two Points Per Part'
# mating_method = 'Two Points'
max_percent_length = 0.5

# Pairing
pairing_method = 'Fittest'

# Mutation
gen_mutation_chance = 0.02
gen_deletion_chance = 0.01
gen_duplication_chance = 0.03