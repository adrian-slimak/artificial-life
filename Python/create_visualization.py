from other.visualization import create_visualization
import learning_parameters as _lp
import EnvManager
from pickle import load
import numpy as np
# import glob

genes_file = _lp.results_save_path+'RNN_16_NC_1_genes.pkl'
prey_genes_file = genes_file
predator_genes_file = genes_file

# prey_genes_file = _lp.results_save_path+'RNN/E1/RNN_8_NC_3_genes.pkl'
# predator_genes_file = _lp.results_save_path+'test1_0_genes.pkl'

vis_file = _lp.results_save_path+'vis_0.txt'

EnvManager.set_parameters(_lp.config_file_path)

env_manager = EnvManager.EnvManager()

with open(prey_genes_file, 'rb') as f:
    prey_genes = np.array(load(f)['prey'][0]).astype(np.float32)

with open(predator_genes_file, 'rb') as f:
    predator_genes = np.array(load(f)['predator'][0]).astype(np.float32)

prey_genes_pointer = float(prey_genes.__array_interface__['data'][0])
predator_genes_pointer = float(predator_genes.__array_interface__['data'][0])

env_manager.set_prey_genes(prey_genes_pointer, 1, prey_genes.shape[0])
env_manager.set_predator_genes(predator_genes_pointer, 1, predator_genes.shape[0])
env_manager.create_visualization(vis_file)

create_visualization(vis_file)

# vis_files = glob.glob(_lp.results_save_path+'/vis*.txt')
# for vis_file in vis_files:
#     create_visualization(vis_file)