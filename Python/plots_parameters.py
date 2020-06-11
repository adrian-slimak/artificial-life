from configs.learning_parameters import number_of_steps, number_of_generations

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
                'lims': [number_of_generations, 200],
            },

        'prey3':
            {
                'title': 'Prey mean stats',
                'position': (0, 1),
                'labels': ['step', 'value'],
                'lines': ['survivorship', 'density', 'dispersion', 'food'],
                'lims': [number_of_generations, 60],
            },

        'prey2':
            {
                'title': 'Prey live stats',
                'position': (0, 2),
                'labels': ['step', 'value'],
                'lines': ['alive', 'density', 'dispersion', 'food'],
                'lims': [number_of_steps, 30],
            },

        'predator1':
            {
                'title': 'Predator fitness',
                'position': (1, 0),
                'labels': ['episode', 'fitness'],
                'lines': ['avg', 'best', 'worst'],
                'lims': [number_of_generations, 40000]
             },

        'predator3':
            {
                'title': 'Predator mean stats',
                'position': (1, 1),
                'labels': ['step', 'value'],
                'lines': ['survivorship', 'density', 'dispersion', 'food', 'attacks'],
                'lims': [number_of_generations, 30],
            },

        'predator2':
            {
                'title': 'Predator live stats',
                'position': (1, 2),
                'labels': ['step', 'value'],
                'lines': ['alive', 'density', 'dispersion', 'food', 'attacks'],
                'lims': [number_of_steps, 20],
            },

    }