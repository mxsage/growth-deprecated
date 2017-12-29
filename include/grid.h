#pragma once

#include "utils.h"
#include "particle.h"

#include <list>
#include <map>
#include <cmath>
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <cassert>

typedef struct coord {
    int x, y, z;
    coord(){}
    coord(int a, int b, int c) : x(a), y(b), z(c) {}
} coord;

typedef struct Box {
    int begin;
    int length;
    int neighbors[27];
} Box;

typedef unsigned long long bint;

class Grid{
public:
    double block_size;
    bint dim;
    int box_num;
    Vec3 min;
    std::map<bint, std::list<int> > grid;
    std::map<bint, int> box_map;

    Grid(Vec3 _max, Vec3 _min, float radius);
    ~Grid(){};

    inline bool in_bounds(const bint &index);
    inline bool in_bounds(const coord &c);

    void add_point(Vec3 position, int p_idx);

    bint get_index(Vec3 v);

    coord index_to_coord(bint index);

    bint coord_to_index(const coord& c);

    std::list<int> get_neighbors(const Vec3& v);

    std::list<int> get_neighbors(Particle* p);

    /* Creates a list of boxes from the existing grid. */
    void create_sorted_list(int* arr, Box* box_arr);

    /*  Write an array of boxes that will point to the sorted list.
        Relies on the box_map to be accurate. */
    void fill_box_array(Box* box_arr);

    /* Create an array of boxes and a sorted list of all the grid cells. */
    void create_box_list(int* arr, Box* box_arr);

    void set_box(Particle* p);
};
