from multiprocessing import Process, Queue
from collections.abc import Iterable
import matplotlib.pyplot as plt
from other.hotkey_listener import HotKeyListener
from pickle import dump
import matplotlib
from configs.learning_parameters import save_ID
plt.style.use('ggplot')
matplotlib.use('tkagg')


class PlotPart:
    def __init__(self, name, params, ax):
        self.name = name
        self._ax = ax
        self.title = params['title']
        self.xlabel = params['labels'][0]
        self.ylabel = params['labels'][1]

        self._ax.set_title(self.title)
        self._ax.set_xlabel(self.xlabel)
        self._ax.set_ylabel(self.ylabel)
        self._ax.set_xlim(0, params['lims'][0])
        self._ax.set_ylim(0, params['lims'][1])

        self.X = []
        self.Y = {}

        colors = ['g', 'r', 'b', 'm', 'y']
        for line_name, color in zip(params['lines'], colors):
            self._ax.plot([], [], color, label=line_name)
            self.Y[line_name] = []

        self._ax.legend()

    def update(self, data):
        if data is None:
            self.X = []
            for y_key in self.Y.keys():
                self.Y[y_key] = []
            return

        self.X = list(range(len(self.X) + 1))
        for line, Ys, v in zip(self._ax.lines, self.Y.values(), data):
            Ys.append(v)
            line.set_xdata(self.X)
            line.set_ydata(Ys)


class LivePlot:
    def __init__(self, plots=None, subplots=(1, 1), figsize=(10, 8)):
        self.fig, self.ax = plt.subplots(subplots[0], subplots[1], figsize=figsize)
        self.plots = {}

        if not isinstance(self.ax, Iterable) or not isinstance(self.ax[0], Iterable):
            self.ax = [self.ax]

        for plot_name, params in plots.items():
            pos = params['position']
            self.plots[plot_name] = PlotPart(plot_name, params, self.ax[pos[0]][pos[1]])

        plt.tight_layout()
        self._start_process()

    def _start_process(self):
        self.plot_queue = Queue()
        self.plot_process = Process(target=self, daemon=True)
        self.plot_process.start()

    def __call__(self):
        HotKeyListener().add('<ctrl>+<alt>+a', self.save)
        timer = self.fig.canvas.new_timer(interval=250)
        timer.add_callback(self._call_back)
        timer.start()
        plt.show()

    def _call_back(self):
        while not self.plot_queue.empty():
            data = self.plot_queue.get()
            if data is None:  # Terminate
                plt.close('all')
                return False
            else:
                self._update_data(data)
        self.fig.canvas.draw()
        return True

    def _update_data(self, data):
        for data_key in data.keys():
            self.plots[data_key].update(data[data_key])
        # self.fig.canvas.flush_events()

    def save(self):
        with open(f'results/data/data_{save_ID}.pkl', 'wb') as file:
            to_save = {}
            for plot_name, plot in self.plots.items():
                to_save[plot_name] = {'Y': plot.Y, 'title': plot.title, 'xlabel': plot.xlabel, 'ylabel': plot.ylabel}
            dump(to_save, file)

        plt.savefig(f'results/plots/plot_{save_ID}.png')

    def update(self, data):
        self.plot_queue.put(data)

    def close(self):
        self.plot_queue.put(None)
