import EnvManager
from time import time
import numpy

prey_genes = numpy.arange(1554*100).reshape(100, 1554).astype(numpy.float32)
predator_genes = numpy.arange(1554*100).reshape(100, 1554).astype(numpy.float32)
prey_fitness = numpy.zeros(100).astype(numpy.float32)
predator_fitness = numpy.zeros(100).astype(numpy.float32)
prey_stats = numpy.zeros(5).astype(numpy.float32)
predator_stats = numpy.zeros(5).astype(numpy.float32)

prey_genes_pointer = float(prey_genes.__array_interface__['data'][0])
predator_genes_pointer = float(predator_genes.__array_interface__['data'][0])
prey_fitness_pointer = float(prey_fitness.__array_interface__['data'][0])
predator_fitness_pointer = float(predator_fitness.__array_interface__['data'][0])
prey_stats_pointer = float(prey_stats.__array_interface__['data'][0])
predator_stats_pointer = float(predator_stats.__array_interface__['data'][0])

EnvManager.set_parameters(r"C:\Users\adek1\source\repos\ArtificalLife\config.json")

EM = EnvManager.EnvManager()
EM.set_fitness_pointers(prey_fitness_pointer, predator_fitness_pointer)
EM.set_stats_pointers(prey_stats_pointer, predator_stats_pointer)

t = time()
for i in range(25):
    EM.set_prey_genes(prey_genes_pointer, prey_genes.shape[0], prey_genes.shape[1])
    EM.set_predator_genes(predator_genes_pointer, predator_genes.shape[0], predator_genes.shape[1])
    EM.run_single_episode()

print((time()-t)/25.)
