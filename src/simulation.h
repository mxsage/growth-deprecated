#pragma once

#define USE_VIEWER

#include <iostream>
#include <vector>
#include <deque>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <stdexcept>
#include <memory>
#include <chrono>

#include "utils.h"
#include "params.h"
#include "geometry.h"
#include "grid.h"
#include "particle.h"
#include "kdtree.h" 

using namespace parameters;
using namespace jk;

class Simulation {
public:
    Simulation();
    ~Simulation();

    void init(Parameters _param);

    Parameters param;
    int frame_num = 0;
    int frozen_num = 0;

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
	Eigen::MatrixXd N;
    void set_matrices();

	void set_parameters(const Parameters& _param);

    /* hold all the cells */
    std::vector<Particle*> cells;

    /* return pointer to cells */
    std::vector<Particle*>* get_cells();

    /* get position for cell i */
    Vec3* get_pos(int i);

    /* return the current population */
    int get_pop();

    /* add nutrients and split */
    void add_food();
    void split();

    /* freeze edge particles */
    void freeze_bad_particles();

    /* add non-collision forces() */
    void add_cpu_forces();

    /* move delta */
    void update_position();

    /* Calculate collisions */
    void collision_grid();
	void collision_tree();

    /* Calculate collisions (brute force)*/
    void brute_force_collision();

    /* tick the simulation forward */
    void update();

    /* print out list of points */
    void point_list();

    /* grid creation helper */
    Grid* make_grid();
};
