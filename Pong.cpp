#include "SFML/Graphics.hpp"
#include "Font.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <string>

float getCenterX(sf::RectangleShape &backgound, sf::Text &text) {
	sf::FloatRect textRect = text.getGlobalBounds();
	text.setOrigin((textRect.getPosition().x + textRect.getSize().x) / 2, (textRect.getPosition().y + textRect.getSize().y) / 2);
	
	return backgound.getSize().x / 2 - 10;
}

int main() {
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(250, 400), "Pong");

	sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color(128, 128, 128));

	sf::RectangleShape barrier(sf::Vector2f(50, 15));
	barrier.setPosition((background.getSize().x / 2) - (barrier.getSize().x / 2), background.getSize().y - barrier.getSize().y * 2);

	sf::CircleShape ball(12.5);
	ball.setPosition(background.getSize().x / 2, background.getSize().y / 2);
	ball.setOrigin(ball.getRadius(), ball.getRadius());

	sf::Font font;
	if (!font.loadFromMemory(fontHex, 13448)) return 0;

	sf::Text fpsText;
	fpsText.setFont(font);
	fpsText.setString("FPS: 0");
	fpsText.setCharacterSize(15);
	fpsText.setFillColor(sf::Color::Black);
	fpsText.setPosition(sf::Vector2f(5, 5));

	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setString("Score: 0");
	scoreText.setCharacterSize(25);
	scoreText.setFillColor(sf::Color::Red);

	sf::FloatRect scoreTextRect = scoreText.getGlobalBounds();
	scoreText.setOrigin((scoreTextRect.getPosition().x + scoreTextRect.getSize().x) / 2, (scoreTextRect.getPosition().y + scoreTextRect.getSize().y) / 2);
	scoreText.setPosition(sf::Vector2f(background.getSize().x / 2, 35));

	sf::Text highScoreText;
	highScoreText.setFont(font);
	highScoreText.setString("High Score: 0");
	highScoreText.setCharacterSize(20);
	highScoreText.setFillColor(sf::Color::Red);
	
	sf::FloatRect highScoreTextRect = highScoreText.getGlobalBounds();
	highScoreText.setOrigin((highScoreTextRect.getPosition().x + highScoreTextRect.getSize().x) / 2, (highScoreTextRect.getPosition().y + highScoreTextRect.getSize().y) / 2);
	highScoreText.setPosition(sf::Vector2f(background.getSize().x / 2, scoreText.getPosition().y + highScoreText.getCharacterSize()));

	bool mouseMoving = false;
	int diff = 0;

	sf::Clock clock;
	float deltaTime = 0.f;
	float angle = 145 + rand() % 80;
	float pi = 3.14159265359;
	float lastTime = 0.f;
	float fps = -1.f;
	int score = 0;
	int highScore = 0;

	while (window.isOpen()) {
		deltaTime = clock.restart().asSeconds();
		lastTime += deltaTime;

		if (lastTime >= 0.5) {
			lastTime = 0;
			fps = floor((1 / deltaTime) * 100 + 0.5) / 100;

			std::ostringstream ss;
			ss << fps;

			fpsText.setString("FPS: " + ss.str());
		}

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (event.type == sf::Event::MouseButtonPressed) {
				int mouseX = sf::Mouse::getPosition(window).x;
				int mouseY = sf::Mouse::getPosition(window).y;

				if (!mouseMoving) {
					if (mouseX >= barrier.getPosition().x && mouseX <= (barrier.getPosition().x + barrier.getSize().x)) {
						if (mouseY >= barrier.getPosition().y && mouseY <= (barrier.getPosition().y + barrier.getSize().y)) {
							mouseMoving = true;
							diff = mouseX - (barrier.getPosition().x + barrier.getSize().x) / 2;
						}
					}
				}
			}

			if (event.type == sf::Event::MouseButtonReleased) {
				if (mouseMoving) {
					mouseMoving = false;
					diff = 0;
				}
			}

			if (event.type == sf::Event::MouseMoved) {
				if (mouseMoving) {
					int mouseX = sf::Mouse::getPosition(window).x;

					int newDiff = mouseX - (barrier.getPosition().x + barrier.getSize().x) / 2;
					if (newDiff != diff) {
						int newX = newDiff - diff;
						if (newX > 0) {
							barrier.move(sf::Vector2f(newX + 300 * deltaTime, 0));
						}
						else if (newX < 0) {
							barrier.move(sf::Vector2f(newX - 300 * deltaTime, 0));
						}

						if (barrier.getPosition().x + barrier.getSize().x >= background.getSize().x) {
							barrier.setPosition(sf::Vector2f(background.getSize().x - barrier.getSize().x, barrier.getPosition().y));

						}
						else if (barrier.getPosition().x <= 0) {
							barrier.setPosition(sf::Vector2f(0, barrier.getPosition().y));
						}

						diff = newDiff;
					}
				}
			}

			if (event.type == sf::Event::Resized) {
				float ratio = event.size.width / background.getSize().x;
				barrier.setPosition(sf::Vector2f(barrier.getPosition().x * ratio, event.size.height - barrier.getSize().y * 2));
				ball.setPosition(sf::Vector2f(ball.getPosition().x * ratio, ball.getPosition().y * ratio));
				
				scoreText.setPosition(sf::Vector2f(event.size.width / 2, scoreText.getPosition().y));
				highScoreText.setPosition(sf::Vector2f(event.size.width / 2, highScoreText.getPosition().y));

				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				background.setSize(sf::Vector2f(event.size.width, event.size.height));
				window.setView(sf::View(visibleArea));
			}
		}

		if (score > highScore) {
			highScore = score;
			highScoreText.setString("High Score: " + std::to_string(highScore));
		}

		if (ball.getPosition().y <= background.getSize().y) {
			float factor = 150 * deltaTime;
			ball.move(sf::Vector2f(sin(angle * pi / 180) * factor, -cos(angle * pi / 180) * factor));
		}

		if (ball.getGlobalBounds().intersects(barrier.getGlobalBounds())) {
			if (ball.getPosition().y <= barrier.getPosition().y) {
				if (ball.getPosition().x > barrier.getPosition().x + barrier.getSize().x / 2) {
					angle = 180 - angle + rand() % 30;
				}
				else if (ball.getPosition().x < barrier.getPosition().x + barrier.getSize().x / 2) {
					angle = 180 - angle - rand() % 30;
				}
				else {
					angle = 180 - angle + (-30 + rand() % 60);
				}

				++score;
				scoreText.setString("Score: " + std::to_string(score));
			}
		}

		if (ball.getPosition().y - ball.getRadius() <= 0) {
			angle = 180 - angle + (-10 + rand() % 20);
		}

		if (
			ball.getPosition().x + ball.getRadius() >= background.getSize().x ||
			ball.getPosition().x - ball.getRadius() <= 0
			) {
			angle = 360 - angle + (-20 + rand() % 40);
		}

		if (
			ball.getPosition().y + ball.getRadius() >= background.getSize().y
			) {
			ball.setPosition(background.getSize().x / 2, background.getSize().y / 2);
			angle = 145 + (rand() % 80);
			score = 0;
		}

		window.clear();
		window.draw(background);
		window.draw(barrier);
		window.draw(ball);
		window.draw(fpsText);
		window.draw(scoreText);
		window.draw(highScoreText);
		window.display();
	}
}