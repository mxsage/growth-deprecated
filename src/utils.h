#pragma once

#include <Eigen/Dense>
#include <string>
#include <cassert>

using Vec3=Eigen::Vector3d;

static const std::string outputPath = "parameters/";

inline void throw_runtime_error(std::string msg){
    throw std::runtime_error(msg);
}
