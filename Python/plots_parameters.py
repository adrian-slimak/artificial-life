from learning_parameters import number_of_generations

plot_size = (14, 6)  # Width / Height

plot_subplots = (2, 3)  # Rows / Columns

plot_structure = \
    {
        'prey1':
            {
                'title': 'Prey fitness',
                'position': (0, 0),
                'labels': ['episode', 'fitness'],
                'lines': ['avg', 'best', 'worst'],
                'lims': [number_of_generations, 110000],
            },

        'prey2':
            {
                'title': 'Prey mean stats',
                'position': (0, 1),
                'labels': ['step', 'value'],
                'lines': ['survivorship', 'dispersion'],
                'lims': [number_of_generations, 50],
            },

        'prey3':
            {
                'title': 'Prey mean stats',
                'position': (0, 2),
                'labels': ['step', 'value'],
                'lines': ['density', 'food'],
                'lims': [number_of_generations, 20],
            },

        'predator1':
            {
                'title': 'Predator fitness',
                'position': (1, 0),
                'labels': ['episode', 'fitness'],
                'lines': ['avg', 'best', 'worst'],
                'lims': [number_of_generations, 110000]
             },

        'predator2':
            {
                'title': 'Predator mean stats',
                'position': (1, 1),
                'labels': ['step', 'value'],
                'lines': ['density', 'dispersion',  'attacks', 'hunts'],
                'lims': [number_of_generations, 25],
            },
    }