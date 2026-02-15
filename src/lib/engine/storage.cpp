#include <engine/engine.hpp>

Engine::StorageComponents::PhysicsDynamic::PhysicsDynamic(b2World* world, b2Vec2 box_shape, b2Vec2 init_pos) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(
        init_pos.x,
        init_pos.y
    );
    body_def.fixedRotation = true;
    body_def.bullet = false;

    b2PolygonShape body_shape;
    body_shape.SetAsBox(box_shape.x / 2.0f, box_shape.y/2.0f);

    b2FixtureDef body_fix;
    body_fix.shape = &body_shape;
    body_fix.density = 1.0f;
    body_fix.friction = 0.5f;

    p_body = world->CreateBody(&body_def);
    p_body->CreateFixture(&body_fix);
    p_body->SetSleepingAllowed(false);
}