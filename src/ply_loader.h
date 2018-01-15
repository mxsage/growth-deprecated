#pragma once

#include <igl/readPLY.h>
#include <string>
#include <Eigen/Dense>

#include "utils.h"
#include "particle.h"

class PlyLoader
{
public:
    PlyLoader(const std::string filename);
    ~PlyLoader(){};

    std::vector<Particle*> create_sim();

private:
    void connect(const size_t i, const size_t j);
    const std::string m_filename;
    std::vector<Particle*> cells;
};
