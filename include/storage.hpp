#pragma once

#ifdef LIB_EXPORTS
    #define LIB_API __declspec(dllexport)
#else
    #define LIB_API __declspec(dllimport)
#endif

#include <cstdint>
#include <vector> 
#include <unordered_map> 
#include <type_traits> 
#include <memory> 
#include <array>
#include <SFML/Graphics.hpp> 
#include <noise/noise.h>
#include <box2d/box2d.h>
#include "consts.hpp" 

using Entity = uint32_t;

namespace EnumData {
    enum class BLOCKS : uint16_t {
        AIR,
        DIRT,
        GRASS,
        STONE,
        COUNT
    };
    enum class ENTITIES : uint16_t {
        PLAYER,
        COUNT
    };
};

class EntityBuilder;
class WorldStorage;

template<typename T>
class PackedStorage {
    private:
        std::vector<T> data;
        std::vector<Entity> entities;
        std::unordered_map<Entity, size_t> lookup;
    public:
        PackedStorage() = default;

        PackedStorage(const PackedStorage&) = delete;
        PackedStorage& operator=(const PackedStorage&) = delete;

        PackedStorage(PackedStorage&&) = default;
        PackedStorage& operator=(PackedStorage&&) = default;

        bool    has_entity(Entity e) const;
        T&      data_of   (Entity e);       
        T&      data_at   (size_t i);
        Entity& entity_of (const T& c);
        Entity& entity_at (size_t i);
        size_t  data_size () const;
        auto    data_begin();
        auto    data_end  ();

        template<typename... Args>
        T& emplace(Entity e, Args&&... args);
};

namespace Comps {
    struct LIB_API Transform { 
        sf::Vector2f pos{};

        Transform(float x = 0, float y = 0) : pos{x, y} {}
    };
    struct Camera {
        sf::View view;
        std::unique_ptr<sf::RenderTexture> canvas;
        std::unique_ptr<sf::Sprite> drawable;

        Camera() = default;
    };
    template<typename T>
    struct VisualManager {
        sf::Texture atlas;
        std::array<sf::IntRect, static_cast<size_t>(T::COUNT)> visuals;

        VisualManager() = default;
    };
    struct LIB_API ChunkGenerator {
        noise::module::Perlin perlin;
        std::array<float, K::CHUNK_W> heightmap;
        std::array<std::array<EnumData::BLOCKS, K::CHUNK_W>, K::CHUNK_H> block_storage;

        ChunkGenerator(noise::module::Perlin p) : perlin(p) {};
    };
    struct LIB_API PhysicsStatic {
        b2Body* p_body = nullptr;

        PhysicsStatic() = default;
    };
    struct LIB_API PhysicsEntity {
        b2Body* p_body = nullptr;

        PhysicsEntity(b2World* world, b2Vec2 box_shape, b2Vec2 init_pos);
    };  
};

template<typename T>
concept WorldComponent = 
    std::is_same_v<T, Comps::Transform> || 
    std::is_same_v<T, Comps::Camera> ||
    std::is_same_v<T, Comps::VisualManager<EnumData::BLOCKS>> ||
    std::is_same_v<T, Comps::ChunkGenerator> ||
    std::is_same_v<T, Comps::PhysicsEntity> ||
    std::is_same_v<T, Comps::PhysicsStatic>;

class LIB_API WorldStorage {
    private:
        PackedStorage<Comps::Transform> transforms;
        PackedStorage<Comps::Camera> cameras;
        PackedStorage<Comps::VisualManager<EnumData::BLOCKS>> block_sprites;
        PackedStorage<Comps::ChunkGenerator> chunks;
        PackedStorage<Comps::PhysicsStatic> p_grounds;
        PackedStorage<Comps::PhysicsEntity> p_dyna_bodies;

        Entity it = 0;
        std::unique_ptr<b2World> physics_world;
    public:
        WorldStorage() { physics_world = std::make_unique<b2World>(b2Vec2{0.0f, -9.8f}); };

        WorldStorage(WorldStorage&&) = default;
        WorldStorage& operator=(WorldStorage&&) = default;

        template<WorldComponent T>
        PackedStorage<T>& get_storage_of_component();

        b2World* p_world();

        template<WorldComponent T>
        T& get(Entity id);

        EntityBuilder create_entity();
        void prepare();
        void load_block_sprites(Comps::VisualManager<EnumData::BLOCKS>& blocks);
};

class LIB_API EntityBuilder {
    private:
        WorldStorage& ws;
        Entity id;
    public:
        EntityBuilder(WorldStorage& world, Entity n) : ws(world), id(n) {}
        
        template<WorldComponent T, typename... Args>
        EntityBuilder& with(Args... args);

        inline operator Entity() { return id; }
};


#include "../src/lib/storage.tpp"