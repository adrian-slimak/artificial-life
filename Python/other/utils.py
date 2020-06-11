from pickle import dump
from os import listdir
import re


def merge_environment_parameters(nested_dictionary):
    new_dict = {}

    def merge_dict(old_dict, new_dict, key_name):
        for key, item in old_dict.items():
            if isinstance(item, dict):
                merge_dict(item, new_dict, key if key_name is None else f'{key_name}_{key}')
            else:
                new_dict[key_name if key == '' else f'{key_name}_{key}'] = item

    merge_dict(nested_dictionary, new_dict, None)

    for brain in nested_dictionary.keys():
        observations_vector_size = 0
        for k, v in new_dict.items():
            if re.match(f'{brain}_observations_\w+_vector_size', k):
                observations_vector_size += v
        new_dict[f'{brain}_observations_vector_size'] = observations_vector_size

    return new_dict


def current_save_ID():
    _id = 0
    ids = [int(re.findall('\d+', i)[0]) for i in listdir('results/plots')]
    if len(ids) > 0:
        _id = max(ids) + 1
    return _id


def save_parameters(id):
    lines = [f'__LEARNING_PARAMETERS__\n']
    with open('configs/learning_parameters.py', 'r') as f:
        lines.extend([l for l in f.readlines()][16:])

    lines.append(f'\n__ENVIRONMENT_PARAMETERS__\n')
    with open('configs/environment_parameters.py', 'r') as f:
        lines.extend([l for l in f.readlines()])

    with open(f'results/configs/parameters_{id}.txt', 'w') as f:
        f.writelines(lines)


def save_weights(models, _id):
    weights = {}
    for model_name, model in models.items():
        weights[model_name] = model.to_model()

    with open(f'results/models/weights_{_id}.pkl', 'wb') as file:
        dump(weights, file)

def save_genes(models, _id):
    genes = {}
    for model_name, model in models.items():
        temp = [obj.genotype for obj in model.population]
        genes[model_name] = temp

    with open(f'results/models/genes_{_id}.pkl', 'wb') as file:
        dump(genes, file)