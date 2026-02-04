#pragma once
#include "storage.hpp"
#include <noise/noise.h>

namespace System {
    void LIB_API apply_tick(WorldStorage& ws);
    void LIB_API gen_visible_chunks(WorldStorage& ws);
    void LIB_API gen_chunk_hitboxes(WorldStorage& ws);
    void LIB_API draw_chunks(WorldStorage& ws, Entity player, Entity block_visuals);
};  