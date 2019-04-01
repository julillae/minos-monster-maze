#pragma once

// Simple interface for the spider enemies. These are 'simple' enemies that patrol back and forth
#include "enemy.hpp"
#include "character.hpp"
#include "simple.hpp"

class Spider: public Simple
{
    static Texture spider_texture;
    static Texture b_texture;
    public:
        // Creates all the associated render resources and default transform
        bool init(vec2 initialPosition, Physics* physicsHandler)override;

        void draw(const mat3& projection)override;

        void set_animation()override;
	
};

class Spiders : public Simples
{
public:
	bool spawn_spider_enemy(vec2 position, float bound, bool upsideDown);
	std::vector<Spider> get_spider_vector();
	// Renders the component
	void draw(const mat3& projection)override;
	void reset();
	void freeze();
	void unfreeze();
	void update(float elapsed_ms);
	void setSpiderProperties(size_t index, vec2 position, vec2 velocity, vec2 scale);

	void destroy();
private:
	std::vector<Spider> m_spiders;
};