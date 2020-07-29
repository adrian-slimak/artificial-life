from learning_parameters import results_save_path
import pandas as pd
import seaborn as sns
sns.set_style("whitegrid")
import matplotlib.pyplot as plt
path = results_save_path + 'RNN/selection method/'


def mean_from(prefix=None):
    data_frames = []
    for i in range(10):
        df = pd.read_csv(path + f"{prefix}_{i}_data.csv", sep=';')
        data_frames.append(df)

    mean_df = pd.concat(data_frames).groupby(level=0).mean()
    std_df = pd.concat(data_frames).groupby(level=0).std()

    return mean_df, std_df

prefix = ['F0', 'W0', 'T0', 'T1', 'T2', 'T3']

for p in prefix:
    mean_df, std_df = mean_from(p)
    sns.lineplot(data=mean_df["Predator mean stats - hunts"])

plt.show()

# mean_df.to_csv(path + f'{prefix}_mean.csv', encoding='utf-8', index=False, sep=';')
# std_df.to_csv(path + f'{prefix}_std.csv', encoding='utf-8', index=False, sep=';')