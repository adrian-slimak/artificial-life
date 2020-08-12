from learning_parameters import results_save_path
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('TkAgg')
sns.set_style("darkgrid")
plt.rcParams['axes.labelweight'] = 'bold'

path = results_save_path# + 'RNN/mutation/'


def mean_from(prefix=None):
    data_frames = []
    for i in range(5):
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

    leg = ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15), ncol=5, fontsize=13)

    for line in leg.get_lines():
        line.set_linewidth(5.0)

# plot('Średnia liczba prób ataków drapieżnika w kolejnych generacjach',
#      'Generacja', 'Liczba prób ataków',
#     "Predator mean stats - attacks",
#     ['F0', 'W0', 'T0', 'T1', 'T2', 'T3'],
#     ['Fittest Half', 'Roulette Wheel', 'Tournament - 10', 'Tournament - 15', 'Tournament - 20', 'Tournament - 30'])

# plot('Średnia przeżywalność ofiar w kolejnych generacjach',
#      'Generacja', 'Przeżywalność',
#     "Prey mean stats - survivorship",
#     ['F0', 'W0', 'T0', 'T1', 'T2', 'T3'],
#     ['Rank', 'Roulette Wheel', 'Tournament - 10', 'Tournament - 15', 'Tournament - 20', 'Tournament - 30'])

# plot('Średnia przeżywalność ofiar w kolejnych generacjach',
#      'Generacja', 'Przeżywalność',
#     "Prey mean stats - survivorship",
#     ['N', 'SP', 'TP', 'SPpP1', 'TPpP1'],
#     ['No crossover', 'Single Point', 'Two Points', 'Single Point per Part', 'Two Points per Part'])
#
# plot('Średnia liczba prób ataków drapieżnika w kolejnych generacjach',
#      'Generacja', 'Liczba prób ataków',
#     "Predator mean stats - attacks",
#     ['N', 'SP', 'TP', 'SPpP1', 'TPpP1'],
#     ['No crossover', 'Single Point', 'Two Points', 'Single Point per Part', 'Two Points per Part'])

# plot('Średnia przeżywalność ofiar w kolejnych generacjach',
#      'Generacja', 'Przeżywalność',
#     "Prey mean stats - survivorship",
#     ['M1', 'M2', 'M3', 'M4'],
#     ['M1', 'M2', 'D3', 'M4'])

# plot('Średnia przeżywalność ofiar w kolejnych generacjach',
#      'Generacja', 'Przeżywalność',
#     "Prey mean stats - survivorship",
#     ['M1De1', 'M1De2', 'M1De3', 'M1De4', 'M1De5', 'M1De6', 'M1De7', 'M1De8', 'M1De10'],
#     ['M1De1', 'M1De2', 'M1De3', 'M1De4', 'M1De5', 'M1De6', 'M1De7', 'M1De8', 'M1De10'])

# plot('Średnia przeżywalność ofiar w kolejnych generacjach',
#      'Generacja', 'Przeżywalność',
#     "Prey mean stats - survivorship",
#      ['M1F1', 'M1F2', 'M1F3', 'M1F4', 'M1F5', 'M1F6', 'M1F7', 'M1F8', 'M1F9', 'M1F10'],
#      ['M1F1', 'M1F2', 'M1F3', 'M1F4', 'M1F5', 'M1F6', 'M1F7', 'M1F8', 'M1F9', 'M1F10'])

plot('Średnia przeżywalność ofiar w kolejnych generacjach',
     'Generacja', 'Przeżywalność',
    "Prey mean stats - survivorship",
    ['M1D1', 'M1D2', 'M1D3', 'M1D4', 'M1D5', 'M1D6', 'M1D7', 'M1D8', 'M1D10'],
    ['M1D1', 'M1D2', 'M1D3', 'M1D4', 'M1D5', 'M1D6', 'M1D7', 'M1D8', 'M1D10'])

# plot('Średnia przeżywalność ofiar w kolejnych generacjach',
#      'Generacja', 'Przeżywalność',
#      "Prey mean stats - survivorship",
#     ['M1D1De2', 'M1D1De3', 'M1D1De4', 'M1D1De5', 'M1D2De2', 'M1D2De3', 'M1D2De4', 'M1D2De5', 'M1D3De2', 'M1D3De3', 'M1D3De4', 'M1D3De5'],
#     ['M1D1De2', 'M1D1De3', 'M1D1De4', 'M1D1De5', 'M1D2De2', 'M1D2De3', 'M1D2De4', 'M1D2De5', 'M1D3De2', 'M1D3De3', 'M1D3De4', 'M1D3De5'])
#     # ['M1D1De2', 'M1D1De3', 'M1D1De4', 'M1D1De5', 'M1D2De2', 'M1D2De3', 'M1D2De4', 'M1D2De5', 'M1D3De2', 'M1D3De3', 'M1D3De4', 'M1D3De5'],
#     # ['M1D1De2', 'M1D1De3', 'M1D1De4', 'M1D1De5', 'M1D2De2', 'M1D2De3', 'M1D2De4', 'M1D2De5', 'M1D3De2', 'M1D3De3','M1D3De4', 'M1D3De5'])
#
# plot('Średnia przeżywalność ofiar w kolejnych generacjach',
#      'Generacja', 'Przeżywalność',
#      "Prey mean stats - survivorship",
#     ['M1D1F1', 'M1D1F2', 'M1D1F3', 'M1D1F4', 'M1D2F1', 'M1D2F2', 'M1D2F3', 'M1D2F4', 'M1D2F5', 'M1D3F1', 'M1D3F2', 'M1D3F3', 'M1D3F4', 'M1D3F5', 'M1D4F1', 'M1D4F2', 'M1D4F3', 'M1D4F4', 'M1D4F5'],
#     ['M1D1F1', 'M1D1F2', 'M1D1F3', 'M1D1F4', 'M1D2F1', 'M1D2F2', 'M1D2F3', 'M1D2F4', 'M1D2F5', 'M1D3F1', 'M1D3F2', 'M1D3F3', 'M1D3F4', 'M1D3F5', 'M1D4F1', 'M1D4F2', 'M1D4F3', 'M1D4F4', 'M1D4F5'])

    # ['M1D1F1', 'M1D1F2', 'M1D1F3', 'M1D1F4', 'M1D2F1', 'M1D2F2', 'M1D2F3', 'M1D2F4', 'M1D2F5', 'M1D3F1', 'M1D3F2', 'M1D3F3', 'M1D3F4', 'M1D3F5', 'M1D4F1', 'M1D4F2', 'M1D4F3', 'M1D4F4', 'M1D4F5'],
    # ['M1D1F1', 'M1D1F2', 'M1D1F3', 'M1D1F4', 'M1D2F1', 'M1D2F2', 'M1D2F3', 'M1D2F4', 'M1D2F5', 'M1D3F1', 'M1D3F2', 'M1D3F3', 'M1D3F4', 'M1D3F5', 'M1D4F1', 'M1D4F2', 'M1D4F3', 'M1D4F4', 'M1D4F5'])

# plot('Średnia przeżywalność ofiar w kolejnych generacjach',
#      'Generacja', 'Przeżywalność',
#      "Prey mean stats - survivorship",
#     ['M1D2De1F1', 'M1D2De1F2', 'M1D2De1F3'],
#     ['M1D2De1F1', 'M1D2De1F2', 'M1D2De1F3'])

# plt.savefig(f'C:/Users/adek1/Desktop/{"1-1"}.png')
plt.show()