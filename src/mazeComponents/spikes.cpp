#include "../include/mazeComponents/spikes.hpp"

#include <string>
#include <algorithm>

bool Spikes::init(vec2 position)
{


    // Reads the spikes mesh from a file, which contains a list of vertices and indices
    FILE* mesh_file = fopen(mesh_path("spike.mesh"), "r");
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
    m_scale.y = 25.f;
    m_rotation = 0.f;
    m_num_indices = indices.size();
    m_position = position;
    can_kill = true;
	drag = 0.75f;
    set_dimensions();

    return true;
}


void Spikes::draw(const mat3& projection)
{
    float color[] = { 0.8, 0.15, 0.15 };
    RenderManager::draw_mesh(projection, m_position, m_rotation, m_scale, this,
                             color, m_num_indices);

}

void Spikes::set_dimensions()
{
    float min_x ;
    float max_x;
    float min_y;
    float max_y;

    for (int i = 0; i < vertices.size(); i++) {
        float vert_x = vertices[i].position.x;
        float vert_y = vertices[i].position.y;

        if (i == 0)
        {
            min_x = vert_x;
            max_x = vert_x;
            min_y = vert_y;
            max_y = vert_y;
        }

        if (vert_x < min_x) min_x = vert_x;
        if (vert_x > max_x) max_x = vert_x;
        if (vert_y < min_y) min_y = vert_y;
        if (vert_y > max_y) max_y = vert_y;

    }

    m_width = (max_x - min_x) * m_scale.x;
    m_height = (max_y - min_y) * m_scale.y;
}

void Spikes::set_left()
{
    set_rotation(67.5);
}

void Spikes::set_right()
{
    set_rotation(-67.5);
}

void Spikes::set_down()
{
    set_rotation(180.f);
}