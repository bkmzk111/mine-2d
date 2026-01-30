
#include <engine.hpp>

int main(void) {

    WorldStorage world;

    Entity player = 1;
    std::vector<Entity> chunk_holders;
        chunk_holders.push_back(100);
        chunk_holders.push_back(101);

    Misc::load_res(world);

    auto& transforms = world.get_storage_of_component<Comps::Transform>();
    auto& velocities = world.get_storage_of_component<Comps::Velocity>();
    auto& cameras    = world.get_storage_of_component<Comps::Camera>();
    auto& chunks     = world.get_storage_of_component<Comps::BlockStorage>();

    auto& player_pos  = transforms.emplace(player, Comps::Transform{{50.0f, -50.0f}});
    auto& player_vel  = velocities.emplace(player, Comps::Velocity{{0.0f, 0.0f}});
    auto& main_camera = cameras.emplace(player, Comps::Camera{});

    for (auto& chunk : chunk_holders)
        chunks.emplace(chunk, Comps::BlockStorage{});

    sf::RenderWindow window(
        sf::VideoMode(K::WIN_SIZE), 
        "MINE2D", 
        sf::Style::Close
    );
    window.setVerticalSyncEnabled(true);

    world.generate_world();
    world.prepare_for_loop();

    while (window.isOpen()) {
        while(const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Black);

        world.apply_tick();
        world.draw_world(main_camera);

        window.draw(*main_camera.drawable);
        window.display();
    }

    return EXIT_SUCCESS;
}