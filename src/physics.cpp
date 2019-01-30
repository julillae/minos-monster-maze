#include "../include/physics.hpp"

// logic for gravity and potentially friction calculations go here

Physics::Physics() = default;

Physics::~Physics() = default;


bool Physics::collisionWithEnemy (Player *c, const Turtle *t) {
    float dx = c->get_position().x - t->get_position().x;
    float dy = c->get_position().y - t->get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(t->get_bounding_box().x, t->get_bounding_box().y);
    float my_r = std::max(c->get_scale().x, c->get_scale().y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}

bool Physics::collisionWithFixedWalls(Player *p, FixedComponent *m) {
    float dx = p->get_position().x - m->get_position().x;
    float dy = p->get_position().y - m->get_position().y;
    float d_sq = dx * dx + dy * dy;
    float other_r = std::max(m->get_bounding_box().x, m->get_bounding_box().y);
    float my_r = std::max(p->get_scale().x, p->get_scale().y);
    float r = std::max(other_r, my_r);
    r *= 0.6f;
    if (d_sq < r * r)
        return true;
    return false;
}


bool Physics::collisionWithFish (Salmon *s, Fish *f) {
        float dx = s->get_position().x - f->get_position().x;
        float dy = s->get_position().y - f->get_position().y;
        float d_sq = dx * dx + dy * dy;
        float other_r = std::max(f->get_bounding_box().x, f->get_bounding_box().y);
        float my_r = std::max(s->get_scale().x, s->get_scale().y);
        float r = std::max(other_r, my_r);
        r *= 0.6f;
        if (d_sq < r * r)
            return true;
        return false;
}

//bool Physics::collisionWithTurtle(Salmon *s, const Turtle *t) {
//
//    float dx = s->get_position().x - t->get_position().x;
//	float dy = s->get_position().y - t->get_position().y;
//	float d_sq = dx * dx + dy * dy;
//	float other_r = std::max(t->get_bounding_box().x, t->get_bounding_box().y);
//	float my_r = std::max(s->get_scale().x, s->get_scale().y);
//	float r = std::max(other_r, my_r);
//	r *= 0.6f;
//	if (d_sq < r * r)
//		return true;
//	return false;
//
//    }

