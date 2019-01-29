#include "../include/physics.hpp"

// logic for gravity and potentially friction calculations go here

Physics::Physics() = default;

Physics::~Physics() = default;


bool Physics::collisionWithEnemy (Character *c, Enemy *e) {
    // TODO
    return true;
}

bool Physics::collisionWithWalls (Character *c, FixedComponent *f) {
    // TODO

    return true;
}

bool Physics::collisionWithFish (Salmon *s, Fish *f) {
        float dx = s->m_position.x - f->get_position().x;
        float dy = s->m_position.y - f->get_position().y;
        float d_sq = dx * dx + dy * dy;
        float other_r = std::max(f->get_bounding_box().x, f->get_bounding_box().y);
        float my_r = std::max(s->m_scale.x, s->m_scale.y);
        float r = std::max(other_r, my_r);
        r *= 0.6f;
        if (d_sq < r * r)
            return true;
        return false;
}

bool Physics::collisionWithTurtle(Salmon *s, const Turtle *t) {

    float dx = s->m_position.x - t->get_position().x;
	float dy = s->m_position.y - t->get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(t->get_bounding_box().x, t->get_bounding_box().y);
	float my_r = std::max(s->m_scale.x, s->m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;

    }

