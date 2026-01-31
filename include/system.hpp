#pragma once
#include "storage.hpp"

namespace System {
    void LIB_API apply_tick(WorldStorage& ws);
    void LIB_API gen_visible_chunks(WorldStorage& ws);
    void LIB_API regen_chunks_on_canvas(WorldStorage& ws, Comps::Camera& camera, Comps::VisualManager& blocks);
};  

namespace Misc {    
    inline LIB_API Vector2f to_scr(Vector2f coords) {
        return Vector2f{ coords.x + K::WIN_SIZE.x / 2.0f, K::WIN_SIZE.y / 2.0f - coords.y }; 
    }
};