#pragma once

#include "utils.h"
#include "params.h"
#include "particle.h"
#include "icosahedron.h"
#include "octahedron.h"
#include "ply_loader.h"

#include <vector>

using namespace parameters;

class Geometry {
public:
    Geometry();
    std::vector<Particle*> create_geometry(Parameters);
    ~Geometry();
};
