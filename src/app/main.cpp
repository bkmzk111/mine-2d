
#include "engine/storage.hpp"
#include "game/consts.hpp"
#include "game/storage.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <engine/engine.hpp>
#include <game/game.hpp>

using namespace Engine;
using namespace Game;

int main(void) {

    sf::RenderWindow window(
        sf::VideoMode(Engine::K::win_size), 
        "MINE2D", 
        sf::Style::Close
    );
    window.setVerticalSyncEnabled(true);

    noise::module::Perlin perlin_noise;
        perlin_noise.SetSeed(67);
        perlin_noise.SetFrequency(0.9);
        perlin_noise.SetOctaveCount(2);
        perlin_noise.SetPersistence(0.5);
        perlin_noise.SetLacunarity(1.5);

    World world;

    auto player = world.create_entity()
        .with<StorageComponents::Transform>(15.0f, 0.0f)
        .with<StorageComponents::Camera>()
        .with<StorageComponents::PhysicsDynamic>(world.get_p_world(), b2Vec2(1.0f, 2.0f), 
                                                                              b2Vec2{15.0f, 0.0f});
        auto& camera = world.get_data_of_entity<StorageComponents::Camera>(player);
        auto& playerBody = world.get_data_of_entity<StorageComponents::PhysicsDynamic>(player).p_body;

    std::vector<EntityBuilder> chunk_entities;
    for (int i = 0; i < 3; ++i) {
        auto e = world.create_entity()
            .with<StorageComponents::Transform>((-1.0f + i ) * Game::K::chunk_size.x , 0.0f)
            .with<GameComponents::ChunkGenerator>(perlin_noise)
            .with<StorageComponents::PhysicsStatic>();
        chunk_entities.push_back(e);
    }

    auto block_visuals = world.create_entity()
        .with<GameComponents::VisualManager<EnumData::Blocks>>();
        auto& block_v = world.get_data_of_entity<GameComponents::VisualManager<EnumData::Blocks>>(block_visuals);

    world.load_block_sprites(block_v);

    System::gen_visible_chunks(world);
    System::gen_chunk_hitboxes(world);

    auto make_camera_drawable = [&camera]() {
        if (!camera.drawable)
            camera.drawable = std::make_unique<sf::Sprite>(camera.canvas->getTexture());
        else 
            camera.drawable->setTexture(camera.canvas->getTexture(), true);
};

    auto [va, states] = System::draw_chunks_va(world, block_v);

    while (window.isOpen()) {
        
        System::apply_tick(world);
        auto& player_pos = world.get_data_of_entity<StorageComponents::Transform>(player);
        while(const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Red);

        camera.view.setCenter(player_pos.render_pos);
        camera.canvas->setView(camera.view);
        camera.canvas->clear(sf::Color(123, 196, 237)); //sky blue

        camera.canvas->draw(va, states); 
        camera.canvas->display();

        make_camera_drawable();

        window.draw(*camera.drawable);
        window.display();
    }

    return EXIT_SUCCESS;
}