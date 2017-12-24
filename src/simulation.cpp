// simulation.cu
// parallel implementation of cellular growth
// Sage Jenson, 2017

#include "simulation.h"

Simulation::Simulation(){}

void Simulation::init(Parameters _param)
{
    param = _param;
    cells.reserve(MAX_POP);

    Geometry g;
    cells = g.create_geometry(param);

    if (param.food_mode == Food::INHERIT)
    {
        for (auto& p : cells)
        {
            p->calculate_curvature();
            p->inherited = p->curvature;
        }
    }

    if (param.food_mode == Food::TENTACLE)
    {
        cells[0]->special = true;
        for (auto& p : cells)
        {
            p->generation = 99;
        }
    }
}


std::vector<Particle*>* Simulation::get_cells()
{
    return &cells;
}

Vec3* Simulation::get_pos(int i)
{
    return &(cells[i]->position);
}

int Simulation::get_pop()
{
    return cells.size();
}

void Simulation::set_matrices()
{
    V.resize(cells.size(), 3);
    for (auto& p : cells)
    {
        Vec3 pos = p->position;
        V(p->index, 0) = pos.x();
        V(p->index, 1) = pos.y();
        V(p->index, 2) = pos.z();
    }

    int num_faces = 0;
    for (auto& p : cells)
    {
        num_faces += p->links.size();
    }

    F.resize(num_faces, 3);
    int cur_face = 0;
    for (auto& p : cells)
    {
        for (int i=0; i<p->links.size(); i++)
        {
            Particle* c = p->links[i];
            Particle* b = p->links[(i+1)% (p->links.size())];
            F(cur_face, 0) = p->index;
            F(cur_face, 1) = b->index;
            F(cur_face, 2) = c->index;
            cur_face++;
        }
    }
}

void Simulation::update()
{
    std::cout << "Add Food. " << std::flush;
	add_food();

	std::cout << "Split." << std::flush;
    split();

    std::cout << "Collision.  " << std::flush;
    collision_grid();

    std::cout << "CPU forces. "<< std::flush;
    add_cpu_forces();

    std::cout << "Update. " << std::flush;
    update_position();

    std::cout << "Frame: " << frame_num;
    std::cout <<" Pop: " << cells.size() << ".\n" << std::endl;

    frame_num++;
}

void Simulation::freeze_bad_particles()
{
    for (auto& p : cells)
    {
        if (not (p->environs || p->frozen || p->good_loop()))
        {
            p->frozen = true;
        }
    }
}

void Simulation::update_position()
{
    frozen_num = 0;
    for (auto& p : cells)
    {
        if (p->frozen)
        {
            frozen_num++;
        } else
        {
            p->update(param.dampening);
        }
    }
}

void Simulation::add_cpu_forces()
{
    for (auto& p : cells)
    {
        if (param.init_shape == Shape::ENVIRONMENT)
        {
            if (p->environs || p->frozen)
            {
                continue;
            }
        }

        if (!p->good_loop())
        {
            p->frozen = true;
            continue;
        }

        // TODO fix frozen cell behavior...
        if (!p->frozen)
        {
            p->calculate(param.spring_factor, param.planar_factor,
                    param.bulge_factor, param.spring_length);
        }
    }
}

void Simulation::add_food()
{
    for (auto& p : cells)
    {
        if (p->environs || p->frozen)
        {
            p->food = 0;
            continue;
        } else
        {
            if (param.food_mode == Food::RANDOM)
            {
                p->food += (float) std::rand() / RAND_MAX;
            }
			else if (param.food_mode == Food::AREA)
            {
                p->food += p->area;
            }
			else if (param.food_mode == Food::X_COORD)
            {
                p->food += p->position.x() + 50;
            }
			else if (param.food_mode == Food::RADIAL)
            {
                float dist = p->position.norm();
                if (dist < 0.5)
                {
                    dist = 0.5;
                }
                dist = pow(dist, 2);
                p->food += 100.0/dist;
            }
			else if (param.food_mode == Food::COLLISIONS)
            {
                if (p->collisions > 0)
                {
					p->food += 1.0 / p->collisions;
                }
            }
			else if (param.food_mode == Food::CURVATURE)
            {
                float amount = p->curvature;
                p->calculate_curvature();
                for (Particle* q : p->links)
                {
                    q->calculate_curvature();
                    amount += q->curvature;
                }
                if ((!std::isnan(amount) && (amount > 0)))
                {
                    p->food += amount;
                }
            }
			else if (param.food_mode == Food::INHERIT)
            {
                p->food += p->inherited;
            }
			else if (param.food_mode == Food::HYBRID)
            {
                p->calculate_curvature();
                float amount = p->curvature;
                if ((!std::isnan(amount)) && (amount > 0))
                {
                    p->food += amount * p->area;
                }
            }
			else if (param.food_mode == Food::SHIFT)
            {
                if (frame_num < 250)
                {
                    p->food += p->area;
                }
				else
                {
                    p->calculate_curvature();
                    float amount = p->curvature;
                    if ((!std::isnan(amount) && (amount > 0)))
                    {
                        p->food += amount;
                    }
                }
            }
			else if (param.food_mode == Food::TENTACLE)
            {
                if (p->special)
                {
                    p->food += p->area;
                    if (frame_num % 1500 == 1499)
                    {
                        p->special_baby = true;
                    }
                }
				else if (p->generation < 3)
                {
                    p->food += p->area;
                }
            }
        }
    }
}

void Simulation::split()
{
    bool did_split = false;
    int new_pop = cells.size();
    int fixed_pop = new_pop;
    for (int i=0; i<fixed_pop; i++)
    {
        if (cells[i]->frozen || cells[i]->environs)
        {
            continue;
        }
        if (cells[i]->food > param.threshold || cells[i]->get_num_links()> param.max_degree)
        {
            if (new_pop == MAX_POP)
            {
                std::cout << "Maximum population of " <<
                    MAX_POP << " reached." << std::flush;
                return;
            }

            if (!cells[i]->good_loop())
            {
                cells[i]->frozen = true;
                continue;
            }

            if (!did_split)
            {
                std::cout << "Did split. " << std::flush;
                did_split = true;
            }

            cells.push_back(new Particle(new_pop));

            if (param.split_mode == Split::ZERO)
            {
                cells[i]->split(cells.back(), false);
            } else if (param.split_mode == Split::LONG)
            {
                cells[i]->split(cells.back(), true);
            }

            if (!cells[new_pop]->good_loop())
            {
                cells[new_pop]->frozen = true;
            }

            new_pop++;
        }
    }
}

void Simulation::collision_grid()
{
    Grid* _g = make_grid();
    std::unique_ptr<Grid> g(_g);
    int sorted[cells.size()];
    Box boxes[g->box_num];

    g->create_box_list(sorted, boxes);
    for (auto& p : cells)
    {
        g->set_box(p);
    }

    float c_sq = param.collision_radius * param.collision_radius;
    for (auto& p : cells)
	{
        for (int n : g->get_neighbors(p))
		{
            auto& q = cells[n];
            Vec3 disp = p->position - q->position;
            float dist = disp.squaredNorm();
            if ((p!=q) and (dist < c_sq) and (!p->connected_to(q)))
            {
                float dist = disp.squaredNorm();
                disp.normalize();
                disp *= (c_sq - dist) / c_sq;
                p->collision_target += disp;
                p->collisions++;
            }
        }
    }

    for (auto& p : cells)
    {
        if (p->collisions != 0)
        {
            p->collision_target /= (float) p->collisions;
            p->collision_target *= param.collision_factor;
            p->delta = p->collision_target;
        }
    }
}

void Simulation::brute_force_collision(){
    float c_sq = param.collision_radius * param.collision_radius;
    for (int i=0; i<cells.size(); i++)
    {
        for (int j=0; j<cells.size(); j++)
        {
            Vec3 disp = cells[i]->position - cells[j]->position;
            float dist = disp.squaredNorm();
            if ((i!=j) and (dist < c_sq) and (!cells[i]->connected_to(cells[j])))
            {
                float dist = disp.squaredNorm();
                disp.normalize();
                disp *= (c_sq - dist) / c_sq;
                cells[i]->collision_target += disp;
                cells[i]->collisions++;
            }
        }
    }

    for (auto& p : cells)
    {
        if (p->collisions != 0)
        {
            p->collision_target /= (float) p->collisions;
            p->collision_target *= param.collision_factor;
            p->delta = p->collision_target;
        }
    }
}

Grid* Simulation::make_grid()
{
    Grid* g;
    int big = 1 << 12;
    float max_x(-big), max_y(-big), max_z(-big);
    float min_x(big), min_y(big), min_z(big);

    for (auto& p : cells)
    {
        Vec3 pos = p->position;
        if (max_x < pos.x()) max_x = pos.x();
        if (max_y < pos.y()) max_y = pos.y();
        if (max_z < pos.z()) max_z = pos.z();
        if (min_x > pos.x()) min_x = pos.x();
        if (min_y > pos.y()) min_y = pos.y();
        if (min_z > pos.z()) min_z = pos.z();
    }

    Vec3 mx(max_x, max_y, max_z);
    Vec3 mn(min_x, min_y, min_z);

    g = new Grid(mx, mn, param.collision_radius);

    for (auto& p : cells)
    {
        g->add_point(p->position, p->index);
    }
    return g;
}

Simulation::~Simulation(){
   for (auto it = cells.begin(); it != cells.end(); ++it){
       delete *it;
   }
}
