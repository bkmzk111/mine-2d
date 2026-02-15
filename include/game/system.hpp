#include <game/storage.hpp>

namespace Game {

namespace System {

    void apply_tick(World& w);
    void gen_visible_chunks(World& w);
    void gen_chunk_hitboxes(World& w);
    std::pair<sf::VertexArray, sf::RenderStates> draw_chunks_va(
        World& w, 
        GameComponents::VisualManager<EnumData::Blocks>& block_visuals
    );

}; // namespace System

}; //namespace Game