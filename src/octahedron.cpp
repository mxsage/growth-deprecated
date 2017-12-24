#include "octahedron.h"


Octahedron::Octahedron(float _len):
    len(_len)
{}

Octahedron::~Octahedron(){};

std::vector<Particle*> Octahedron::create_sim()
{
    std::vector<Particle*> cells;
    Vec3 points[] = {
        Vec3(0, 0, 1),
        Vec3(0, 1, 0),
        Vec3(1, 0, 0),
        Vec3(0, -1, 0),
        Vec3(-1, 0, 0),
        Vec3(0, 0, -1)
    };

    int pop = 0;
    for (auto& p : points)
    {
        cells.push_back(new Particle(pop));
        cells[pop]->position = p;
        pop++;
    }

    for (auto& p : cells)
    {
        p->normal = p->position.normalized();
        for (auto& q : cells)
        {
            if (p != q)
            {
                p->add_link(q);
            }
        }
    }

    cells[0]->remove_link(cells[5]);
    cells[5]->remove_link(cells[0]);

    cells[1]->remove_link(cells[3]);
    cells[3]->remove_link(cells[1]);

    cells[2]->remove_link(cells[4]);
    cells[4]->remove_link(cells[2]);

    return cells;
}
