#pragma once

#include "utils.h"
#include "particle.h"
#include <vector>

class Octahedron
{
public:
    Octahedron (float _len);
    ~Octahedron ();

    /* takes a pointer to the cell simulation */
    std::vector<Particle*> create_sim();

private:
    float len;
};
