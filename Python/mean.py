from learning_parameters import results_save_path
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('TkAgg')
sns.set_style("darkgrid")
plt.rcParams['axes.labelweight'] = 'bold'

path = results_save_path + 'RNN/selection method/'


def mean_from(prefix=None):
    data_frames = []
    for i in range(10):
        df = pd.read_csv(path + f"{prefix}_{i}_data.csv", sep=';')
        data_frames.append(df)

    mean_df = pd.concat(data_frames).groupby(level=0).mean()
    std_df = pd.concat(data_frames).groupby(level=0).std()

    return mean_df, std_df


def plot(title='', xlabel='', ylabel='', column='', file_prefixes=[], line_labels=[]):
    fig = plt.figure(figsize=(10, 6))
    ax = fig.add_subplot(1, 1, 1)
    ax.locator_params(nbins=15, axis='x')
    ax.locator_params(nbins=15, axis='y')
    ax.set_xlim(0, 1000)
    ax.tick_params(axis='both', labelsize=12)
    plt.title(title, fontsize=16, fontweight='bold')
    plt.xlabel(xlabel, fontsize=14)
    plt.ylabel(ylabel, fontsize=14, fontweight='bold')

    for p, l in zip(file_prefixes, line_labels):
        mean_df, std_df = mean_from(p)
        plt.plot(mean_df[column], label=l)

    fig.tight_layout()

    box = ax.get_position()
    ax.set_position([box.x0, box.y0 + box.height * 0.15, box.width, box.height * 0.85])

    leg = ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15), ncol=3, fontsize=13)

    for line in leg.get_lines():
        line.set_linewidth(5.0)

# plot('Średnia liczba prób ataków drapieżnika w kolejnych generacjach',
#      'Generacja', 'Liczba prób ataków',
#     "Predator mean stats - attacks",
#     ['F0', 'W0', 'T0', 'T1', 'T2', 'T3'],
#     ['Fittest Half', 'Roulette Wheel', 'Tournament - 10', 'Tournament - 15', 'Tournament - 20', 'Tournament - 30'])

plot('Średnia przeżywalność ofiar w kolejnych generacjach',
     'Generacja', 'Przeżywalność',
    "Prey mean stats - survivorship",
    ['F0', 'W0', 'T0', 'T1', 'T2', 'T3'],
    ['Rank', 'Roulette Wheel', 'Tournament - 10', 'Tournament - 15', 'Tournament - 20', 'Tournament - 30'])

plt.savefig(f'C:/Users/adek1/Desktop/{"1-1"}.png')
plt.show()