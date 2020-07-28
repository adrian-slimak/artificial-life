from learning_parameters import results_save_path
import pandas as pd
import matplotlib.pyplot as plt
path = results_save_path

prefix = 'F0'
i = 0
data_frames = []
for i in range(10):
    df = pd.read_csv(f"{path}{prefix}_{i}_data.csv", sep=';')
    data_frames.append(df)

mean_df = pd.concat(data_frames).groupby(level=0).mean()
std_df = pd.concat(data_frames).groupby(level=0).std()

mean_df.to_csv(path + f'{prefix}_mean.csv', encoding='utf-8', index=False, sep=';')
std_df.to_csv(path + f'{prefix}_std.csv', encoding='utf-8', index=False, sep=';')