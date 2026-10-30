// CGT-215-Lab-08-John3823.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

//function that spawns a balloon on the left side of the screen and moves it to the right
void SpawnBalloon(World& world, vector<PhysicsSprite*>& balloons,
    Texture& balloonTex) {

    PhysicsSprite* balloon = new PhysicsSprite();

    balloon->setTexture(balloonTex);

    //sets height for balloon to spawn at
    balloon->setCenter(Vector2f(0, 300));

    // Move to the right
    balloon->setVelocity(Vector2f(0.15, 0));

    world.AddPhysicsBody(*balloon);

    balloons.push_back(balloon);
}


int main()
{
   
	// create window
    RenderWindow window(VideoMode(800, 600), "Balloon Buster");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);
    bool gameOver(false);
    bool spaceReleased(false);

	//create crossbow
    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    //create balloons
    vector<PhysicsSprite*> balloons;

    Texture balloonTex;
    LoadTex(balloonTex, "images/bird.png");

	//create balloon life timer
    Clock balloonClock;

    //create arrow
    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);



    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
        };

    Text scoreText;
    Font font;
    if (!font.loadFromFile("images/Allcaps-Regular.ttf")) {
        cout << "Couldn't load font Allcaps-Regular.ttf" << endl;
        exit(1);
    }
    scoreText.setFont(font);
    scoreText.setCharacterSize(25);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(650, 10);
    scoreText.setString("Score: 0");

    Text arrowCountText;
    arrowCountText.setFont(font);

    arrowCountText.setCharacterSize(25);
    arrowCountText.setFillColor(Color::White);
    arrowCountText.setPosition(10, 10);

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    while (true) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 5) {
            lastTime = currentTime;
            // Check if game is over
            if (gameOver) {

                if (!Keyboard::isKeyPressed(Keyboard::Space)) {
                    spaceReleased = true;
                }

                if (spaceReleased && Keyboard::isKeyPressed(Keyboard::Space)) {
                    window.close();
                    return 0;
                }

                continue;
            }

            world.UpdatePhysics(deltaMS);

            if (drawingArrow) {
                for (int i = 0; i < balloons.size(); i++) {

                    if (arrow.getGlobalBounds().intersects(
                        balloons[i]->getGlobalBounds())) {

                        world.RemovePhysicsBody(arrow);
                        drawingArrow = false;

                        world.RemovePhysicsBody(*balloons[i]);

                        delete balloons[i];

                        balloons.erase(balloons.begin() + i);

                        score++;

                        break;
                    }
                }
            }

            for (int i = 0; i < balloons.size(); i++) {

                if (balloons[i]->getCenter().x > 850) {

                    world.RemovePhysicsBody(*balloons[i]);

                    delete balloons[i];

                    balloons.erase(balloons.begin() + i);

                    i--;
                }
            }

            if (balloonClock.getElapsedTime().asSeconds() > 2) {
                SpawnBalloon(world, balloons, balloonTex);
                balloonClock.restart();
            }

            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow && arrows > 0 && !gameOver) {

                drawingArrow = true;
                arrows--;

                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);

                if (arrows == 0) {
                    gameOver = true;
                }
            }

            window.clear();

            if (gameOver) {

                Text gameOverText;
                gameOverText.setFont(font);
                gameOverText.setCharacterSize(50);
                gameOverText.setFillColor(Color::White);
                gameOverText.setString("GAME OVER");

                FloatRect textBounds = gameOverText.getGlobalBounds();

                gameOverText.setPosition(
                    400 - textBounds.width / 2,
                    250 - textBounds.height / 2
                );

                window.draw(gameOverText);

                Text finalScoreText;
                finalScoreText.setFont(font);
                finalScoreText.setCharacterSize(25);
                finalScoreText.setFillColor(Color::White);
                finalScoreText.setString("Final Score: " + to_string(score));

                finalScoreText.setPosition(310, 330);

                window.draw(finalScoreText);

            }
            else {

                // Draw score
                scoreText.setString("Score: " + to_string(score));
                window.draw(scoreText);

                // Draw balloons
                for (int i = 0; i < balloons.size(); i++) {
                    window.draw(*balloons[i]);
                }

                // Draw arrow
                if (drawingArrow) {
                    window.draw(arrow);
                }

                // Draw crossbow
                window.draw(crossBow);

                // Draw remaining arrows
                arrowCountText.setString("Arrows: " + to_string(arrows));
                window.draw(arrowCountText);
            }

            window.display();
        }
    }

    while (true);
}
