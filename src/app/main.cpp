
#include <storage.hpp>
#include <system.hpp>
#include <iostream>

int main(void) {
    noise::module::Perlin perlin_noise;
        perlin_noise.SetSeed(INT_MIN);
        perlin_noise.SetFrequency(0.9);
        perlin_noise.SetOctaveCount(2);
        perlin_noise.SetPersistence(0.5);
        perlin_noise.SetLacunarity(1.5);

    sf::RenderWindow window(
        sf::VideoMode(K::WIN_SIZE), 
        "MINE2D", 
        sf::Style::Close
    );
    window.setVerticalSyncEnabled(true);

    WorldStorage world;

    auto player = world.create_entity()
        .with<Comps::Transform>(6.0f, 25.0f)
        .with<Comps::Camera>()
        .with<Comps::PhysicsEntity>(world.p_world(), b2Vec2{1.0f, 2.0f}, b2Vec2{6.0f, 25.0f});
        auto& main_camera = world.get<Comps::Camera>(player);
        auto& player_pos  = world.get<Comps::Transform>(player);
    
    auto sprite_holder = world.create_entity()
        .with<Comps::VisualManager<EnumData::BLOCKS>>();
        auto& block_sprites = world.get<Comps::VisualManager<EnumData::BLOCKS>>(sprite_holder);
    
    std::vector<EntityBuilder> chunk_holders;

    world.load_block_sprites(block_sprites);
    world.prepare();

    std::pair<sf::VertexArray, sf::RenderStates> render_data;
    bool dirty = true;

    auto make_drawable = [&]() -> void {
        const sf::Texture& done = main_camera.canvas->getTexture();
        if (!main_camera.drawable)
            main_camera.drawable = std::make_unique<sf::Sprite>(done);
        main_camera.drawable->setTexture(done, true);
    };
    auto regen_chunks = [&]() -> void {
        chunk_holders.clear();

        const int player_chunk_x = static_cast<int>(std::floor(player_pos.pos.x / (K::CHUNK_W * K::BLOCK_S)));
        for (int dx = -2; dx <= 2; ++dx) {
            const float chunk_x = (player_chunk_x + dx) * K::CHUNK_W;

            chunk_holders.push_back(
                world.create_entity()
                    .with<Comps::ChunkGenerator>(perlin_noise)
                    .with<Comps::Transform>(chunk_x, 0.0f)
                    .with<Comps::PhysicsStatic>()
            );
        }

        System::gen_visible_chunks(world);
        System::gen_chunk_hitboxes(world);
        render_data = System::draw_chunks_va(world, sprite_holder);
    };

    while (window.isOpen()) {
        player_pos = world.get<Comps::Transform>(player);
        
        while(const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        if (dirty) {
            regen_chunks();
            dirty = false;
        }

        window.clear(sf::Color::Transparent);

        System::apply_tick(world);

        main_camera.view.setSize(sf::Vector2f{K::WIN_SIZE.x, -static_cast<float>(K::WIN_SIZE.y)});
        main_camera.view.setCenter(player_pos.pos);
        main_camera.canvas->setView(main_camera.view);

        main_camera.canvas->clear(sf::Color(113, 196, 245));
        main_camera.canvas->draw(render_data.first, render_data.second);
        main_camera.canvas->display();

        make_drawable();

        window.draw(*main_camera.drawable);
        window.display();
    }

    return EXIT_SUCCESS;
}