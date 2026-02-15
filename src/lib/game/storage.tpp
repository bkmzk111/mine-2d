
template<typename T>
requires Engine::StorageComponent<T> || Game::GameComponent<T>
Engine::PackedStorage<T>& Game::World::get_storage_of_component() {
    if constexpr (std::is_same_v<T, Engine::StorageComponents::Transform>)
        return transforms;
    if constexpr (std::is_same_v<T, Engine::StorageComponents::Camera>)
        return cameras;
    if constexpr (std::is_same_v<T, GameComponents::ChunkGenerator>)
        return chunk_generators;
    if constexpr (std::is_same_v<T, GameComponents::VisualManager<EnumData::Blocks>>)
        return block_visuals;
    if constexpr (std::is_same_v<T, Engine::StorageComponents::PhysicsStatic>)
        return p_static_bodies;
    if constexpr (std::is_same_v<T, Engine::StorageComponents::PhysicsDynamic>)
        return p_dynamic_bodies;
}
template<typename T>
requires Engine::StorageComponent<T> || Game::GameComponent<T>
T& Game::World::get_data_of_entity(Entity e) { 
    return get_storage_of_component<T>().data_of(e); 
}

template<typename T, typename... Args>
requires Engine::StorageComponent<T> || Game::GameComponent<T>
Game::EntityBuilder& Game::EntityBuilder::with(Args... args) {
    w.get_storage_of_component<T>().emplace(id, T{std::forward<Args>(args)...});
    return *this;
}