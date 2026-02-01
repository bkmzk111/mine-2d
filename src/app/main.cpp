
#include <storage.hpp>
#include <system.hpp>

int main(void) {

    sf::RenderWindow window(
        sf::VideoMode(K::WIN_SIZE), 
        "MINE2D", 
        sf::Style::Close
    );
    window.setVerticalSyncEnabled(true);

    WorldStorage world;

    noise::module::Perlin perlin;
        perlin.SetSeed(67);
        perlin.SetFrequency(0.9);
        perlin.SetOctaveCount(2);
        perlin.SetPersistence(0.5);
        perlin.SetLacunarity(1.5);

    auto player = world.create_entity()
        .with<Comps::Transform>(0.0f, 0.0f)
        .with<Comps::Velocity>(0.0f, 0.0f)
        .with<Comps::Camera>();
    
    auto sprite_holder = world.create_entity()
        .with<Comps::VisualManager>();
    
    std::vector<EntityBuilder> chunk_holders;
        chunk_holders.push_back(world.create_entity().with<Comps::ChunkGenerator>(perlin)
                                                     .with<Comps::Transform>(0.0f, 0.0f));
        chunk_holders.push_back(world.create_entity().with<Comps::ChunkGenerator>(perlin)
                                                     .with<Comps::Transform>(float(K::CHUNK_W), 0.0f));
        chunk_holders.push_back(world.create_entity().with<Comps::ChunkGenerator>(perlin)
                                                     .with<Comps::Transform>(float(K::CHUNK_W*2), 0.0f));
    System::gen_visible_chunks(world);

    auto& block_sprites = world.get<Comps::VisualManager>(sprite_holder);
    auto& main_camera   = world.get<Comps::Camera>(player);

    world.load_block_sprites(block_sprites);
    world.prepare_for_loop();
    
    System::draw_chunks(world, main_camera, block_sprites);
    while (window.isOpen()) {
        while(const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Transparent);

        System::apply_tick(world);

        window.draw(*main_camera.drawable);
        window.display();
    }

    return EXIT_SUCCESS;
}