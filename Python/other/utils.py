from pickle import dump
from os import listdir
import re

results_save_path = r"C:/Users/adek1/source/repos/ArtificalLife/Results/"
config_file_path = r"C:/Users/adek1/source/repos/ArtificalLife/config.json"


def current_save_ID():
    _id = 0
    ids = [int(re.findall('\d+', i)[0]) for i in listdir(results_save_path)]
    if len(ids) > 0:
        _id = max(ids) + 1
    return _id


def save_parameters(id):
    with open(config_file_path, 'r') as conf_file:
        with open(results_save_path+f'configs/parameters_{id}.json', 'w') as f:
            f.writelines(conf_file.readlines())


def save_genes(models, _id):
    genes = {}
    for model_name, model in models.items():
        temp = [obj.genotype for obj in model.population]
        genes[model_name] = temp

    with open(results_save_path+f'models/genes_{_id}.pkl', 'wb') as file:
        dump(genes, file)