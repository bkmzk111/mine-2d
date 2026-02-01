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
    for (int i = 1; i < static_cast<int>(BLOCK::COUNT); ++i)
        blocks.visuals[i] = {{16 * (i-1), 0}, {16,16}};
}
