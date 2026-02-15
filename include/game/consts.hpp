#pragma once
#include <SFML/System.hpp>

namespace Game {
    namespace K {
        constexpr sf::Vector2u chunk_size = {16u, 30u};
        constexpr float block_size = 32.0f;
        constexpr int block_list_tile_size = 16;
    }; // namespace K
}; // namespace Game