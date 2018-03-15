#include "grid.h"

Grid::Grid(Vec3 _max, Vec3 _min, float radius) :
    box_num(0)
{
    float epsilon = 0.01;
    radius += std::max(epsilon, radius);
    block_size = radius;
    min = _min - Vec3 (radius, radius, radius);
    _max -= min;
    double d = std::max({_max.x(), _max.y(), _max.z()});
    d /= block_size;
    d = std::ceil(d);

    dim = (bint) d + 1;

    if (std::pow(dim, 3) > ULLONG_MAX)
    {
        throw_runtime_error("Out of bounds in the grid.");
    }
}

inline bool Grid::in_bounds(const bint &index)
{
    return index <= dim*dim*dim;
}

inline bool Grid::in_bounds(const coord &c)
{
    return  c.x >= 0 and
        c.y >= 0 and
        c.z >= 0 and
        (unsigned int) c.x < dim and
        (unsigned int) c.y < dim and
        (unsigned int) c.z < dim;
}

void Grid::add_point(Vec3 position, int p_idx)
{
    bint index = get_index(position);
    if (grid.find(index) == grid.end())
    {
        std::list<int> l;
        grid[index] = l;
        box_num++;
    }

    grid[index].push_back(p_idx);
}

bint Grid::get_index(Vec3 v)
{
    v -= min;

    int x = std::floor(v.x() / block_size);
    int y = std::floor(v.y() / block_size);
    int z = std::floor(v.z() / block_size);

    coord c(x, y, z);

    return coord_to_index(c);
}

bint Grid::coord_to_index(const coord& c)
{
    return (bint) c.x + c.y*dim + c.z*dim*dim;
}

coord Grid::index_to_coord(bint index)
{
    if (not in_bounds(index))
    {
        throw_runtime_error("Grid index throw_runtime_error.");
    }
    int x = index % dim;
    int y = ((index - x) % (dim*dim))/dim;
    int z = (index - x - (y*dim))/(dim*dim);

    coord c(x, y, z);

    return c;
}

std::list<int> Grid::get_neighbors(const Vec3& v)
{
    std::list<int> neighbors;
    bint index = get_index(v);
    coord c = index_to_coord(index);

    if (not in_bounds(index))
    {
        throw_runtime_error("Particle out of bounds.");
    }

    for (int i=-1; i<=1; i++)
    {
        for (int j=-1; j<=1; j++)
        {
            for (int k=-1; k<=1; k++)
            {
                coord new_coord(c.x + i, c.y + j, c.z + k);

                if (in_bounds(new_coord))
                {
                    bint new_index = coord_to_index(new_coord);
                    if (grid[new_index].size() > 0)
                    {
                        neighbors.insert(neighbors.end(),
                                grid[new_index].begin(),
                                grid[new_index].end());
                    }
                }
            }
        }
    }
    return neighbors;
}

std::list<int> Grid::get_neighbors(Particle* p)
{
    return get_neighbors(p->position);
}

void Grid::set_box(Particle* p)
{
    p->box = box_map[get_index(p->position)];
}

void Grid::create_sorted_list(int* sorted, Box* box_arr)
{
    // current position in the sorted list
    int current_pos = 0;
    int num_boxes = 0;

    // iterate through all of the possible cells
    for (unsigned int k=0; k<dim; k++)
    {
        for (unsigned int j=0; j<dim; j++)
        {
            for (unsigned int i=0; i<dim; i++)
            {
                coord c(i, j, k);
                bint idx = coord_to_index(c);

                // if the cell is populated create a box
                if (grid[idx].size() > 0)
                {
                    box_map[idx] = num_boxes;
                    box_arr[num_boxes].begin = current_pos;
                    box_arr[num_boxes].length = grid[idx].size();
                    for (std::list<int>::iterator it = grid[idx].begin(); it !=grid[idx].end(); ++it)
                    {
                        sorted[current_pos] = (*it);
                        current_pos++;
                    }
                    num_boxes++;
                }
            }
        }
    }

    if (num_boxes != box_num)
    {
        throw_runtime_error("Number of boxes inconsistent.");
    }
}

// TODO fix this garbage
void Grid::fill_box_array(Box* box_arr)
{
    for (unsigned int z=0; z<dim; z++)
    {
        for (unsigned int y=0; y<dim; y++)
        {
            for (unsigned int x=0; x<dim; x++)
            {
                coord c(x, y, z);

                bint grid_idx = coord_to_index(c);
                if (box_map.find(grid_idx) !=box_map.end())
                {
                    int cur_idx = 0;
                    int box_index = box_map[grid_idx];

                    for (int i=0; i<27; i++)
                    {
                        box_arr[box_index].neighbors[i] = -1;
                    }

                    for (int i=-1; i<=1; i++)
                    {
                        for (int j=-1; j<=1; j++)
                        {
                            for (int k=-1; k<=1; k++)
                            {
                                coord new_coord(c.x + i, c.y + j, c.z + k);

                                bint new_index = coord_to_index(new_coord);
                                if (in_bounds(new_coord) && (box_map.find(new_index) != box_map.end()))
                                {
                                    box_arr[box_index].neighbors[cur_idx] = box_map[new_index];
                                    cur_idx++;
                                }
                            }
                        }
                    }

                    if (cur_idx > 27)
                    {
                        throw_runtime_error("Over 27 neighbors!!?");
                    }
                }
            }
        }
    }
}

void Grid::create_box_list(int* sorted, Box* box_arr)
{
    create_sorted_list(sorted, box_arr);
    fill_box_array(box_arr);
}
