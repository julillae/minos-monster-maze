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
	//RenderManager::draw(projection, m_position, m_rotation, m_scale, &texture, this);

    transform_begin();

    transform_translate(m_position);
    transform_rotate(m_rotation);
    transform_scale(m_scale);

    transform_end();

    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Getting uniform locations
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

    // Setting vertices and indices
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
    glEnableVertexAttribArray(in_position_loc);
    glEnableVertexAttribArray(in_color_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

    float color[] = { 0.8, 0.15, 0.15 };
    glUniform3fv(color_uloc, 1, color);

    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);


    // Drawing!
    glDrawElements(GL_TRIANGLES,(GLsizei)m_num_indices, GL_UNSIGNED_SHORT, nullptr);

}

void Spikes::set_dimensions()
{
    float min_x = 0.f;
    float max_x = 0.f;
    float min_y = 0.f;
    float max_y = 0.f;
    for (auto vert : vertices)
    {
        float vert_x = vert.position.x;
        if (min_x == 0.f || vert_x < min_x) min_x = vert_x;
        if (max_x == 0.f || vert_x > max_x) max_x = vert_x;

        float vert_y = vert.position.y;
        if (min_y == 0.f || vert_y < min_y) min_y = vert_y;
        if (max_y == 0.f || vert_y > max_y) max_y = vert_y;

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