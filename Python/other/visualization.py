import arcade
import os
import imageio
import numpy as np

SCALER = 2
SCREEN_WIDTH = 512 * SCALER
SCREEN_HEIGHT = 512 * SCALER
SPRITE_SCALING = 0.3
SCREEN_TITLE = "Simulation - Visualization"
FOOD_ENABLED = False


class Agent(arcade.Sprite):
    """ Player class """

    def __init__(self, image, scale):
        super().__init__(image, scale)

        # Create a variable to hold our speed. 'angle' is created by the parent
        self.alive = 0
        self.angle = 0
        self.center_x = 0
        self.center_y = 0

    def update(self):
        pass

    def update_state(self, state):
        state = state.split(';')
        self.alive = state[0] == '1'
        self.center_x = (float(state[1])+256) * SCALER
        self.center_y = (float(state[2])+256) * SCALER
        self.angle = np.rad2deg(float(state[3]))
        if not self.alive:
            self.center_x = -1000
            self.center_y = -1000


class Visualization(arcade.Window):
    """
    Main application class.
    """

    def __init__(self, width, height, title, vis_file):
        # Call the parent class initializer
        super().__init__(width, height, title)

        self.set_update_rate(0.005)

        file_path = os.path.dirname(os.path.abspath(__file__))
        self.sprite_path = file_path + '\sprite.png'

        self.video_writer = imageio.get_writer(vis_file.replace('txt', 'mp4').replace('vis', 'video'), format='mp4', mode='I', fps=100, quality=10)
        # self.video_writer = imageio.get_writer(vis_file.replace('txt', 'gif').replace('vis', 'video'), format='gif', mode='I', fps=60, quality=10)

        self.simulation_steps = None
        with open(vis_file, 'r') as file:
            self.simulation_steps = file.read().splitlines()

        self.current_step = 0
        self.n_steps = int((len(self.simulation_steps)-1)/2)

        n = self.simulation_steps[0].split('|')
        self.n_preys = int(n[0])
        self.n_predators = int(n[1])
        if FOOD_ENABLED:
            self.n_food = int(n[2])

        # Variables that will hold sprite lists
        self.prey_sprites = None
        self.predator_sprites = None
        self.food_sprites = None

        # Set up the player info
        self.preys = []
        self.predators = []
        self.food = []

        # Set the background color
        arcade.set_background_color(arcade.color.BLACK)

    def setup(self):
        """ Set up the game and initialize the variables. """

        # Sprite lists
        self.prey_sprites = arcade.SpriteList()
        self.predator_sprites = arcade.SpriteList()
        self.food_sprites = arcade.SpriteList()

        # Set up the agents
        for i in range(self.n_preys):
            agent = Agent(self.sprite_path, SPRITE_SCALING)
            agent._set_color(arcade.color.BLUE)
            self.preys.append(agent)
            self.prey_sprites.append(agent)

        for i in range(self.n_predators):
            agent = Agent(self.sprite_path, SPRITE_SCALING)
            agent._set_color(arcade.color.RED)
            self.predators.append(agent)
            self.predator_sprites.append(agent)

        if FOOD_ENABLED:
            for i in range(self.n_food):
                agent = Agent(self.sprite_path, SPRITE_SCALING)
                agent._set_color(arcade.color.GREEN)
                self.food.append(agent)
                self.food_sprites.append(agent)

    def on_draw(self):
        """
        Render the screen.
        """

        # This command has to happen before we start drawing
        arcade.start_render()

        # Draw all the sprites.
        self.prey_sprites.draw()
        self.predator_sprites.draw()

    def on_update(self, delta_time):
        """ Movement and game logic """

        # Call update on all sprites
        if not self.current_step < self.n_steps:
            # self.video_writer.close()
            self.close()
            return

        k = self.current_step * 2
        if FOOD_ENABLED:
            k = self.current_step * 3

        preys_state = self.simulation_steps[k+1].split('|')
        predators_state = self.simulation_steps[k+2].split('|')

        for i in range(self.n_preys):
            self.preys[i].update_state(preys_state[i])

        for i in range(self.n_predators):
            self.predators[i].update_state(predators_state[i])

        if FOOD_ENABLED:
            food_state = self.simulation_steps[k+3].split('|')
            for i in range(self.n_predators):
                self.food[i].update_state(food_state[i])

        # image = arcade.get_image(0, 0)
        # self.video_writer.append_data(np.asanyarray(image))

        self.current_step += 1

    def on_key_press(self, key, modifiers):
        """Called whenever a key is pressed. """
        pass

    def on_key_release(self, key, modifiers):
        """Called when the user releases a key. """
        pass


def create_visualization(vis_file):
    window = Visualization(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE, vis_file)
    window.setup()
    arcade.run()
