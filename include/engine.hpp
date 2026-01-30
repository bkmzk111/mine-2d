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

        inline bool    has_entity(Entity e)   const { return lookup.contains(e); }
        inline T&      data_of   (Entity e)         { return data[lookup[e]]; }
        inline T&      data_at   (size_t i)         { return data[i]; }
        inline Entity& entity_of (const T& c)       { return entities[&c - data.data()]; }
        inline Entity& entity_at (size_t i)         { return entities[i]; }
        inline size_t  data_size ()           const { return data.size(); }
        inline auto    data_begin()                 { return data.begin(); }
        inline auto    data_end  ()                 { return data.end(); }

        template<typename... Args>
        inline T& emplace(Entity e, Args&&... args) {
            if (this->has_entity(e)) 
                return data_of(e);
            
            size_t index = data.size();
            data.emplace_back(std::forward<Args>(args)...);
            entities.push_back(e);
            lookup[e] = index;
            return data.back();
        }
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
        sf::VertexArray va;
        std::array<sf::IntRect, static_cast<size_t>(BLOCK::COUNT)> visuals;

        VisualManager() {
            va.setPrimitiveType(sf::PrimitiveType::Triangles);
        };
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
        inline PackedStorage<T>& get_storage_of_component() {
            if constexpr (std::is_same_v<T, Comps::Transform>)
                return transforms;
            if constexpr (std::is_same_v<T, Comps::Velocity>)
                return velocities;
            if constexpr (std::is_same_v<T, Comps::Camera>)
                return cameras;
            if constexpr (std::is_same_v<T, Comps::BlockStorage>)
                return chunks;
            if constexpr (std::is_same_v<T, Comps::VisualManager>)
                return sprites;
        }

        template<WorldComponent T>
        inline T& get(Entity id) { 
            return get_storage_of_component<T>().data_of(id);
        }

        EntityBuilder create_entity();

        void generate_world();
        void prepare_for_loop();
        void apply_tick();
        void draw_world(Comps::Camera& camera, Comps::VisualManager& blocks);
};

class LIB_API EntityBuilder {
    private:
        WorldStorage& ws;
        Entity id;
    public:
        EntityBuilder(WorldStorage& world, Entity n) : ws(world), id(n) {};
        
        template<WorldComponent T, typename... Args>
        inline EntityBuilder& with(Args... args) {
            ws.get_storage_of_component<T>().emplace(id, T{std::forward<Args>(args)...});
            return *this;
        }
        inline operator Entity() { return id; }
};

namespace Misc {
    LIB_API void load_block_sprites(Comps::VisualManager& blocks);

    inline LIB_API Vector2f to_scr(Vector2f coords) {
        return Vector2f{ coords.x + K::WIN_SIZE.x / 2.0f, K::WIN_SIZE.y / 2.0f - coords.y }; 
    }
};
