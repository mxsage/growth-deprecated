#include <igl/readPLY.h>
#include <string>

#include "utils.h"
#include "particle.h"

class PlyLoader
{
public:
    PlyLoader(const std::string filename);
    ~PlyLoader(){};

    std::vector<Particle*> create_sim();

private:
    void connect(const Eigen::Index i, const Eigen::Index j);
    const std::string m_filename;
    std::vector<Particle*> cells;
};
