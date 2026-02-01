#include "../include/system.hpp"
#include <iostream>

void System::apply_tick(WorldStorage& ws) {
    auto& transforms = ws.get_storage_of_component<Comps::Transform>();
    auto& velocities = ws.get_storage_of_component<Comps::Velocity>();

    for (size_t i = 0; i < velocities.data_size(); ++i) {
        Entity e = velocities.entity_at(i);

        if (!transforms.has_entity(e)) 
            continue;

        Comps::Transform& t = transforms.data_of(e);
        Comps::Velocity&  v = velocities.data_of(e);

        t.pos.x += v.vel.x;
        t.pos.y += v.vel.y;
    }
}
void System::gen_visible_chunks(WorldStorage& ws) {
    auto& chunks = ws.get_storage_of_component<Comps::ChunkGenerator>();
    auto& transforms = ws.get_storage_of_component<Comps::Transform>();

    for (auto it = chunks.data_begin(); it != chunks.data_end(); ++it) {
        auto& transform = transforms.data_of(chunks.entity_of(*it));

        auto compute_depth = [&](int x, int y, float h) -> float {
            float depth = 6.0f + float(it->perlin.GetValue(x * 0.1, y * 0.1, 0.0) * 4.0);
            depth -= (h > 10 ? 2 : 0);
            return depth;
        };

        float heightmap[K::CHUNK_W];
        for (size_t i = 0; i < K::CHUNK_W; ++i) {
            float val = float(it->perlin.GetValue(double(transform.pos.x + i)*0.05, 0.0, 0.0));
            val = (val+1.0f) * 0.5f;
            heightmap[i] = static_cast<int>(val * (K::CHUNK_H - 1));
        }

        for (auto& h : heightmap)
            std::cout << h << ", ";
        std::cout << std::endl;

        for (int i = 0; i < K::CHUNK_W * K::CHUNK_H; ++i) {
            int x = i % K::CHUNK_W;

            int array_y = i / K::CHUNK_W;
            int world_y = (K::CHUNK_H - 1) - array_y;

            float stone_depth = compute_depth(x, world_y, heightmap[x]);

            if (world_y > heightmap[x])
                it->block_storage[array_y][x] = BLOCK::AIR;
            else if (world_y == heightmap[x])
                it->block_storage[array_y][x] = BLOCK::GRASS;
            else if (heightmap[x] - world_y < stone_depth)
                it->block_storage[array_y][x] = BLOCK::DIRT;
            else
                it->block_storage[array_y][x] = BLOCK::STONE;
        }
    }
}
void System::draw_chunks(WorldStorage& ws, Comps::Camera& camera, Comps::VisualManager& blocks) {
    auto& cameras = ws.get_storage_of_component<Comps::Camera>();
    auto& transforms = ws.get_storage_of_component<Comps::Transform>();
    auto& chunks = ws.get_storage_of_component<Comps::ChunkGenerator>();

    Entity cam_entity = cameras.entity_of(camera);
    Comps::Transform& cam_pos = transforms.data_of(cam_entity);

    sf::VertexArray va;
    va.resize(chunks.data_size() * K::CHUNK_W * K::CHUNK_H * 6);
    va.setPrimitiveType(sf::PrimitiveType::Triangles);

    int block_index = 0;
    int chunk_index = 0;

    for (auto it = chunks.data_begin(); it != chunks.data_end(); ++it, ++chunk_index) {
        auto& chunk_ts = transforms.data_of(chunks.entity_of(*it));
        Vector2f chunk_world = chunk_ts.pos * K::BLOCK_S;

        for (int i = 0; i < K::CHUNK_W * K::CHUNK_H; ++i) {
            int y = i / K::CHUNK_W;
            int x = i % K::CHUNK_W;
            BLOCK block = it->block_storage[y][x];

            if (block == BLOCK::AIR)
                continue;

            sf::IntRect uv = blocks.visuals[static_cast<size_t>(block)];

            Vector2f block_local{
                x * K::BLOCK_S,
                -y * K::BLOCK_S
            };
            Vector2f w_coord = chunk_world + block_local;
            Vector2f s_coord = Misc::to_scr(w_coord - cam_pos.pos);

            va[block_index+0].position = s_coord;
            va[block_index+1].position = {s_coord.x+K::BLOCK_S, s_coord.y};
            va[block_index+2].position = {s_coord.x+K::BLOCK_S, s_coord.y+K::BLOCK_S};
            va[block_index+3].position = {s_coord.x+K::BLOCK_S, s_coord.y+K::BLOCK_S};
            va[block_index+4].position = {s_coord.x, s_coord.y+K::BLOCK_S};
            va[block_index+5].position = s_coord;

            va[block_index+0].texCoords = sf::Vector2f(uv.position);
            va[block_index+1].texCoords = {float(uv.position.x+uv.size.x), float((uv.position.y))};
            va[block_index+2].texCoords = {float(uv.position.x+uv.size.x), float((uv.position.y+uv.size.y))};
            va[block_index+3].texCoords = {float(uv.position.x+uv.size.x), float((uv.position.y+uv.size.y))};
            va[block_index+4].texCoords = {float(uv.position.x), float((uv.position.y+uv.size.y))};
            va[block_index+5].texCoords = sf::Vector2f(uv.position);

            block_index+=6;
        }
    }
    va.resize(block_index);
    
    sf::RenderStates states;
    states.texture = &blocks.atlas;

    camera.canvas->clear(sf::Color(113, 196, 245));
    camera.canvas->draw(va, states);
    camera.canvas->display();

    const sf::Texture& done = camera.canvas->getTexture();
    if (!camera.drawable)
        camera.drawable = std::make_unique<sf::Sprite>(done);
    camera.drawable->setTexture(done, true);
}