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
void WorldStorage::draw_world(Comps::Camera& camera, VisualManager& blocks) {

}

void Misc::load_block_sprites(VisualManager& blocks) {
    sf::Texture atlas;
    if (!atlas.loadFromFile("res\\texture\\block_list.png"))
        return;
    atlas.setSmooth(false);
    blocks.atlas = std::move(atlas);
    blocks.visuals[static_cast<int>(BLOCK::DIRT)] = {{0,0}, {16,16}};
}
