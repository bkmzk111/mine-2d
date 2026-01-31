#include "../include/system.hpp"

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
    auto& chunks = ws.get_storage_of_component<Comps::BlockStorage>();

    for (size_t i = 0; i < chunks.data_size(); ++i) {
        Comps::BlockStorage& chunk = chunks.data_at(i);

        chunk.arr = {{
            {BLOCK::GRASS, BLOCK::GRASS, BLOCK::GRASS, BLOCK::GRASS},
            {BLOCK::DIRT, BLOCK::DIRT, BLOCK::DIRT, BLOCK::DIRT},
            {BLOCK::DIRT, BLOCK::DIRT, BLOCK::DIRT, BLOCK::DIRT},
        }};
    }
}
void System::regen_chunks_on_canvas(WorldStorage& ws, Comps::Camera& camera, Comps::VisualManager& blocks) {
    auto& cameras = ws.get_storage_of_component<Comps::Camera>();
    auto& transforms = ws.get_storage_of_component<Comps::Transform>();
    auto& chunks = ws.get_storage_of_component<Comps::BlockStorage>();

    Entity cam_entity = cameras.entity_of(camera);
    Comps::Transform& cam_pos = transforms.data_of(cam_entity);

    sf::VertexArray va;
    va.resize(chunks.data_size() * K::CHUNK_W * K::CHUNK_H * 6);
    va.setPrimitiveType(sf::PrimitiveType::Triangles);

    int block_index = 0;
    int chunk_index = 0;

    for (auto it = chunks.data_begin(); it != chunks.data_end(); ++it, ++chunk_index) {
        Vector2f chunk_offset{chunk_index * K::CHUNK_W * K::BLOCK_S, 0};

        for (int i = 0; i < K::CHUNK_W * K::CHUNK_H; ++i) {
            int y = i / K::CHUNK_W;
            int x = i % K::CHUNK_W;
            BLOCK block = it->arr[y][x];

            if (block == BLOCK::AIR)
                continue;

            sf::IntRect uv = blocks.visuals[static_cast<size_t>(block)];

            Vector2f w_coord{x*K::BLOCK_S, -y*K::BLOCK_S};
            w_coord += chunk_offset;
            Vector2f rel = w_coord - cam_pos.pos;
            Vector2f s_coord = Misc::to_scr(rel);

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

    camera.canvas->draw(va, states);
    camera.canvas->display();

    const sf::Texture& done = camera.canvas->getTexture();
    if (!camera.drawable)
        camera.drawable = std::make_unique<sf::Sprite>(done);
    camera.drawable->setTexture(done, true);
}