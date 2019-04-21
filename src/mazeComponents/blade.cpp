#include "../../include/mazeComponents/blade.hpp"

#include <string>
#include <algorithm>

bool Blade::init(vec2 position)
{
    // Reads the spikes mesh from a file, which contains a list of vertices and indices
    FILE* mesh_file = fopen(mesh_path("blade.mesh"), "r");
    if (mesh_file == nullptr)
        return false;

    // Reading vertices and colors
    size_t num_vertices;
    fscanf(mesh_file, "%zu\n", &num_vertices);
    for (size_t i = 0; i < num_vertices; ++i)
    {
        float x, y, z;
        float _u[3]; // unused
        int r, g, b;
        fscanf(mesh_file, "%f %f %f %f %f %f %d %d %d\n", &x, &y, &z, _u, _u+1, _u+2, &r, &g, &b);
        Vertex vertex;
        vertex.position = { x, y, -z };
        vertex.color = { (float)r / 255, (float)g / 255, (float)b / 255 };
        vertices.push_back(vertex);
        local_vertex_coords.push_back(vertex.position);

    }

    // Reading associated indices
    size_t num_indices;
    fscanf(mesh_file, "%zu\n", &num_indices);
    for (size_t i = 0; i < num_indices; ++i)
    {
        int idx[3];
        fscanf(mesh_file, "%d %d %d\n", idx, idx + 1, idx + 2);
        indices.push_back((uint16_t)idx[0]);
        indices.push_back((uint16_t)idx[1]);
        indices.push_back((uint16_t)idx[2]);
    }

    // Done reading
    fclose(mesh_file);

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Index Buffer creation
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Vertex Array (Container for Vertex + Index buffer)
    glGenVertexArrays(1, &mesh.vao);
    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl")))
        return false;

    // Setting initial values
    m_scale.x = 10.f;
	m_scale.y = 10.f;
    m_rotation = 0.f;
    m_num_indices = indices.size();
    m_position = position;
	drag = 0.75f;
	can_kill = true;
    set_rotation(0);
    set_dimensions();
    remove_tether_coord();
    set_rotation_properties();
    set_collision_properties();

    return true;
}


void Blade::draw(const mat3& projection)
{
    float color[] = { 1.0, 1.0, 1.0 };
    vec2 rotation_point = vec2({m_position.x, m_position.y - m_height / 2});
    vec2 positionDiff = vec2({0.f, m_height / 2});

    transform_begin();
    transform_translate(rotation_point);
    transform_rotate(m_rotation);
    transform_translate(positionDiff);
    transform_scale(m_scale);
    transform_end();

    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Setting vertices and indices
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

    RenderManager::draw_mesh(projection, this, color, m_num_indices);

}

void Blade::set_pendulum_rot()
{
    float currRotation = get_rotation();

    if (currRotation > max_rotation || currRotation < min_rotation) {
        dr = -dr;
    }

    currRotation = currRotation + dr;
    set_rotation(currRotation);

    transform_begin();
    transform_translate(rotation_point);
    transform_rotate(m_rotation);
    transform_translate(positionDiff);
    transform_scale(m_scale);
    transform_end();

    transform_vertex_coord();
}

void Blade::set_rotation_properties()
{
    rotation_point = vec2({m_position.x, m_position.y - m_height / 2});
    positionDiff = vec2({0.f, m_height / 2});
}

void Blade::remove_tether_coord()
{
    for (int i = 0; i < 4; i++) {
        local_vertex_coords.pop_back();
    }
}

bool Blades::spawn_blades(vec2 position)
{
    Blade sw;

    if (sw.init(position))
    {
        m_blades.emplace_back(sw);
        return true;
    }
    fprintf(stderr, "Failed to spawn blades");
    return false;
}

std::vector<Blade> Blades::get_blade_vector()
{
	return m_blades;
}

void Blades::draw(const mat3 & projection)
{
	for (auto& sw : m_blades)
		sw.draw(projection);
}

void Blades::update()
{
    if (!isFrozen) {
        for (auto& sw : m_blades)
            sw.set_pendulum_rot();
    }
}

void Blades::setBladeProperties(size_t index, float rotation)
{
    m_blades[index].set_rotation(rotation);
}


void Blades::destroy()
{
	for (auto& sw : m_blades)
		sw.destroy();
	m_blades.clear();
}

void Blades::freeze()
{
    isFrozen = true;
}

void Blades::unfreeze()
{
    isFrozen = false;
}

bool Blades::renderSetup() { return false; }
