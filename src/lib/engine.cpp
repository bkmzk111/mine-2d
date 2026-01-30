#include "engine.hpp"

using namespace Comps;

EntityBuilder WorldStorage::create_entity() {
    it+=1;
    return EntityBuilder{*this, it};
}

void WorldStorage::generate_world() {
    for (size_t i = 0; i < chunks.data_size(); ++i) {
        BlockStorage& chunk = chunks.data_at(i);

        chunk.arr = {{
            {BLOCK::AIR, BLOCK::DIRT, BLOCK::AIR, BLOCK::DIRT},
            {BLOCK::DIRT, BLOCK::DIRT, BLOCK::DIRT, BLOCK::DIRT},
            {BLOCK::AIR, BLOCK::AIR, BLOCK::AIR, BLOCK::AIR},
        }};
    }
}
void WorldStorage::prepare_for_loop() {
    for (size_t i = 0; i < cameras.data_size(); ++i) {
        Comps::Camera& camera = cameras.data_at(i);
        camera.canvas = std::make_unique<sf::RenderTexture>(K::WIN_SIZE);
    }
}
void WorldStorage::apply_tick() {
    for (size_t i = 0; i < velocities.data_size(); ++i) {
        Entity e = velocities.entity_at(i);

        if (!transforms.has_entity(e)) 
            continue;

        Transform& t = transforms.data_of(e);
        Velocity&  v = velocities.data_of(e);

        t.pos.x += v.vel.x;
        t.pos.y += v.vel.y;
    }
}
void WorldStorage::draw_world(Comps::Camera& camera, SpriteManager& blocks) {

    Entity& cam_entity = cameras.entity_of(camera);
    Transform& cam_pos = transforms.data_of(cam_entity);

    camera.canvas->clear(sf::Color::Transparent);

    const float chunk_ws = K::CHUNK_W * K::BLOCK_S;
    const float chunk_hs = K::CHUNK_H * K::BLOCK_S;

    int chunk_index = 0;
    for (auto it = chunks.data_begin(); it != chunks.data_end(); ++it, ++chunk_index) {
        
        Vector2f chunk_offset{chunk_ws * chunk_index, 0};

        for (int i = 0; i < K::CHUNK_H * K::CHUNK_W; ++i) {
            int r = i / K::CHUNK_W;
            int c = i % K::CHUNK_W;
            
            if (it->arr[r][c] == BLOCK::AIR)
                continue;

            Vector2f world_coord{c*K::BLOCK_S, -r*K::BLOCK_S};
            world_coord += chunk_offset;

            Vector2f rel = world_coord - cam_pos.pos;
            Vector2f scr_coord = Misc::to_scr(rel);

            sf::Sprite& block(*blocks.arr[static_cast<size_t>(BLOCK::DIRT)].spr);
            block.setPosition(scr_coord);

            camera.canvas->draw(block);
        }
    }
    camera.canvas->display();

    const sf::Texture& done = camera.canvas->getTexture();
    if (!camera.drawable)
        camera.drawable = std::make_unique<sf::Sprite>(done);
    camera.drawable->setTexture(done, true);
}

void Misc::load_block_sprites(SpriteManager& blocks) {
    sf::Texture dirt_txtr;
    if (!dirt_txtr.loadFromFile("res\\texture\\block\\dirt.png"))
        return;
    dirt_txtr.setSmooth(false);

    Vector2f scale{
        K::BLOCK_S / dirt_txtr.getSize().x,
        K::BLOCK_S / dirt_txtr.getSize().y
    };

    blocks.arr[static_cast<size_t>(BLOCK::DIRT)].txtr = std::move(dirt_txtr);

    sf::Sprite dirt_spr(blocks.arr[static_cast<size_t>(BLOCK::DIRT)].txtr);
    dirt_spr.setScale(scale);
    blocks.arr[static_cast<size_t>(BLOCK::DIRT)].spr = std::make_unique<sf::Sprite>(std::move(dirt_spr));
}
