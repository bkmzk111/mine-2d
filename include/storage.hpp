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
#include "consts.hpp" 

using Entity = uint32_t;
using Vector2f = sf::Vector2f;
using Vector2u = sf::Vector2u;

enum class BLOCK : uint16_t {
    AIR,
    DIRT,
    GRASS,
    COUNT
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
        Vector2f pos{};
        Transform(float x = 0, float y = 0) : pos{x, y} {}
    };
    struct LIB_API Velocity { 
        Vector2f vel{};
        Velocity(float dx = 0, float dy = 0) : vel{dx, dy} {}
    };
    struct BlockStorage {
        std::array<std::array<BLOCK, K::CHUNK_W>, K::CHUNK_H> arr;

        BlockStorage() = default;
    };
    struct Camera {
        std::unique_ptr<sf::RenderTexture> canvas;
        std::unique_ptr<sf::Sprite> drawable;

        Camera() = default;
    };
    struct VisualManager {
        sf::Texture atlas;
        std::array<sf::IntRect, static_cast<size_t>(BLOCK::COUNT)> visuals;

        VisualManager() = default;
    };
};

template<typename T>
concept WorldComponent = 
    std::is_same_v<T, Comps::Transform> || 
    std::is_same_v<T, Comps::Velocity> ||
    std::is_same_v<T, Comps::Camera> ||
    std::is_same_v<T, Comps::BlockStorage> ||
    std::is_same_v<T, Comps::VisualManager>;

class LIB_API WorldStorage {
    private:
        PackedStorage<Comps::Transform> transforms;
        PackedStorage<Comps::Velocity> velocities;
        PackedStorage<Comps::Camera> cameras;
        PackedStorage<Comps::BlockStorage> chunks;
        PackedStorage<Comps::VisualManager> sprites;

        Entity it;
    public:
        WorldStorage() = default;

        WorldStorage(WorldStorage&&) = default;
        WorldStorage& operator=(WorldStorage&&) = default;

        template<WorldComponent T>
        PackedStorage<T>& get_storage_of_component();

        template<WorldComponent T>
        T& get(Entity id);

        EntityBuilder create_entity();
        void prepare_for_loop();
        void load_block_sprites(Comps::VisualManager& blocks);
};

class LIB_API EntityBuilder {
    private:
        WorldStorage& ws;
        Entity id;
    public:
        EntityBuilder(WorldStorage& world, Entity n) : ws(world), id(n) {};
        
        template<WorldComponent T, typename... Args>
        EntityBuilder& with(Args... args);

        inline operator Entity() { return id; }
};

#include "../src/lib/storage.tpp"