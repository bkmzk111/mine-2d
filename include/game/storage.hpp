#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <engine/engine.hpp>
#include "consts.hpp"
#include "engine/storage.hpp"

namespace Game {

    class EntityBuilder;
    class World;
    
    namespace EnumData {
        enum class Blocks : uint16_t {
            AIR,
            DIRT,
            GRASS,
            STONE,
            COUNT
        };
        enum class Creatures : uint16_t {
            PLAYER,
            COUNT
        };
    }; //namespace EnumData
    
    //game-specific components
    namespace GameComponents {
        struct ChunkGenerator {
            noise::module::Perlin perlin;
            std::array<int, Game::K::chunk_size.x> heightmap;
            std::array<std::array<EnumData::Blocks, Game::K::chunk_size.x>, Game::K::chunk_size.y> blocks;

            ChunkGenerator(noise::module::Perlin p) : perlin{p} {};
        };
        template<typename T>
        struct VisualManager {
            std::shared_ptr<sf::Texture> atlas;
            std::array<sf::IntRect, static_cast<size_t>(T::COUNT)> visuals;
    
            VisualManager() = default;
        };
    }; // namespace GameComponents

    template<typename T>
    concept GameComponent =
        std::is_same_v<T, GameComponents::ChunkGenerator> ||
        std::is_same_v<T, GameComponents::VisualManager<EnumData::Blocks>>;
    
    //Container for all components
    class World {
        private:
            Engine::PackedStorage<Engine::StorageComponents::Transform> transforms;
            Engine::PackedStorage<Engine::StorageComponents::Camera> cameras;
            Engine::PackedStorage<GameComponents::ChunkGenerator> chunk_generators;
            Engine::PackedStorage<GameComponents::VisualManager<EnumData::Blocks>> block_visuals;
            Engine::PackedStorage<Engine::StorageComponents::PhysicsStatic> p_static_bodies;
            Engine::PackedStorage<Engine::StorageComponents::PhysicsDynamic> p_dynamic_bodies;

            std::unique_ptr<b2World> physics_world;
            Entity count;
        public:
            World() {physics_world = std::make_unique<b2World>(b2Vec2{0.0f, -9.8f}); count = 0;};
    
            World(World&&) = default;
            World& operator=(World&&) = default;
    
            template<typename T> 
            requires Engine::StorageComponent<T> || GameComponent<T>
                Engine::PackedStorage<T>& get_storage_of_component();
    
            template<typename T>
            requires Engine::StorageComponent<T> || GameComponent<T>
                T& get_data_of_entity(Entity e);
    
            EntityBuilder create_entity();
            void load_block_sprites(GameComponents::VisualManager<EnumData::Blocks>& blocks);
            
            inline b2World* get_p_world() { 
                return physics_world.get(); 
            }
    };
    
    //API helper for creating entities with components
    class EntityBuilder {
        private:
            World& w;
            Entity id;
        public:
            EntityBuilder(World& world, Entity e) : w(world), id(e) {};

            template<typename T, typename... Args>
            requires Engine::StorageComponent<T> || GameComponent<T>
                EntityBuilder& with(Args... args);
    
            inline operator Entity() { return id; }
    };

}; //namespace Game

#include "../src/lib/game/storage.tpp"