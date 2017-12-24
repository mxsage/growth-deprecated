#include "icosahedron.h"

Icosahedron::Icosahedron(float _config_radius):
    config_radius(_config_radius),
    pop(0)
{}

Icosahedron::~Icosahedron(){};

std::vector<Particle*> Icosahedron::create_sim()
{
    std::vector<Vec3> points =  subdivided_icosahedron(2);
    for (unsigned int i=0; i<points.size(); i++)
    {
        cells.push_back(new Particle(pop));
        Vec3 v = points[i];
        v.normalize();
        cells.back()->position = v;
        cells.back()->normal = v;
        pop++;
    }

    init_springs(.4);

    for (int i=0; i<pop; i++)
    {
        cells[i]->position *= config_radius;
    }
    return cells;
}

// Sphere creation functions
std::vector<Vec3> Icosahedron::icosa_vertices()
{
    std::vector<Vec3> vertices(12);

    double theta = 26.56505117707799 * M_PI / 180.0;

    double stheta = sin(theta);
    double ctheta = cos(theta);

    vertices[0] = Vec3(0, 0, 1);

    // the lower pentagon
    double phi = M_PI / 5.0;
    for (int i = 1; i < 6; ++i)
    {
        vertices[i] = Vec3(ctheta * cos(phi), ctheta * sin(phi), -stheta);

        phi += 2.0 * M_PI / 5.0;
    }

    // the upper pentagon
    phi = 0.0;
    for (int i = 6; i < 11; ++i)
    {
        vertices[i] = Vec3(ctheta * cos(phi), ctheta * sin(phi), stheta);

        phi += 2.0 * M_PI / 5.0;
    }

    vertices[11] = Vec3(0,0,1); // the upper vertex

    return vertices;
}

void Icosahedron::subdivide_iteration(std::deque<Vec3>* vertices)
{
    int original_size = vertices->size();

    for (int i = 0; i < original_size; i += 3)
    {
        Vec3 v1 = vertices->front();
        vertices->pop_front();
        Vec3 v2 = vertices->front();
        vertices->pop_front();
        Vec3 v3 = vertices->front();
        vertices->pop_front();

        Vec3 v4 = (v1 + v2) / 2.0;
        Vec3 v5 = (v2 + v3) / 2.0;
        Vec3 v6 = (v3 + v1) / 2.0;

        vertices->push_back(v1);
        vertices->push_back(v4);
        vertices->push_back(v6);

        vertices->push_back(v4);
        vertices->push_back(v2);
        vertices->push_back(v5);

        vertices->push_back(v6);
        vertices->push_back(v5);
        vertices->push_back(v3);

        vertices->push_back(v6);
        vertices->push_back(v4);
        vertices->push_back(v5);
    }
}

std::vector<Vec3 > remove_duplicates(std::deque<Vec3 >* dup_list)
{
    std::vector<Vec3 > unique_list;
    while (not dup_list->empty())
    {
        Vec3 p = dup_list->front();
        dup_list->pop_front();
        unique_list.push_back(p);
        std::deque<Vec3 >::iterator it;

        for (it =dup_list->begin(); it !=dup_list->end();)
        {
            if ((*it) == p)
            {
                it = dup_list->erase(it);
            } else
            {
                ++it;
            }
        }
    }

    return unique_list;
}

std::vector<Vec3 > Icosahedron::subdivided_icosahedron(int levels)
{
    std::vector<Vec3 > icosahedron = icosa_vertices();
    std::deque<Vec3 > vertices;

    int triangles[] =
    {0,2,1,0,3,2,0,4,3,0,5,4,0,1,5,1,2,7,2,3,8,3,4,9,4,5,10,5,1,6,1,
        7,6,2,8,7,3,9,8,4,10,9,5,6,10,6,7,11,7,8,11,8,9,11,9,10,11,10,6,11};

    for (int i=0; i<60; i++)
    {
        vertices.push_back(icosahedron[triangles[i]]);
    }

    for (int i = 0; i < levels; i++)
    {
        subdivide_iteration(&vertices);
    }


    std::vector<Vec3 > final_point_list = remove_duplicates(&vertices);
    return final_point_list;
}

void Icosahedron::init_springs(float radius)
{
    for (int i=0; i<pop; i++)
    {
        for (int j=0; j<pop; j++)
        {
            Vec3 disp = cells[i]->position - cells[j]->position;
            if ((!(i == j)) and (disp.squaredNorm() < radius*radius))
            {
                cells[i]->add_link(cells[i]);
            }
        }
    }
}
