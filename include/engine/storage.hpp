#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <type_traits>
#include <memory>
#include "consts.hpp"
#include <box2d/box2d.h>

using Entity = uint32_t;

namespace Engine {

//ECS Storage
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
        T&      data_of(Entity e);
        Entity& entity_of(const T& d);
        size_t  data_size() const;
        auto    data_begin();
        auto    data_end();

        template<typename... Args>
        T& emplace(Entity e, Args&&... args);
};

//engine-level components
namespace StorageComponents {
    struct Transform {
        sf::Vector2f pos{};
        sf::Vector2f render_pos{};

        Transform(float x = 0.0f, float y = 0.0f) : pos{x, y} {}; 
    };
    struct Camera {
        sf::View view;
        std::unique_ptr<sf::RenderTexture> canvas;
        std::unique_ptr<sf::Sprite> drawable;

        inline Camera() { 
            view = sf::View(
                sf::FloatRect{
                    {0.0f, 0.0f}, 
                    {Engine::K::win_size.x, Engine::K::win_size.y}
                }
            );
            canvas = std::make_unique<sf::RenderTexture>(Engine::K::win_size);
            canvas->clear(sf::Color::Transparent);
            canvas->display();
        }
    };
    struct PhysicsStatic {
        b2Body* p_body;

        PhysicsStatic() { p_body = nullptr;};
    };
    struct PhysicsDynamic {
        b2Body* p_body;

        PhysicsDynamic(b2World* world, b2Vec2 box_shape, b2Vec2 init_pos);
    };

}; //namespace StorageComponents

template<typename T>
    concept StorageComponent =
        std::is_same_v<T, Engine::StorageComponents::Transform> ||
        std::is_same_v<T, Engine::StorageComponents::Camera> ||
        std::is_same_v<T, Engine::StorageComponents::PhysicsStatic> ||
        std::is_same_v<T, Engine::StorageComponents::PhysicsDynamic>;

}; //namespace Engine

#include "../../src/lib/engine/storage.tpp"

