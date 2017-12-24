#pragma once

#include "utils.h"
#include "particle.h"
#include <deque>
#include <vector>
#include <iostream>
#include <algorithm>

class Icosahedron {
public:
    Icosahedron(float _config_radius);
    ~Icosahedron();

    /* takes a pointer to the cell simulation */
    std::vector<Particle*> create_sim();

private:
    float config_radius;
    int pop;

    std::vector<Particle*> cells;
    Particle make_particle(int);
    std::vector<Vec3> icosa_vertices();
    void subdivide_iteration(std::deque<Vec3>* vertices);
    std::vector<Vec3> subdivided_icosahedron(int levels);
    void init_springs(float radius);
};
