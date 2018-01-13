#include "geometry.h"

Geometry::Geometry(){}

std::vector<Particle*> Geometry::create_geometry(Parameters param)
{
    std::vector<Particle*> cells;
    if (param.init_shape == Shape::OCTAHEDRON)
    {
        Octahedron oct(1);
        cells = oct.create_sim();
    }

    else if (param.init_shape == Shape::PLY)
    {
        PlyLoader ply(outputPath + "models/" + param.filename);
        cells = ply.create_sim();
    }

    return cells;
}

Geometry::~Geometry(){}
