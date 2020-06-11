from configs.environment_parameters import environment_parameters as environment_parameters
from configs.engine_configuration import engine_config as engine_configuration
from other.hotkey_listener import HotKeyListener
from networks.lstm import LSTMModel
from networks.rnn import RNNModel
from networks.dense import MLPModel
from other.utils import save_parameters, current_save_ID


# save_ID = current_save_ID()
save_ID = 0
hotkey_listener = HotKeyListener()
hotkey_listener.add('<ctrl>+<alt>+a', lambda: save_parameters(save_ID))


# ENVIRONMENT PARAMETERS
unity_environment_path = "C:/Users/adek1/Desktop/Env/ArtificalAnimals.exe"
# unity_environment_path = None

brains = [brain_name for brain_name in environment_parameters if ('count' in environment_parameters[brain_name] and environment_parameters[brain_name]['count'] > 0)]

show_plots = True

# GENERAL
NetworkModel = RNNModel

units = 8
use_bias = True

number_of_generations = 500
number_of_steps = 4000

# MULTI
number_of_environments = 8

# GENETIC ALGORITHM PARAMETERS
# Population
population_size = 60

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