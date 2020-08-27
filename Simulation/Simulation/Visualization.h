#pragma once
#include <Eigen/Dense>

#include <SFML/Graphics.hpp>

class PreySwarm;
class PredatorSwarm;

class Visualization
{
private:
	sf::RenderWindow* window;
	sf::Event ev;

	PreySwarm* prey_swarm;
	PredatorSwarm* predator_swarm;

	sf::Texture texture;
	sf::Sprite** prey_sprites;
	sf::Sprite** predator_sprites;
	sf::Sprite** food_sprites;

	void initVariables();
	void initWindow();

public:
	Visualization();
	Visualization(PreySwarm &prey_swarm, PredatorSwarm &predator_swarm);
	virtual ~Visualization();

	void pollEvents();
	void updateSwarms();
	void update();

	void renderSwarms();
	void renderView(int id, bool prey);
	void renderHear(int id, bool prey);
	void render();
};