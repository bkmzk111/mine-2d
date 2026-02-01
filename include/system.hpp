#pragma once
#include "storage.hpp"
#include <noise/noise.h>

namespace System {
    void LIB_API apply_tick(WorldStorage& ws);
    void LIB_API gen_visible_chunks(WorldStorage& ws);
    void LIB_API draw_chunks(WorldStorage& ws, Comps::Camera& camera, Comps::VisualManager& blocks);
};  

namespace Misc {    
    inline LIB_API Vector2f to_scr(Vector2f world_pos, Vector2f cam_pos) {
        return {
            world_pos.x - cam_pos.x,
            K::WIN_SIZE.y - (world_pos.y - cam_pos.y)
        };
    }
};