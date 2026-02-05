#include "storage.hpp"

using namespace Comps;

// TODO: make gen_entity_hitboxes()
PhysicsEntity::PhysicsEntity(b2World* world, b2Vec2 box_shape, b2Vec2 init_pos) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(init_pos.x, init_pos.y);
    body_def.fixedRotation = true;
    body_def.bullet = true;

    b2PolygonShape body_shape;
    body_shape.SetAsBox(box_shape.x / 2.0f, box_shape.y / 2.0f);

    b2FixtureDef body_fix;
    body_fix.shape = &body_shape;
    body_fix.density = 1.0f;
    body_fix.friction = 0.2f;

    p_body = world->CreateBody(&body_def);
    p_body->CreateFixture(&body_fix);
}

EntityBuilder WorldStorage::create_entity() {
    it+=1;
    return EntityBuilder{*this, it};
}

b2World* WorldStorage::p_world() {
    return physics_world.get();
}

void WorldStorage::prepare() {
    for (size_t i = 0; i < cameras.data_size(); ++i) {
        Comps::Camera& camera = cameras.data_at(i);
        camera.canvas = std::make_unique<sf::RenderTexture>(K::WIN_SIZE);
        camera.canvas->clear(sf::Color::Transparent);
    }
}
void WorldStorage::load_block_sprites(VisualManager<EnumData::BLOCKS>& blocks) {
    sf::Texture atlas;
    if (!atlas.loadFromFile("res\\texture\\block_list.png"))
        throw std::runtime_error("Failed to load block_list.png");
    atlas.setSmooth(false);
    blocks.atlas = std::move(atlas);
    for (int i = 1; i < static_cast<int>(EnumData::BLOCKS::COUNT); ++i)
        blocks.visuals[i] = {{16 * (i-1), 0}, {16,16}};
}
