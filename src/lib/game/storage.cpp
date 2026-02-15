#include "game/consts.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <game/game.hpp>

void Game::World::load_block_sprites(GameComponents::VisualManager<EnumData::Blocks>& block_v) {
    block_v.atlas = std::make_shared<sf::Texture>();
    if (!block_v.atlas->loadFromFile("res/texture/block_list.png")) {
        throw std::runtime_error("Failed to load atlas");
    }
    block_v.atlas->setSmooth(false);
    
    for (int i = 1; i < static_cast<int>(EnumData::Blocks::COUNT); ++i) {
        block_v.visuals[i] = {{K::block_list_tile_size * (i-1), 0}, {K::block_list_tile_size,
                                                                                               K::block_list_tile_size}};
    }
}

