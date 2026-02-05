
template<typename T>
template<typename... Args>
T& PackedStorage<T>::emplace(Entity e, Args&&... args) {  
    if (this->has_entity(e)) 
        return data_of(e);
            
    size_t index = data.size();
    data.emplace_back(std::forward<Args>(args)...);
    entities.push_back(e);
    lookup[e] = index;
    return data.back();      
}

template<typename T> 
    bool PackedStorage<T>::has_entity(Entity e) const { return lookup.contains(e); }
template<typename T>
    T& PackedStorage<T>::data_of(Entity e) { return data[lookup[e]]; }
template<typename T>
    T& PackedStorage<T>::data_at(size_t i) { return data[i]; }
template<typename T>
    Entity& PackedStorage<T>::entity_of(const T& c) { return entities[&c - data.data()]; }
template<typename T>
    Entity& PackedStorage<T>::entity_at(size_t i) { return entities[i]; }
template<typename T>
    size_t PackedStorage<T>::data_size() const { return data.size(); }
template<typename T>
    auto PackedStorage<T>::data_begin() { return data.begin(); }
template<typename T>
    auto PackedStorage<T>::data_end() { return data.end(); }

template<WorldComponent T>
PackedStorage<T>& WorldStorage::get_storage_of_component() {
    if constexpr (std::is_same_v<T, Comps::Transform>)
        return transforms;
    if constexpr (std::is_same_v<T, Comps::Camera>)
        return cameras;
    if constexpr (std::is_same_v<T, Comps::VisualManager<EnumData::BLOCKS>>)
        return block_sprites;
    if constexpr (std::is_same_v<T, Comps::ChunkGenerator>)
        return chunks;
    if constexpr (std::is_same_v<T, Comps::PhysicsStatic>)
        return p_grounds;
    if constexpr (std::is_same_v<T, Comps::PhysicsEntity>)
        return p_dyna_bodies;
}
template<WorldComponent T, typename... Args>
EntityBuilder& EntityBuilder::with(Args... args) {
    ws.get_storage_of_component<T>().emplace(id, T{std::forward<Args>(args)...});
    return *this;
}
template<WorldComponent T>
    T& WorldStorage::get(Entity id) { return get_storage_of_component<T>().data_of(id); }

