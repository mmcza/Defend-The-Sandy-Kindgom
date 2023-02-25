#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "windows.h"
#include <stdlib.h>
#include <time.h> 

class pocisk : sf::CircleShape
{
    float x_, y_, timer_, angle_, ratio_, size_;
public:
    sf::CircleShape circle;
    pocisk(const float &angle, const float& ratio, const float& size)
    {
        timer_ = 0.0;
        x_ = 140.0;
        y_ = 580.0;
        angle_ = angle * 3.14/180;
        ratio_ = ratio;
        size_ = size;
        circle.setRadius(size_);
        circle.setPosition(x_, y_);
        circle.setFillColor(sf::Color(200, 100, 30));
    }
    void fly(float &time_elapsed_seconds)
    {
        timer_ += time_elapsed_seconds;
        x_ = 140.0 + 5000 * timer_ * cos(angle_) / ratio_;
        y_ = 580.0 + 1000 * timer_ * timer_ - 1500 * 1 * timer_ * sin(angle_);
        circle.setPosition(x_, y_);
    }
};

class ground
{
public:
    std::vector<int> level = {};
    int x = 10;
    ground()
    {
        srand(time(NULL));
        int last = 700;
        for (int i = 0; i < 1500; i++)
        {
            level.emplace_back(last);
            last += rand() % 5 - 2;
            if (last > 700)
            {
                last = 700;
            }
        }
    }
};

sf::Text napis(std::string s, int char_size, float x, float y, sf::Color color_)
{
    sf::Text text_;
    //text_.setFont(font);
    text_.setString(s);
    text_.setCharacterSize(char_size);
    text_.setFillColor(color_);
    text_.setPosition({ x,y });
    return text_;
}

class enemy : sf::Sprite
{
    sf::Texture texture_;
    float velocity,x,y;
public:
    std::string name;
    int points, gold;
    sf::Sprite opponent;
    enemy(ground lvl)
    {
        x = 1420.0;
        int pom = int(x) - 200;
        y = lvl.level[pom]-75;
        int hero = rand() % 18;
        if (hero < 10)
        {
            if (!texture_.loadFromFile("soldier.png"))
            {
                std::cout << "Failed loading the soldier texture\n";
            }
            velocity = 9;
            points = 5;
            gold = 5;
            name = "Soldier";
        }
        if (hero > 9 && hero < 15)
        {
                if (!texture_.loadFromFile("scorpion.png"))
                {
                    std::cout << "Failed loading the scorpion texture\n";
                }
                velocity = 25;
                points = 15;
                gold = 10;
                name = "Scorpion";
        }
        if (hero > 14 && hero < 17)
        {
                if (!texture_.loadFromFile("tank.png"))
                {
                    std::cout << "Failed loading the tank texture\n";
                }
                velocity = 15;
                points = 25;
                gold = 15;
                name = "Tank";
        }
        if (hero == 17)
        {
            if (!texture_.loadFromFile("dragon1.png"))
            {
                std::cout << "Failed loading the dragon texture\n";
            }
            velocity = 37;
            points = 35;
            gold = 25;
            name = "Dragon";
            y = rand() % 300 + 100;
        }
        opponent.setTexture(texture_);
        opponent.setPosition(x, y);
    }
    void move(ground lvl, float time_elapsed_seconds)
    {
        x = x - time_elapsed_seconds*velocity;
        if (name == "Dragon")
        {
            opponent.setPosition(x, y);
            opponent.setTexture(texture_);
        }
        else
        {
            int pom = int(x) - 200;
            if (pom <= 0)
            {
                pom = 0;
                y = 700;
            }
            y = lvl.level[pom + 35] - 75;
            opponent.setPosition(x, y);
            opponent.setTexture(texture_);
        }
    }
};

void collisions(ground lvl, std::vector<enemy>& opponents, std::vector<pocisk>& bullets, int &hp, int &points, int &gold, int &points2, int maxballs)
{
    for (int i = 0; i < bullets.size(); i++)
    {
        sf::FloatRect circlebounds = bullets[i].circle.getGlobalBounds();
        int pom = int(circlebounds.left + (circlebounds.width) / 2);
        if (pom > 1499)
        {
            pom = 1499;
        }
        if (circlebounds.top + circlebounds.height >= lvl.level[pom])
        {
            bullets.erase(bullets.begin() + i);
        }
        else if (circlebounds.left > 1500)
        {
            bullets.erase(bullets.begin() + i);
        }
        else
        {
            for (int j = 0; j < opponents.size(); j++)
            {
                sf::FloatRect opponentbounds = opponents[j].opponent.getGlobalBounds();
                float circle_bot = circlebounds.top + circlebounds.height;
                float circle_right = circlebounds.left + circlebounds.width;
                float opponent_bot = opponentbounds.top + opponentbounds.height;
                float opponent_right = opponentbounds.left + opponentbounds.width;
                if ((circle_bot >= opponentbounds.top && circle_bot <= opponent_bot) || (circlebounds.top >= opponentbounds.top && circlebounds.top <= opponent_bot))
                {
                    if (circlebounds.left >= opponentbounds.left && circlebounds.left <= opponent_right)
                    {
                        points += opponents[j].points;
                        points2 += opponents[j].points;
                        gold += opponents[j].gold;
                        bullets.erase(bullets.begin() + i);
                        opponents.erase(opponents.begin() + j);
                        break;
                    }
                    else if (circle_right >= opponentbounds.left && circle_right <= opponent_right)
                    {
                        points += opponents[j].points;
                        points2 += opponents[j].points;
                        gold += opponents[j].gold;
                        bullets.erase(bullets.begin() + i);
                        opponents.erase(opponents.begin() + j);
                        break;
                    }
                }
            }
        }
    }
    for (int j = 0; j < opponents.size(); j++)
    {
        sf::FloatRect opponentbounds = opponents[j].opponent.getGlobalBounds();
        if (opponentbounds.left <= 200.0)
        {
            hp -= 10;
            opponents.erase(opponents.begin() + j);
        }
    }
    if (bullets.size() > maxballs)
    {
        bullets.erase(bullets.end() -1);
    }
}

void mouse_operations(sf::Vector2f mouse, int &hp, int &gold, int &max_balls, float &ratio)
{
    if (mouse.x >= 1550 && mouse.x <= 1750 && mouse.y>=200 && mouse.y <= 275)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (gold >= 50)
            {
                hp += 10;
                gold -= 50;
            }
        }
    }
    else if (mouse.x >= 1550 && mouse.x <= 1750 && mouse.y >= 350 && mouse.y <= 425)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (gold >= 75)
            {
                max_balls += 1;
                gold -= 75;
            }
        }
    }
    else if (mouse.x >= 1550 && mouse.x <= 1750 && mouse.y >= 500 && mouse.y <= 575)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (gold >= 40)
            {
                ratio =ratio/1.2;
                gold -= 40;
            }
        }
    }
}

int main() 
{
        sf::RenderWindow window(sf::VideoMode(1800, 700), "The best game ever");
        float angle = 45;
        float timer = 0;
        int i = 0;
        float generator = 4.0;
        double avg, sum = 0;
        float ratio = 5, size = 10;
        int hp = 100, gold = 0, points = 0, points2 = 0, max_balls = 5;
        sf::Clock clock;
        std::vector<pocisk> bullets;
        ground lvl;
        sf::Image background_image;
        bool starting = 0;
        if (!background_image.loadFromFile("test.png"))
        {
            std::cout << "Failed loading the background\n";
            return 0;
        }
        for (int x = 0; x < 1300; x++)
        {
            for (int y = lvl.level[x]; y < 700; y++)
            {
                background_image.setPixel(x + 200, y, sf::Color::Yellow);
            }
        }
        sf::Texture background_texture;
        background_texture.loadFromImage(background_image);
        sf::Sprite background;
        background.setTexture(background_texture);


        sf::Font impact;
        if (!impact.loadFromFile("impact.ttf"))
        {
            std::cout << "Failed loading the font\n";
            return 0;
        }

        sf::Texture castle_texture;
        if (!castle_texture.loadFromFile("castle.png"))
        {
            std::cout << "Failed loading the castle texture\n";
            return 0;
        }
        sf::Sprite castle;
        castle.setTexture(castle_texture);
        castle.setPosition({ 10,600 });

        sf::Texture cannon_texture;
        if (!cannon_texture.loadFromFile("cannon.png"))
        {
            std::cout << "Failed loading the cannon texture\n";
            return 0;
        }
        sf::Sprite cannon;
        cannon.setTexture(cannon_texture);
        cannon.setPosition({ 80,610 });
        cannon.setScale({ 1.7,1.7 });

        sf::Texture hp_n;
        if (!hp_n.loadFromFile("hp_n.png"))
        {
            std::cout << "Failed loading button texture\n";
            return 0;
        }
        sf::Texture hp_a;
        if (!hp_a.loadFromFile("hp_a.png"))
        {
            std::cout << "Failed loading button texture\n";
            return 0;
        }
        sf::Sprite hpbutton;
        hpbutton.setTexture(hp_n);
        hpbutton.setPosition({ 1550,200 });

        sf::Texture maxb_n;
        if (!maxb_n.loadFromFile("maxb_n.png"))
        {
            std::cout << "Failed loading button texture\n";
            return 0;
        }
        sf::Texture maxb_a;
        if (!maxb_a.loadFromFile("maxb_a.png"))
        {
            std::cout << "Failed loading button texture\n";
            return 0;
        }
        sf::Sprite maxbbutton;
        maxbbutton.setTexture(maxb_n);
        maxbbutton.setPosition({ 1550,350 });

        sf::Texture ratio_n;
        if (!ratio_n.loadFromFile("ratio_n.png"))
        {
            std::cout << "Failed loading button texture\n";
            return 0;
        }
        sf::Texture ratio_a;
        if (!ratio_a.loadFromFile("ratio_a.png"))
        {
            std::cout << "Failed loading button texture\n";
            return 0;
        }
        sf::Sprite ratiobutton;
        ratiobutton.setTexture(ratio_n);
        ratiobutton.setPosition({ 1550,500 });

        sf::SoundBuffer tank_buffer;
        if (!tank_buffer.loadFromFile("tank.wav"))
        {
            std::cout << "Failed loading tank sound\n";
            return -1;
        }
        sf::Sound tank_sound;
        tank_sound.setBuffer(tank_buffer);

        sf::SoundBuffer soldier_buffer;
        if (!soldier_buffer.loadFromFile("soldier.wav"))
        {
            std::cout << "Failed loading soldier sound\n";
            return -1;
        }
        sf::Sound soldier_sound;
        soldier_sound.setBuffer(soldier_buffer);

        sf::SoundBuffer scorpion_buffer;
        if (!scorpion_buffer.loadFromFile("scorpion.wav"))
        {
            std::cout << "Failed loading scorpion sound\n";
            return -1;
        }
        sf::Sound scorpion_sound;
        scorpion_sound.setBuffer(scorpion_buffer);

        sf::SoundBuffer dragon_buffer;
        if (!dragon_buffer.loadFromFile("dragon.wav"))
        {
            std::cout << "Failed loading dragon sound\n";
            return -1;
        }
        sf::Sound dragon_sound;
        dragon_sound.setBuffer(dragon_buffer);

        std::vector<enemy> opponents;

        sf::Music music;
        if (!music.openFromFile("song.wav"))
        {
            return -1;
        }
        music.play();

    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                angle += 0.5;
                if (angle >= 90)
                {
                    angle = 90;
                }
                std::cout << angle << "\n";
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                angle -= 0.5;
                if (angle < 0)
                {
                    angle = 0;
                }
                std::cout << angle << "\n";
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                bullets.emplace_back(pocisk(angle, ratio, size));
            }
        }
        if (starting == 0)
        {
            window.clear(sf::Color::Blue);
            window.draw(background);
            std::string s = "In some far dimension the Sandy Kingdom has been struggling with attacks of their greedy neighbours.\n       The Amers, Australs and Sybers after many years of unseccesful attempts came to conclusion \n                                                                that it will be easier to create one big army. \n                      And with help of the Dragon Airforces crossed the border of Sandy Kindgdom. \n\nKing Cawa I has a special task for you. You get the best cannon in the castle and have to stop the upcoming enemies...\n\n                                                                              Press SPACE to continue";
            sf::Text angle_text = napis(s, 30, 100, 220, sf::Color::White);
            angle_text.setFont(impact);
            window.draw(angle_text);
            window.display();
            if (opponents.size() > 0)
            {
                opponents.erase(opponents.begin());
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                starting=1;
            }
        }
        else if (hp>0 && points<4000)
        {
        sf::Time elapsed = clock.getElapsedTime();
        sf::Int64 time_elapsed = elapsed.asMicroseconds();
        timer += elapsed.asSeconds();
        sum += time_elapsed;
        i++;
        window.clear(sf::Color::Blue);
        if (timer > generator)
        {
            timer = 0;
            opponents.emplace_back(lvl);
            if (generator > 1.5)
            {
                int a = opponents.size() - 1;
                if (opponents[a].name == "Tank")
                {
                    tank_sound.play();
                }
                else if (opponents[a].name == "Soldier")
                {
                    soldier_sound.play();
                }
                else if (opponents[a].name == "Scorpion")
                {
                    scorpion_sound.play();
                }
                else if (opponents[a].name == "Dragon")
                {
                    dragon_sound.play();
                }
            }
        }
        if (points2 >= 80)
        {
            generator -= 0.3;
            if (generator < 0.2)
            {
                generator = 0.2;
            }
            points2 = 0;
        }
        // draw everything here...

        window.draw(background);
        if (gold >= 50)
        {
            hpbutton.setTexture(hp_a);
        }
        else
        {
            hpbutton.setTexture(hp_n);
        }
        if (gold >= 75)
        {
            maxbbutton.setTexture(maxb_a);
        }
        else
        {
            maxbbutton.setTexture(maxb_n);
        }
        if (gold >= 40)
        {
            ratiobutton.setTexture(ratio_a);
        }
        else
        {
            ratiobutton.setTexture(ratio_n);
        }
        window.draw(hpbutton);
        window.draw(ratiobutton);
        window.draw(maxbbutton);
        collisions(lvl, opponents, bullets, hp, points, gold, points2, max_balls);
        for (int i = 0; i < bullets.size(); i++)
        {
            float time_elapsed_seconds = elapsed.asSeconds();
            bullets[i].fly(time_elapsed_seconds);
            window.draw(bullets[i].circle);
        }
        elapsed = clock.restart();

        std::ostringstream angle_to_disp;
        angle_to_disp << "Angle: " << angle;
        std::string a_s = angle_to_disp.str();
        sf::Text angle_text = napis(a_s, 24, 10, 550, sf::Color::White);
        angle_text.setFont(impact);
        window.draw(angle_text);

        std::ostringstream hp_to_disp;
        hp_to_disp << "Hp: " << hp;
        std::string hp_s = hp_to_disp.str();
        sf::Text hp_text = napis(hp_s, 30, 20, 50, sf::Color::Red);
        hp_text.setFont(impact);
        window.draw(hp_text);

        std::ostringstream pts_to_disp;
        pts_to_disp << "Points: " << points;
        std::string pts_s = pts_to_disp.str();
        sf::Text pts_text = napis(pts_s, 30, 1520, 20, sf::Color::Red);
        pts_text.setFont(impact);
        window.draw(pts_text);

        std::ostringstream gold_to_disp;
        gold_to_disp << "Gold: " << gold;
        std::string g_s = gold_to_disp.str();
        sf::Text gold_text = napis(g_s, 30, 1520, 80, sf::Color::Yellow);
        gold_text.setFont(impact);
        window.draw(gold_text);

        cannon.setRotation(25 - angle);

        window.draw(cannon);
        window.draw(castle);
        float time_elapsed_seconds = elapsed.asSeconds();
        if (opponents.size() > 0)
        {
            for (int i = 0; i < opponents.size(); i++)
            {
                opponents[i].move(lvl, time_elapsed_seconds);
                window.draw(opponents[i].opponent);
            }
        }
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        mouse_operations(mouse, hp, gold, max_balls, ratio);
        window.display();
        }
        else if (points >= 4000)
        {
        opponents.clear();
        bullets.clear();
        window.clear(sf::Color::Blue);
        window.draw(background);
        std::string s = "Congratulations you saved the Sandy Kingdom!";
        sf::Text angle_text = napis(s, 70, 100, 220, sf::Color::White);
        angle_text.setFont(impact);
        window.draw(angle_text);
        window.display();
        }
        else
        {
        opponents.clear();
        bullets.clear();
        window.clear(sf::Color::Blue);
        window.draw(background);
        std::string s = "Unfortunately the Sandy Kindgom has been conquered";
        sf::Text angle_text = napis(s, 50, 100, 220, sf::Color::White);
        angle_text.setFont(impact);
        window.draw(angle_text);
        window.display();
        }
    }
    sum = sum / 1000000;
    avg = i/sum;
    std::cout << "Wyswietlono lacznie " << i << " klatek w czasie "<<sum<<"sekund\n";
    std::cout << "Average frames per second: " << avg << "\n";
    return 0;
}