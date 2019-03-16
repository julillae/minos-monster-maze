#pragma once

#include <vector>
#include "../common.hpp"
#include "../renderManager.hpp"

class MazeComponent : public Renderable
{
public:

    virtual bool init(vec2 position) = 0;

	// Releases all associated resources
	void destroy();

	virtual void draw(const mat3& projection) = 0;

	void set_size(vec2 size);

	// Sets the new position
	void set_position(vec2 position);

	// Set component rotation in radians
	void set_rotation(float radians);

	vec2 get_position()const;

	void set_scale(vec2 scale);

	float get_rotation()const;

	float get_width();
	
    float get_height();

    vec2 get_bounding_box()const;

    float get_drag();

    void set_vertex_coord();
	std::vector<vec2> get_vertex_coord();

    bool can_kill = false;
protected:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
	float m_width;
	float m_height;
    float drag;
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    std::vector<vec2> vertex_coords;
};