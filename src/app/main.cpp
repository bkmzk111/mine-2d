
#include <storage.hpp>
#include <system.hpp>

int main(void) {

    sf::RenderWindow window(
        sf::VideoMode(K::WIN_SIZE), 
        "MINE2D", 
        sf::Style::Close
    );
    window.setVerticalSyncEnabled(true);

    noise::module::Perlin perlin;
        perlin.SetSeed(INT_MIN);
        perlin.SetFrequency(0.9);
        perlin.SetOctaveCount(2);
        perlin.SetPersistence(0.5);
        perlin.SetLacunarity(1.5);

    WorldStorage world;

    auto player = world.create_entity()
        .with<Comps::Transform>(6.0f, 25.0f)
        .with<Comps::Camera>()
        .with<Comps::PhysicsEntity>(world.p_world(), b2Vec2{1.0f, 2.0f}, b2Vec2{6.0f, 25.0f});
        auto& main_camera = world.get<Comps::Camera>(player);
    
    auto sprite_holder = world.create_entity()
        .with<Comps::VisualManager<EnumData::BLOCKS>>();
        auto& block_sprites = world.get<Comps::VisualManager<EnumData::BLOCKS>>(sprite_holder);
    
    std::vector<EntityBuilder> chunk_holders;
        for (int i = 0; i < 5; ++i)
            chunk_holders.push_back(world.create_entity().with<Comps::ChunkGenerator>(perlin)
                                                         .with<Comps::Transform>(-2.0f*K::CHUNK_W + i*K::CHUNK_W, 0.0f)
                                                         .with<Comps::PhysicsStatic>());

    world.load_block_sprites(block_sprites);
    world.prepare();

    System::gen_visible_chunks(world);
    System::gen_chunk_hitboxes(world);

    auto [va, states] = System::draw_chunks_va(world, sprite_holder);

    while (window.isOpen()) {
        while(const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Transparent);

        System::apply_tick(world);

        main_camera.view.setSize(sf::Vector2f{K::WIN_SIZE.x, -static_cast<float>(K::WIN_SIZE.y)});
        main_camera.view.setCenter(world.get<Comps::Transform>(player).pos);
        main_camera.canvas->setView(main_camera.view);

        main_camera.canvas->clear(sf::Color(113, 196, 245));
        main_camera.canvas->draw(va, states);
        main_camera.canvas->display();

        const sf::Texture& done = main_camera.canvas->getTexture();
        if (!main_camera.drawable)
            main_camera.drawable = std::make_unique<sf::Sprite>(done);
        main_camera.drawable->setTexture(done, true);

        window.draw(*main_camera.drawable);
        window.display();
    }

    return EXIT_SUCCESS;
}