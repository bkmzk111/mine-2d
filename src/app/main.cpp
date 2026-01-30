
#include <engine.hpp>

int main(void) {

    sf::RenderWindow window(
        sf::VideoMode(K::WIN_SIZE), 
        "MINE2D", 
        sf::Style::Close
    );
    window.setVerticalSyncEnabled(true);

    WorldStorage world;

    auto player = world.create_entity()
        .with<Comps::Transform>(0.0f, 0.0f)
        .with<Comps::Velocity>(0.0f, 0.0f)
        .with<Comps::Camera>();
    
    auto sprite_holder = world.create_entity()
        .with<Comps::SpriteManager>();
    
    std::vector<EntityBuilder> chunk_holders;
        chunk_holders.push_back(world.create_entity().with<Comps::BlockStorage>());
        chunk_holders.push_back(world.create_entity().with<Comps::BlockStorage>());
    world.generate_world();

    auto& block_sprites = world.get<Comps::SpriteManager>(sprite_holder);
    auto& main_camera   = world.get<Comps::Camera>(player);

    Misc::load_block_sprites(block_sprites);
    
    world.prepare_for_loop();
    while (window.isOpen()) {
        while(const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Transparent);

        world.apply_tick();
        world.draw_world(main_camera, block_sprites);

        window.draw(*main_camera.drawable);
        window.display();
    }

    return EXIT_SUCCESS;
}