

template<typename T>
template<typename... Args>
T& Engine::PackedStorage<T>::emplace(Entity e, Args&&... args) {
    if (this->has_entity(e)) 
        return data_of(e);
            
    size_t index = data.size();
    data.emplace_back(std::forward<Args>(args)...);
    entities.push_back(e);
    lookup[e] = index;
    return data.back();
}

template<typename T>
bool Engine::PackedStorage<T>::has_entity(Entity e) const { 
    return lookup.contains(e); 
}
template<typename T>
T& Engine::PackedStorage<T>::data_of(Entity e) { 
    return data[lookup[e]]; 
}
template<typename T>
Entity& Engine::PackedStorage<T>::entity_of(const T& c) { 
    return entities[&c - data.data()]; 
}
template<typename T>
size_t Engine::PackedStorage<T>::data_size() const {
    return data.size();
}
template<typename T>
auto Engine::PackedStorage<T>::data_begin() {
    return data.begin(); 
}
template<typename T>
auto Engine::PackedStorage<T>::data_end() {
    return data.end(); 
}