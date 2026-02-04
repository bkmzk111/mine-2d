#include "../include/system.hpp"

void System::apply_tick(WorldStorage& ws) {
    auto& p_dyna_bodies = ws.get_storage_of_component<Comps::PhysicsEntity>();
    auto& transforms = ws.get_storage_of_component<Comps::Transform>();

    ws.p_world()->Step(1.0f / 60.0f, 8, 3);
    for (auto it = p_dyna_bodies.data_begin(); it != p_dyna_bodies.data_end(); ++it) {
        auto& ts = transforms.data_of(p_dyna_bodies.entity_of(*it));
        b2Vec2 p_pos = it->p_body->GetPosition();

        ts.pos = {p_pos.x * K::BLOCK_S, p_pos.y * K::BLOCK_S};
    }
}
void System::gen_chunk_hitboxes(WorldStorage& ws) {
    auto& chunks = ws.get_storage_of_component<Comps::ChunkGenerator>();
    auto& hitboxes = ws.get_storage_of_component<Comps::PhysicsStatic>();
    auto& transforms = ws.get_storage_of_component<Comps::Transform>();

    struct HeightSegment {
        int start_x;
        int width;
        int height;
    };
    
    for (auto it = chunks.data_begin(); it != chunks.data_end(); ++it) {
        auto& hb = hitboxes.data_of(chunks.entity_of(*it));
        auto& ts = transforms.data_of(chunks.entity_of(*it));

        std::vector<HeightSegment> segments;
        int x = 0;
        while (x < K::CHUNK_W) {
            int h = it->heightmap[x];
            int start_x = x;
            while (x < K::CHUNK_W && it->heightmap[x] == h)
                ++x;
            int width = x - start_x;
            segments.push_back(HeightSegment{start_x, width, h});
        }

        b2BodyDef grnd_def;
            grnd_def.type = b2_staticBody;
            grnd_def.position = {ts.pos.x, ts.pos.y};

        b2Body* grnd = ws.p_world()->CreateBody(&grnd_def);

        for (const auto& seg : segments) {
            float hx = seg.width * 0.5f;
            float hy = 0.5f;

            float center_x = seg.start_x + hx;
            float center_y = seg.height - hy;

            b2PolygonShape shape;
            shape.SetAsBox(hx, hy, b2Vec2(center_x, center_y), 0.0f);

            b2FixtureDef fixDef;
            fixDef.shape = &shape;
            fixDef.friction = 0.6f;

            grnd->CreateFixture(&fixDef);
        }
        hb.p_body = std::move(grnd);
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

        for (size_t x = 0; x < K::CHUNK_W; ++x) {
            float val = float(it->perlin.GetValue(double(transform.pos.x + x) * 0.05, 0.0, 0.0));
            val = (val + 1.0f) * 0.5f; 
            it->heightmap[x] = static_cast<int>(val * (K::CHUNK_H - 1));
        }

        for (int x = 0; x < K::CHUNK_W; ++x)
            for (int y = 0; y < K::CHUNK_H; ++y) { 
                float stone_depth = compute_depth(x, y, it->heightmap[x]);

                if (y > it->heightmap[x])
                    it->block_storage[y][x] = BLOCK::AIR;
                else if (y == it->heightmap[x])
                    it->block_storage[y][x] = BLOCK::GRASS;
                else if (it->heightmap[x] - y < stone_depth)
                    it->block_storage[y][x] = BLOCK::DIRT;
                else
                    it->block_storage[y][x] = BLOCK::STONE;
            }
    }
}
void System::draw_chunks(WorldStorage& ws, Entity player, Entity block_visuals) {
    auto& cameras = ws.get_storage_of_component<Comps::Camera>();
    auto& transforms = ws.get_storage_of_component<Comps::Transform>();
    auto& chunks = ws.get_storage_of_component<Comps::ChunkGenerator>();
    auto& visuals = ws.get_storage_of_component<Comps::VisualManager>();

    Comps::Camera& main_camera = cameras.data_of(player);
    Comps::Transform& cam_pos = transforms.data_of(player);
    Comps::VisualManager& blocks = visuals.data_of(block_visuals);

    sf::VertexArray va;
    va.resize(chunks.data_size() * K::CHUNK_W * K::CHUNK_H * 6);
    va.setPrimitiveType(sf::PrimitiveType::Triangles);

    main_camera.view.setSize(sf::Vector2f{K::WIN_SIZE.x, -static_cast<float>(K::WIN_SIZE.y)});
    main_camera.view.setCenter(cam_pos.pos);
    main_camera.canvas->setView(main_camera.view);

    int block_index = 0;
    int chunk_index = 0;

    for (auto it = chunks.data_begin(); it != chunks.data_end(); ++it, ++chunk_index) {
        auto& chunk_ts = transforms.data_of(chunks.entity_of(*it));
        sf::Vector2f chunk_world = sf::Vector2f{chunk_ts.pos.x, chunk_ts.pos.y} * K::BLOCK_S;

        for (int i = 0; i < K::CHUNK_W * K::CHUNK_H; ++i) {
            int y = i / K::CHUNK_W;
            int x = i % K::CHUNK_W;
            BLOCK block = it->block_storage[y][x];

            if (block == BLOCK::AIR)
                continue;

            sf::IntRect uv = blocks.visuals[static_cast<size_t>(block)];

            sf::Vector2f block_local{
                x * K::BLOCK_S,
                y * K::BLOCK_S
            };
            sf::Vector2f s_coord = chunk_world + block_local;

            va[block_index+0].position = s_coord;
            va[block_index+1].position = {s_coord.x+K::BLOCK_S, s_coord.y};
            va[block_index+2].position = {s_coord.x+K::BLOCK_S, s_coord.y+K::BLOCK_S};
            va[block_index+3].position = {s_coord.x+K::BLOCK_S, s_coord.y+K::BLOCK_S};
            va[block_index+4].position = {s_coord.x, s_coord.y+K::BLOCK_S};
            va[block_index+5].position = s_coord;

            va[block_index+0].texCoords = {float(uv.position.x), float(uv.position.y + uv.size.y)};
            va[block_index+1].texCoords = {float(uv.position.x + uv.size.x), float(uv.position.y + uv.size.y)};
            va[block_index+2].texCoords = {float(uv.position.x + uv.size.x), float(uv.position.y)};
            va[block_index+3].texCoords = {float(uv.position.x + uv.size.x), float(uv.position.y)};
            va[block_index+4].texCoords = {float(uv.position.x), float(uv.position.y)};
            va[block_index+5].texCoords = {float(uv.position.x), float(uv.position.y + uv.size.y)};

            block_index+=6;
        }
    }
    va.resize(block_index);
    sf::RenderStates states;
    states.texture = &blocks.atlas;

    main_camera.canvas->clear(sf::Color(113, 196, 245));
    main_camera.canvas->draw(va, states);
    main_camera.canvas->display();

    const sf::Texture& done = main_camera.canvas->getTexture();
    if (!main_camera.drawable)
        main_camera.drawable = std::make_unique<sf::Sprite>(done);
    main_camera.drawable->setTexture(done, true);
}