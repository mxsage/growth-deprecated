#include "ply_loader.h"

PlyLoader::PlyLoader(const std::string filename) :
    m_filename(filename)
{}

void PlyLoader::connect(const Eigen::Index i, const Eigen::Index j)
{
    if (!cells[i]->connected_to(cells[j]))
    {
        cells[i]->add_link(cells[j]);
    }

    if (!cells[j]->connected_to(cells[i]))
    {
        cells[j]->add_link(cells[i]);
    }
}

std::vector<Particle*> PlyLoader::create_sim()
{
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::MatrixXd N;
    Eigen::MatrixXd UV;

    if (!igl::readPLY(m_filename, V, F, N, UV))
    {
        throw_runtime_error("Couldn't load ply from [" + m_filename + "]!");
    }

    Eigen::Index pop = 0;
    for (Eigen::Index i = 0; i < V.rows(); ++i)
    {
        Particle* p = new Particle(pop);
        p->position = Eigen::Vector3d(V(i, 0), V(i, 1), V(i, 2));

        if (N.rows() != 0)
        {
            p->normal = Eigen::Vector3d(N(i, 0), N(i, 1), N(i, 2));
            p->normal.normalize();
        }
        else
        {
            p->normal = p->position.normalized();
        }

        cells.push_back(p);
        pop++;
    }

    for (auto i = 0; i < F.rows(); ++i)
    {
        connect(F(i, 0), F(i, 1));
        connect(F(i, 0), F(i, 2));
        connect(F(i, 1), F(i, 2));
    }

    return cells;
}
