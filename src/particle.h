#pragma once

#include "params.h"
#include "utils.h"
#include <list>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <array>
#include <limits>

class Particle
{
public:
    std::vector<Particle*> links;
    const int index;

    int age         = 0;    // age of particle
    int generation  = 0;

    int box         = -1;   // box index in the grid

    double food      = 0;
    double area      = 0;    // sum of area of adjacent triangles
    double curvature = 0;
    double inherited = 0;
    double tx, ty;
    Vec3 position, delta, normal, planar_target, color, collision_target;

    int collisions      = 0;            // set in the GPU kernel
    bool visited        = false;        // visited in the face set
    bool frozen         = false;        // if frozen, doesn't update
    bool environs       = false;
    bool special        = false;
    bool special_baby   = false;
    bool ordered        = false;

    Particle(int _index);
    ~Particle();

    void print();

    int get_num_links() const;
    Vec3 get_position() const;

    /* update the position of the particle and reset delta */
    void update(const double dampening);

    /* check if these particles are connected (by address) */
    bool connected_to(Particle* p) const;

    /* add link with this particle */
    void add_link(Particle* p);

    /* remove a link with this particle */
    void remove_link(Particle* p);

    /* get next particle, any direction, from p */
    Particle* get_next(const Particle* p);

    /* get next particle in ring, given p and q */
    Particle* get_next(const Particle* p_0, const Particle* p_1);

    /* order links (one-ring neighborhood) */
    void order_neighbors();

    /* calculate normal (avg of one neighborhood cross products) */
    void calculate_normal();

    /* is this particle connected to a frozen particle? */
    bool connected_to_frozen();

    /* does this particle have a good loop? */
    bool good_loop();

    /* calculate avg position of neighbors */
    void calculate_planar_target();

    /* calculate custom curvature metric (see paper) */
    void calculate_curvature();

    /* calculate and add all forces */
    void calculate(const double spring_factor, const double planar_factor,
                   const double bulge_factor, const double spring_length);

    /* add spring displacement to delta given params */
    void add_spring_force(const double spring_factor, const double spring_length);

    /* add planar force to delta given params */
    void add_planar_force(const double planar_factor);

    /* add bulge force to delta given params */
    void add_bulge_force(const double bulge_factor, const double spring_length);

    /* add collision force given params */
    void add_collision_force(const double repulsion_factor, const double collision_radius);

    /* set links of the baby and current particle */
    void set_links(Particle* baby, const bool longest);

    /* set positions of baby and parent to reflect average positions */
    void set_baby_parent_positions(Particle* baby);

    /* find shortest splitting axis */
    int find_shortest_axis();

    /* find longest splitting axis */
    int find_longest_axis();

    /* create baby and set appropriate values */
    void create_baby(Particle* baby);

    /* split particle and create a baby */
    void split(Particle* baby, const bool longest);
};
