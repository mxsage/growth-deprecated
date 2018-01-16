#pragma once

#include <Eigen/Dense>
#include <string>

using Vec3 = Eigen::Vector3d;

static const std::string outputPath = "/Users/sagejenson/dev/growth/";

inline void throw_runtime_error(const std::string msg){
    throw std::runtime_error(msg);
}
