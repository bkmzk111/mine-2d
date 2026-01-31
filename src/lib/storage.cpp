#include "storage.hpp"

using namespace Comps;

EntityBuilder WorldStorage::create_entity() {
    it+=1;
    return EntityBuilder{*this, it};
}
void WorldStorage::prepare_for_loop() {
    for (size_t i = 0; i < cameras.data_size(); ++i) {
        Comps::Camera& camera = cameras.data_at(i);
        camera.canvas = std::make_unique<sf::RenderTexture>(K::WIN_SIZE);
        camera.canvas->clear(sf::Color::Transparent);
    }
}

void WorldStorage::load_block_sprites(VisualManager& blocks) {
    sf::Texture atlas;
    if (!atlas.loadFromFile("res\\texture\\block_list.png"))
        throw std::runtime_error("Failed to load block_list.png");
    atlas.setSmooth(false);
    blocks.atlas = std::move(atlas);
    blocks.visuals[static_cast<int>(BLOCK::DIRT)]  = {{0,0}, {16,16}};
    blocks.visuals[static_cast<int>(BLOCK::GRASS)] = {{16,0},{16,16}};
}
