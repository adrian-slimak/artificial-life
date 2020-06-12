import learning_parameters as _lp
from pickle import load
import numpy as np
import random


def get_shapes_lengths(input_dim, units, output_dim, model_name='lstm', use_bias=True):
    shapes, lengths = None, None

    k = 4 if model_name == 'lstm' else 1

    if model_name == 'lstm' or model_name == 'rnn':
        if use_bias:
            shapes = [[(input_dim, units * k), (units, units * k), (units, output_dim)], [(1, units * k), (1, output_dim)]]
            lengths = [input_dim * units * k, units * units * k, units * output_dim, units * k, output_dim]
        else:
            shapes = [[(input_dim, units * k), (units, units * k), (units, output_dim)]]
            lengths = [input_dim * units * k, units * units * k, units * output_dim]

    if model_name == 'mlp':
        if use_bias:
            shapes = [[(input_dim, units), (units, output_dim)], [(1, units), (1, output_dim)]]
            lengths = [input_dim * units, units * output_dim, units, output_dim]
        else:
            shapes = [[(input_dim, units), (units, output_dim)]]
            lengths = [input_dim * units, units * output_dim]

    return shapes, lengths

class Genotype:
    def __init__(self, lengths):
        self.lengths = lengths
        self.genotype = np.zeros((sum(self.lengths)), dtype=np.float32)
        self.length = len(self.genotype)
        self.fitness = None

    def copy(self):
        new_one = type(self)(self.lengths)
        new_one.genotype = self.genotype.copy()
        return new_one

    def random_init(self, min=_lp.init_min_genes, max=_lp.init_max_genes, loc=_lp.init_loc, scale=_lp.init_scale):
        percentOfGenes = random.uniform(min, max)
        numOfGenes = int(self.length * percentOfGenes)
        genesIdx = random.sample(range(0, self.length - 1), numOfGenes)
        genesVals = np.random.normal(loc=loc, scale=scale, size=self.length)
        for id in genesIdx:
            self.genotype[id] = genesVals[id]


class GeneticAlgorithm:
    def __init__(self, input_dim, units, output_dim, population_size, model_name='lstm', use_bias=True):
        self.population = []

        self.shapes, self.lengths = get_shapes_lengths(input_dim, units, output_dim, model_name, use_bias)

        self.population_size = population_size
        self.use_bias = use_bias

        self.model_name = model_name

    def initial_population(self):
        self.population = [Genotype(self.lengths) for i in range(self.population_size)]
        for individual in self.population:
            individual.random_init()

    def load_population_from_file(self, file_name, brain_name='predator'):
        self.population = [Genotype(self.lengths) for i in range(self.population_size)]

        with open(f'results/models/{file_name}', 'rb') as f:
            models = load(f)[brain_name]
            for indv, model in zip(self.population, models):
                indv.genotype = model

    def calc_fitness(self, fitness):
        for idx, value in enumerate(fitness):
            self.population[idx].fitness = value

        # Sort population by fitness, best to worst order
        self.population = sorted(self.population, key=lambda individual: individual.fitness, reverse=True)

    def next_generation(self):
        selected_individuals = GeneticAlgorithm.selection(self.population, method=_lp.selection_method)

        parents = GeneticAlgorithm.pairing(selected_individuals)

        offsprings = [GeneticAlgorithm.mating(parents[x]) for x in range(len(parents))]
        offsprings = [individual for sublist in offsprings for individual in sublist]

        elite_individuals = self.population[:2]
        selected_individuals = selected_individuals[2:]

        next_gen = selected_individuals + offsprings
        for individual in next_gen:
            GeneticAlgorithm.mutation(individual)
        next_gen.extend(elite_individuals)

        if len(next_gen) != self.population_size:
            raise Exception("Next Gen size different than expected")

        self.population = next_gen

    @staticmethod
    def selection(population, method='Fittest Half'):
        if method == 'Fittest Half':
            selected_individuals = [population[i] for i in range(len(population) // 2)]
            return selected_individuals

        elif method == 'Roulette Wheel':
            fitness = [indiv.fitness for indiv in population]
            selected_individuals = random.choices(population, weights=fitness, k=len(population)//2)
            return selected_individuals

        elif method == 'Tournament':
            selected_individuals = []
            for i in range(len(population)//2):
                selected = random.choices(population, k=int(len(population)//3.5))
                selected = max(selected, key=lambda indi: indi.fitness)
                selected_individuals.append(selected)
            return selected_individuals

        else:
            raise Exception('Not such selection method found')

    @staticmethod
    def pairing(selected, method=_lp.pairing_method):
        individuals = selected
        parents = []

        if method == 'Fittest':
            parents = [[individuals[x], individuals[x + 1]] for x in range(len(individuals) // 2)]

        return parents

    @staticmethod
    def mating(parents, max_percent_length=_lp.max_percent_length, method=_lp.mating_method):
        offsprings = [parents[0].copy(), parents[1].copy()]

        if method == 'Two Points Per Part':
            partStart = 0
            for partLength in parents[0].lengths:
                l = int(random.uniform(0, max_percent_length) * (partLength-1))
                s = random.randrange(partStart, partStart + partLength)
                if s+l>=len(parents[0].genotype):
                    l-=(s+l)-len(parents[0].genotype)
                offsprings[0].genotype[s:s+l] = parents[1].genotype[s:s+l].copy()
                offsprings[1].genotype[s:s+l] = parents[0].genotype[s:s+l].copy()
                partStart += partLength

        elif method == 'Two Points':
            genotype_length = len(parents[0].genotype)
            l = int(random.uniform(0, max_percent_length) * genotype_length)
            s = random.randrange(0, genotype_length)
            if s + l >= len(parents[0].genotype):
                l -= (s+l)-len(parents[0].genotype)
            offsprings[0].genotype[s:s+l] = parents[1].genotype[s:s+l]
            offsprings[1].genotype[s:s+l] = parents[0].genotype[s:s+l]

        else:
            raise Exception('Not such mating method found')

        return offsprings

    @staticmethod
    def mutation(individual, gen_mutation_chance=_lp.gen_mutation_chance, gen_deletion_chance=_lp.gen_deletion_chance, gen_duplication_chance=_lp.gen_duplication_chance):
        for gen_id in range(individual.length):
            if individual.genotype[gen_id] == 0.:
                if random.random() <= gen_duplication_chance:
                    individual.genotype[gen_id] = random.gauss(mu=0., sigma=_lp.init_scale)
            else:
                if random.random() <= gen_deletion_chance:
                    individual.genotype[gen_id] = 0.
                if random.random() <= gen_mutation_chance:
                    individual.genotype[gen_id] = random.gauss(mu=_lp.init_loc, sigma=_lp.init_scale)

        # if random.random() <= gen_duplication_chance:
        #     i1 = random.randint(0, individual.length - 2)
        #     i2 = random.randint(i1 + 1, individual.length - 1)
        #     i0 = random.randint(0, individual.length - 1)
        #     individual.genotype[i0:i0 + (i2-i1) + 1] = (individual.genotype[i1:i2 + 1])[0:individual.length - i0]
        #
        # if random.random() <= gen_deletion_chance:
        #     i1 = random.randint(0, individual.length - 2)
        #     i2 = random.randint(i1 + 1, individual.length - 1)
        #     individual.genotype[i1:i1+(individual.length-i2-1)] = individual.genotype[i2+1:]
        #     individual.genotype[-(i2 - i1 + 1):] = np.random.normal(loc=_lp.init_loc, scale=_lp.init_scale, size=i2-i1+1)

    def to_genes(self):
        genes = []

        for individual in self.population:
            genes.append(individual.genotype)

        genes = np.array(genes)

        return genes
