#include "Visualization.h"

#include "PreySwarm.h"
#include "PredatorSwarm.h"
#include "LSTM.h"

#include <iostream>


void Visualization::initVariables()
{
	this->window = nullptr;

	texture.loadFromFile("sprites/sprite.png");

	this->prey_sprites = new sf::Sprite*[prey_swarm->population_size];

	for (int i = 0; i < prey_swarm->population_size; i++)
	{
		this->prey_sprites[i] = new sf::Sprite();
		this->prey_sprites[i]->setTexture(texture);
		this->prey_sprites[i]->setColor(sf::Color::Blue);

		this->prey_sprites[i]->setOrigin(sf::Vector2f(25.f, 20.f));
		this->prey_sprites[i]->setScale(0.2f, 0.2f);
	}

	this->predator_sprites = new sf::Sprite*[predator_swarm->population_size];

	for (int i = 0; i < predator_swarm->population_size; i++)
	{
		this->predator_sprites[i] = new sf::Sprite();
		this->predator_sprites[i]->setTexture(texture);
		this->predator_sprites[i]->setColor(sf::Color::Red);

		this->predator_sprites[i]->setOrigin(sf::Vector2f(25.f, 20.f));
		this->predator_sprites[i]->setScale(0.2f, 0.2f);
	}

	if (PreySwarm::food_enabled)
	{
		this->food_sprites = new sf::Sprite*[PreySwarm::food_amount];

		for (int i = 0; i < PreySwarm::food_amount; i++)
		{
			this->food_sprites[i] = new sf::Sprite();
			this->food_sprites[i]->setTexture(texture);
			this->food_sprites[i]->setColor(sf::Color::Green);

			this->food_sprites[i]->setOrigin(sf::Vector2f(25.f, 20.f));
			this->food_sprites[i]->setScale(0.2f, 0.2f);
		}
	}
}

void Visualization::initWindow()
{
	this->window = new sf::RenderWindow(sf::VideoMode(512, 512), "Artifical Life", sf::Style::Titlebar | sf::Style::Close);
	this->window->setFramerateLimit(30);
}

Visualization::Visualization()
{
	this->initVariables();
	this->initWindow();
}

Visualization::Visualization(PreySwarm &prey_swarm, PredatorSwarm &predator_swarm)
{
	this->prey_swarm = &prey_swarm;
	this->predator_swarm = &predator_swarm;
	this->initVariables();
	this->initWindow();
}

Visualization::~Visualization()
{
	delete this->window;
}

void Visualization::pollEvents()
{
	while (this->window->pollEvent(this->ev))
	{
		switch (this->ev.type)
		{
		case sf::Event::Closed:
			this->window->close();
			break;
		case sf::Event::KeyPressed:
			if (this->ev.key.code == sf::Keyboard::Escape)
				this->window->close();
			break;
		}
	}
}

void Visualization::updateSwarms()
{
	int me = 0;
	this->prey_swarm->model->y(me, 0) = 0;
	this->prey_swarm->model->y(me, 1) = 0;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		this->prey_swarm->model->y(me, 1) = -1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		this->prey_swarm->model->y(me, 1) = 1;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		this->prey_swarm->model->y(me, 0) = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		this->prey_swarm->model->y(me, 0) = -1;

	float x = 0;
	float y = 0;
	float rad2deg = 180.f / 3.14f;
	for (int i = 0; i < prey_swarm->population_size; i++)
	{
		x = this->prey_swarm->position(i, 0) + 256.f;
		y = this->prey_swarm->position(i, 1) + 256.f;
		this->prey_sprites[i]->setPosition(x, y);
		this->prey_sprites[i]->setRotation(this->prey_swarm->angle[i] * rad2deg);
	}

	for (int i = 0; i < predator_swarm->population_size; i++)
	{
		x = this->predator_swarm->position(i, 0) + 256.f;
		y = this->predator_swarm->position(i, 1) + 256.f;
		this->predator_sprites[i]->setPosition(x, y);
		this->predator_sprites[i]->setRotation(this->predator_swarm->angle[i] * rad2deg);
	}

	if (PreySwarm::food_enabled)
	{
		for (int i = 0; i < PreySwarm::food_amount; i++)
		{
			x = this->prey_swarm->food_position(i, 0) + 256.f;
			y = this->prey_swarm->food_position(i, 1) + 256.f;
			this->food_sprites[i]->setPosition(x, y);
			this->food_sprites[i]->setRotation(0.f);
		}
	}
}

void Visualization::update()
{
	this->pollEvents();

	this->updateSwarms();
}

void Visualization::renderSwarms()
{
	//for (int i = 0; i < prey_swarm->population_size; i++)
	//	if (prey_swarm->alive[i])
	//		renderView(i, true);

	//for (int i = 0; i < predator_swarm->population_size; i++)
	//	if (predator_swarm->alive[i])
		renderHear(0, true);
		//renderView(0, true);

	for (int i = 0; i < prey_swarm->population_size; i++)
		if (prey_swarm->alive[i])
			this->window->draw(*prey_sprites[i]);

	for (int i = 0; i < predator_swarm->population_size; i++)
			this->window->draw(*predator_sprites[i]);

	if (PreySwarm::food_enabled)
	{
		for (int i = 0; i < PreySwarm::food_amount; i++)
			if (prey_swarm->food_alive[i])
				this->window->draw(*food_sprites[i]);
	}
}

void Visualization::renderView(int id, bool prey)
{
	sf::Color blue = sf::Color::Blue;
	blue.a = 128;
	sf::Color red = sf::Color::Red;
	red.a = 128;
	sf::Color green = sf::Color::Green;
	green.a = 128;
	sf::Vertex line[] = {sf::Vertex(sf::Vector2f(10, 10)), sf::Vertex(sf::Vector2f(150, 150))};
	sf::ConvexShape convex;
	convex.setPointCount(3);
	convex.setFillColor(sf::Color(128,128,128,128));

	float a_rad = 0.f;
	float x = 0.f;
	float y = 0.f;
	float arc_start = 0.f;
	float range = 0.f;

	if (prey)
	{
		x = prey_swarm->position(id, 0) + 256.f;
		y = prey_swarm->position(id, 1) + 256.f;
		arc_start = (prey_swarm->angle[id] - PreySwarm::vision_angle_half_rad);
		range = PreySwarm::vision_range;
	}
	else
	{
		x = predator_swarm->position(id, 0) + 256.f;
		y = predator_swarm->position(id, 1) + 256.f;
		arc_start = (predator_swarm->angle[id] - PredatorSwarm::vision_angle_half_rad);
		range = PredatorSwarm::vision_range;
	}

	sf::Vector2f pt0(x,y);
	sf::Vector2f pt1;
	sf::Vector2f pt2;

	convex.setPoint(0, sf::Vector2f(x, y));

	for (int i = 0; i < 13; i++)
	{
		convex.setFillColor(sf::Color(128, 128, 128, 128));

		if (prey)
		{
			if (prey_swarm->model->x(id, i) > 0.f)
				convex.setFillColor(blue);
			if (prey_swarm->model->x(id, 13 + i) > 0.f)
				convex.setFillColor(red);
			if (prey_swarm->model->x(id, 26 + i) > 0.f)
				convex.setFillColor(green);
		}
		else
		{
			if (predator_swarm->model->x(id, i) > 0.f)
				convex.setFillColor(blue);
			if (predator_swarm->model->x(id, 13+i) > 0.f)
				convex.setFillColor(red);
		}

		a_rad = arc_start + i * PreySwarm::vision_cell_angle_rad;
		pt1 = sf::Vector2f(x + std::cos(a_rad) * range, y + std::sin(a_rad) * range);
		a_rad = arc_start + (i + 1) * PreySwarm::vision_cell_angle_rad;
		pt2 = sf::Vector2f(x + std::cos(a_rad) * range, y + std::sin(a_rad) * range);

		convex.setPoint(1, pt1);
		convex.setPoint(2, pt2);
		this->window->draw(convex);

		line[0].position = pt1;
		line[1].position = pt2;
		this->window->draw(line, 2, sf::Lines);
		line[0].position = pt0;
		this->window->draw(line, 2, sf::Lines);
	}

	line[0].position = pt0;
	line[1].position = sf::Vector2f(x + std::cos(arc_start) * range, y + std::sin(arc_start) * range);
	this->window->draw(line, 2, sf::Lines);
}

void Visualization::renderHear(int id, bool prey)
{
	sf::Color yellow = sf::Color::Yellow;
	yellow.a = 128;
	sf::Color magenta = sf::Color::Magenta;
	magenta.a = 128;
	sf::Vertex line[] = { sf::Vertex(sf::Vector2f(10, 10)), sf::Vertex(sf::Vector2f(150, 150)) };
	sf::ConvexShape convex;
	convex.setPointCount(3);
	convex.setFillColor(sf::Color(128, 128, 128, 128));

	float a_rad = 0.f;
	float x = 0.f;
	float y = 0.f;
	float arc_start = 0.f;
	float range = 0.f;

	if (prey)
	{
		x = prey_swarm->position(id, 0) + 256;
		y = prey_swarm->position(id, 1) + 256;
		arc_start = (prey_swarm->angle[id] - 3.14f);
		range = PreySwarm::vision_range;
	}
	else
	{
		x = predator_swarm->position(id, 0) + 256;
		y = predator_swarm->position(id, 1) + 256;
		arc_start = (predator_swarm->angle[id] - 3.14f);
		range = PredatorSwarm::vision_range;
	}

	sf::Vector2f pt0(x, y);
	sf::Vector2f pt1;
	sf::Vector2f pt2;

	convex.setPoint(0, sf::Vector2f(x, y));

	for (int i = 0; i < 12; i++)
	{
		convex.setFillColor(sf::Color(128, 128, 128, 128));

		if (prey)
		{
			if (prey_swarm->model->x(id, PreySwarm::vision_size + i*2) > 0.f)
				convex.setFillColor(yellow);
			if (prey_swarm->model->x(id, PreySwarm::vision_size + i*2 + 1) > 0.f)
				convex.setFillColor(magenta);
		}
		else
		{
			if (predator_swarm->model->x(id, PredatorSwarm::vision_size + i) > 0.f)
				convex.setFillColor(yellow);
		}

		a_rad = arc_start + i * PreySwarm::hear_cell_angle_rad;
		pt1 = sf::Vector2f(x + std::cos(a_rad) * range, y + std::sin(a_rad) * range);
		a_rad = arc_start + (i + 1) * PreySwarm::hear_cell_angle_rad;
		pt2 = sf::Vector2f(x + std::cos(a_rad) * range, y + std::sin(a_rad) * range);

		convex.setPoint(1, pt1);
		convex.setPoint(2, pt2);
		this->window->draw(convex);

		line[0].position = pt1;
		line[1].position = pt2;
		this->window->draw(line, 2, sf::Lines);
		line[0].position = pt0;
		this->window->draw(line, 2, sf::Lines);
	}

	line[0].position = pt0;
	line[1].position = sf::Vector2f(x + std::cos(arc_start) * range, y + std::sin(arc_start) * range);
	this->window->draw(line, 2, sf::Lines);
}

void Visualization::render()
{
	this->window->clear();

	this->update();
	this->renderSwarms();

	this->window->display();
}