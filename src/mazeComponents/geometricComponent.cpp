#include "../include/mazeComponents/geometricComponent.hpp"

void GeometricComponent::set_dimensions()
{
    float min_x;
    float max_x;
    float min_y;
    float max_y;

    for (size_t i = 0; i < local_vertex_coords.size(); i++) {
        float vert_x = local_vertex_coords[i].x;
        float vert_y = local_vertex_coords[i].y;

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

