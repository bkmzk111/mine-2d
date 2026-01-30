#pragma once

#ifdef LIB_EXPORTS
    #define LIB_API __declspec(dllexport)
#else
    #define LIB_API __declspec(dllimport)
#endif

#include <cstdint> //23.01
#include <vector> // 23.01
#include <unordered_map> // 23.01
#include <type_traits> // 23.01
#include <memory> // 24.01
#include <array> //25.01
#include <SFML/Graphics.hpp> // 24.01
#include "consts.hpp" //29.01

using Entity = uint32_t;
using Vector2f = sf::Vector2f;
using Vector2u = sf::Vector2u;

enum class BLOCK : uint16_t {
    AIR,
    DIRT
};

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

        inline T& emplace(Entity e, T&& component_data) {
            if (this->has_entity(e)) 
                return data_of(e);
            
            size_t index = data.size();
            data.emplace_back(std::move(component_data));
            entities.push_back(e);
            lookup[e] = index;
            return data.back();
        }
};

namespace Comps {
    struct LIB_API Transform { 
        Vector2f pos{};
    };
    struct LIB_API Velocity { 
        Vector2f vel{};
    };
    struct BlockStorage {
        std::array<std::array<BLOCK, K::CHUNK_W>, K::CHUNK_H> arr;
        static std::unordered_map<BLOCK, sf::Texture> textures;
        static std::unordered_map<BLOCK, sf::Sprite> sprites;

        BlockStorage() = default;
    };
    struct Camera {
        std::unique_ptr<sf::RenderTexture> canvas;
        std::unique_ptr<sf::Sprite> drawable;

        Camera() = default;
    };
};

template<typename T>
concept WorldComponent = 
    std::is_same_v<T, Comps::Transform> || 
    std::is_same_v<T, Comps::Velocity> ||
    std::is_same_v<T, Comps::Camera> ||
    std::is_same_v<T, Comps::BlockStorage>;

class LIB_API WorldStorage {
    private:
        PackedStorage<Comps::Transform> transforms;
        PackedStorage<Comps::Velocity> velocities;
        PackedStorage<Comps::Camera> cameras;
        PackedStorage<Comps::BlockStorage> chunks;
    public:
        WorldStorage() = default;

        WorldStorage(const WorldStorage&) = delete;
        WorldStorage& operator=(const WorldStorage&) = delete;

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
        }
        
        void generate_world();
        void prepare_for_loop();
        void apply_tick();
        void draw_world(Comps::Camera& camera);
};

namespace Misc {
    LIB_API void load_res(WorldStorage& ws);

    inline LIB_API Vector2f to_scr(Vector2f coords) {
        return Vector2f{ coords.x + K::WIN_SIZE.x / 2.0f, K::WIN_SIZE.y / 2.0f - coords.y }; 
    }
};