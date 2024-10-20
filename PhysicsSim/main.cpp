#include <SFML/Graphics.hpp>
#include "solver.hpp"
#include "renderer.hpp"
#include <iostream>


int main()
{


    const int WIDTH = 1000;
    const int HEIGHT = 1000;
    const char TITLE[] = "Physics Sim";
    const int MAX_FPS = 60;


    // Window setup
    sf::RenderWindow win(sf::VideoMode(WIDTH, HEIGHT), TITLE, sf::Style::Titlebar | sf::Style::Close);
    sf::Event ev;
    win.setFramerateLimit(MAX_FPS);

    // initialisation
    Solver solver;
    Renderer renderer{ win };
    sf::Clock clock = sf::Clock::Clock();
    bool chain = true;
    bool spring = false;


    //solver.addObject({ 400.0f, 200.0f }, 50.0f);

    
    //int id1 = solver.addObject({ 300.0f, 600.0f }, 10.0f, true);
    //int id2 = solver.addObject({ 500.0f, 250.0f }, 10.0f);
    
    /*
    int id1 = solver.addObject({ 400.0f, 450.0f }, 10.0f, false, {0.0f, -1.0f});
    int id2 = solver.addObject({ 600.0f, 450.0f }, 10.0f, false, {0.0f, 1.0f});
    */
    //solver.addLink(id1, id2, 0.001f);
    //solver.addSpring(id1, id2, 100.0f);

    if (chain)
        {
        int id1 = solver.addObject({ 300.0f, 600.0f }, 10.0f, true);
        int id2;
        int nb_circle = 20;
        for (int i = 1; i < nb_circle; i++)
        {
            id2 = solver.addObject({ 300.0f + 22*i, 600.0f}, 10.0f);
            // decrease strength
            if (spring) solver.addSpring(id1, id2, 1000000.0f);
            else solver.addLink(id1, id2, 0.5f);
            id1 = id2;
        }

        id2 = solver.addObject({ 300.0f + 22 * nb_circle, 600.0f }, 10.0f, true);

        if (spring) solver.addSpring(id1, id2, 1000000.0f);
        else solver.addLink(id1, id2, 0.5f);
    }

    


    
    //std::cout << "vector address " << & solver.getObject() << "\n";
    //std::cout << "obj address " << &obj << "\n";
    //std::cout << "[0] address " << & solver.getObject()[0] << "\n";
    //std::cout << "obj2 adress " << & obj2 << "\n";
    //std::cout << "[1] address " << & solver.getObject()[1] << "\n";
    
    // variables for camera movement
    bool isCameraMoving = false;
    sf::Vector2f camera_start_position;
    sf::Vector2f camera_start_screen;


    float fps;
    int frame = 0;
    sf::Time pre = clock.getElapsedTime();
    sf::Time now;
    // main loop
    while (win.isOpen())
    {
        // Event polling
        while (win.pollEvent(ev))
        {
            switch (ev.type)
            {
            case sf::Event::Closed:
                win.close();
                break;
            case sf::Event::KeyPressed:
                if (ev.key.code == sf::Keyboard::Escape)
                {
                    win.close();
                }
                else if (ev.key.code == sf::Keyboard::Space)
                {
                    sf::Vector2f pos = renderer.getWorldCoordinate(static_cast<sf::Vector2f>(sf::Mouse::getPosition(win)));
                    solver.addObject(pos, 10.0f);
                }
                else if (ev.key.code == sf::Keyboard::S)
                {
                    sf::Vector2f pos = renderer.getWorldCoordinate(static_cast<sf::Vector2f>(sf::Mouse::getPosition(win)));
                    solver.spawnSoftBody(pos, 10.0f, 10000.0f, 500);
                }
                else if (ev.key.code == sf::Keyboard::M)
                {
                    sf::Vector2f pos = renderer.getWorldCoordinate(static_cast<sf::Vector2f>(sf::Mouse::getPosition(win)));
                    solver.createPressureSoftBody(pos, 20, 10.0f, 2000.0f, 10000.0f);
                }
                else if (ev.key.code == sf::Keyboard::D)
                {
                    sf::Vector2f pos = renderer.getWorldCoordinate(static_cast<sf::Vector2f>(sf::Mouse::getPosition(win)));
                    solver.addRectSoftBody(pos, 10, 10, 5, 10000.0f, 500);
                }
                break;
            case sf::Event::MouseWheelScrolled:
            {
                float zoom_change = 1 + ev.mouseWheelScroll.delta / 10.0f;
                renderer.changeZoom(zoom_change, static_cast<sf::Vector2f>(sf::Mouse::getPosition(win)));
                break;
            }

            case sf::Event::MouseButtonPressed:

                // check if middle button is pressed:
                if (ev.mouseButton.button == sf::Mouse::Middle)
                {
                    isCameraMoving = true;
                    camera_start_screen = static_cast<sf::Vector2f>(sf::Mouse::getPosition(win));
                    camera_start_position = renderer.getCameraPosition();
                }
                break;
            case sf::Event::MouseButtonReleased:

                // check if middle button:
                if (ev.mouseButton.button == sf::Mouse::Middle)
                {
                    isCameraMoving = false;
                }
                break;
            }
        }
    

        // update camera position
        if (isCameraMoving)
        {
            sf::Vector2f mouse_displacment = camera_start_screen - static_cast<sf::Vector2f>(sf::Mouse::getPosition(win));
            
            renderer.setPosition(camera_start_position + mouse_displacment / renderer.getScale());
        }
        

        // Update Physics

        solver.update(1.0f/MAX_FPS);

        // Render Window
        win.clear();        // clear old fram


        renderer.render(solver);


        frame++;
        now = clock.getElapsedTime();
        if (frame % 10 == 0) 
        {
            fps = 1.0f / (now.asSeconds() - pre.asSeconds());
            //std::cout << "fps: " << floor(fps) << "\n";
        }
        pre = now;
        // draw simulation

        win.display();      // draw current frame
    }


    return 0;
}