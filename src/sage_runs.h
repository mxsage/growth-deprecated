#pragma once

#include "params.h"

using namespace parameters;

inline Parameters current()
{
    Parameters p;
    p.spring_factor = 0.5;
    p.spring_length = 0.1;
    p.planar_factor = 0.99;
    p.bulge_factor = 0.01;
    p.collision_factor = 0.5;
    p.collision_radius = 0.1;
    p.threshold = 10.5;
    p.max_degree = 8;

    p.food_mode = Food::AREA;
    p.split_mode = Split::LONG;
    p.init_shape = Shape::OCTAHEDRON;

    p.collision_radius = 0.4;

    return p;
}

inline Parameters parameter_generator()
{
    Parameters p = current();

    p.write_cell_radius = false;
    p.write_faces = true;
    p.save_frequency = 10;
    return p;
}
