#include "game/consts.hpp"
#include "game/storage.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <game/game.hpp>

using namespace Game;

EntityBuilder World::create_entity() {
    count+=1;
    return EntityBuilder{*this, count};
}

void System::gen_visible_chunks(World& w) {
    auto& chunks = w.get_storage_of_component<GameComponents::ChunkGenerator>();
    auto& transforms = w.get_storage_of_component<Engine::StorageComponents::Transform>();

    for (auto it = chunks.data_begin(); it != chunks.data_end(); ++it) {
        auto& transform = transforms.data_of(chunks.entity_of(*it));

        auto compute_depth = [&](int x, int y, float h) -> float {
            float depth = 6.0f + float(it->perlin.GetValue(x * 0.1, y * 0.1, 0.0) * 4.0);
            depth -= (h > 10 ? 2 : 0);
            return depth;
        };

        for (size_t x = 0; x < K::chunk_size.x; ++x) {
            float val = float(it->perlin.GetValue(double(transform.pos.x + x) * 0.05, 0.0, 0.0));
            val = (val + 1.0f) * 0.5f; 
            it->heightmap[x] = static_cast<int>(val * (K::chunk_size.y - 1));
        }

        for (int x = 0; x < K::chunk_size.x; ++x)
            for (int y = 0; y < K::chunk_size.y; ++y) { 
                if (y > it->heightmap[x])
                {
                    float depth_from_surface = y - it->heightmap[x];
                    float stone_depth = compute_depth(transform.pos.x + x, transform.pos.y + y, it->heightmap[x]);
                    it->blocks[y][x] = (depth_from_surface >= stone_depth) 
                    ? EnumData::Blocks::STONE 
                    : EnumData::Blocks::DIRT;
                }
                else if (y == it->heightmap[x])
                    it->blocks[y][x] = EnumData::Blocks::GRASS;
                else
                    it->blocks[y][x] = EnumData::Blocks::AIR;
            }
    }
}

std::pair<sf::VertexArray, sf::RenderStates> System::draw_chunks_va(World& w, GameComponents::VisualManager<EnumData::Blocks>& block_visuals) { 
    auto& transforms = w.get_storage_of_component<Engine::StorageComponents::Transform>();
    auto& chunks = w.get_storage_of_component<GameComponents::ChunkGenerator>();

    sf::VertexArray va;
    va.setPrimitiveType(sf::PrimitiveType::Triangles);
    va.resize(chunks.data_size() * K::chunk_size.x * K::chunk_size.y * 6);

    int block_index = 0;
    int chunk_index = 0;

    for (auto it = chunks.data_begin(); it != chunks.data_end(); ++it, ++chunk_index) {
        auto& chunk_ts = transforms.data_of(chunks.entity_of(*it));
        sf::Vector2f chunk_world = sf::Vector2f{chunk_ts.pos.x, chunk_ts.pos.y} * K::block_size;

        for (int i = 0; i < K::chunk_size.x * K::chunk_size.y; ++i) {
            int y = i / K::chunk_size.x;
            int x = i % K::chunk_size.x;
            EnumData::Blocks block = it->blocks[y][x];

            if (block == EnumData::Blocks::AIR)
                continue;

            sf::IntRect uv = block_visuals.visuals[static_cast<size_t>(block)];

            sf::Vector2f block_local{
                x * K::block_size,
                y * K::block_size
            };
            sf::Vector2f s_coord = chunk_world + block_local;

            va[block_index+0].position = s_coord;
            va[block_index+1].position = {s_coord.x+K::block_size, s_coord.y};
            va[block_index+2].position = {s_coord.x+K::block_size, s_coord.y+K::block_size};
            va[block_index+3].position = {s_coord.x+K::block_size, s_coord.y+K::block_size};
            va[block_index+4].position = {s_coord.x, s_coord.y+K::block_size};
            va[block_index+5].position = s_coord;

            va[block_index+0].texCoords = {float(uv.position.x), float(uv.position.y)};
            va[block_index+1].texCoords = {float(uv.position.x + uv.size.x), float(uv.position.y)};
            va[block_index+2].texCoords = {float(uv.position.x + uv.size.x), float(uv.position.y + uv.size.y)};
            va[block_index+3].texCoords = {float(uv.position.x + uv.size.x), float(uv.position.y + uv.size.y)};
            va[block_index+4].texCoords = {float(uv.position.x), float(uv.position.y + uv.size.y)};
            va[block_index+5].texCoords = {float(uv.position.x), float(uv.position.y)};

            
            block_index+=6;
        }
    }
    va.resize(block_index);
    sf::RenderStates states;
    states.texture = block_visuals.atlas.get();

    return {va, states};
}

void System::gen_chunk_hitboxes(World& w) {
    auto& chunks = w.get_storage_of_component<GameComponents::ChunkGenerator>();
    auto& hitboxes = w.get_storage_of_component<Engine::StorageComponents::PhysicsStatic>();
    auto& transforms = w.get_storage_of_component<Engine::StorageComponents::Transform>();

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
        while (x < Game::K::chunk_size.x) {
            int h = it->heightmap[x];
            int start_x = x;
            while (x < Game::K::chunk_size.x && it->heightmap[x] == h)
                ++x;
            int width = x - start_x;
            segments.push_back(HeightSegment{start_x, width, h});
        }

        b2BodyDef grnd_def;
            grnd_def.type = b2_staticBody;
            grnd_def.position = {ts.pos.x, ts.pos.y};

        b2Body* grnd = w.get_p_world()->CreateBody(&grnd_def);

        for (const auto& seg : segments) {
            float hx = seg.width * 0.5f;
            float hy = 0.5f;

            float center_x = seg.start_x + seg.width * 0.5f;
            float center_y = -it->heightmap[seg.start_x] - hy; 

            b2PolygonShape shape;
            shape.SetAsBox(hx, hy, b2Vec2(center_x, center_y), 0.0f);

            b2FixtureDef fixDef;
            fixDef.shape = &shape;
            fixDef.friction = 0.5f;

            grnd->CreateFixture(&fixDef);
        }
        hb.p_body = std::move(grnd);
    }
}

void System::apply_tick(World& w) {
    auto& p_dyna_bodies = w.get_storage_of_component<Engine::StorageComponents::PhysicsDynamic>();
    auto& transforms = w.get_storage_of_component<Engine::StorageComponents::Transform>();

    w.get_p_world()->Step(1.0f / 60.0f, 8, 3);
    for (auto it = p_dyna_bodies.data_begin(); it != p_dyna_bodies.data_end(); ++it) {
        auto& ts = transforms.data_of(p_dyna_bodies.entity_of(*it));
        b2Vec2 p_pos = it->p_body->GetPosition();

        ts.pos = {p_pos.x, p_pos.y};
        ts.render_pos = {p_pos.x * K::block_size, -p_pos.y * K::block_size};
    }
}