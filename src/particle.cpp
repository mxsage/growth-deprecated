#include "particle.h"

Particle::Particle(int _index) :
    index(_index)
{
    delta.setZero();
    position.setZero();
    planar_target.setZero();
    collision_target.setZero();
    links.reserve(MAX_CAPACITY);
}

Particle::~Particle(){};

void Particle::print()
{
    std::cout << "Particle Info: \nIndex " << index
        << " position" << position.x() << " "
        << position.y() << " " << position.z() << "\n"
        << "Num Links: " << links.size() << std::endl;
}

int Particle::get_num_links() const
{
    return links.size();
}

Vec3 Particle::get_position() const
{
    return position;
}

void Particle::update(const double dampening)
{
    if (environs)
    {
        return;
    }
    else if (!(std::isfinite(delta.x()) &&
               std::isfinite(delta.y()) &&
               std::isfinite(delta.z())))
    {
        throw_runtime_error("Delta is NAN or inf.");
    }
    else
    {
        position += delta * dampening;
    }

    delta.setZero();
    collision_target.setZero();
    collisions = 0;
    age++;
}

bool Particle::connected_to(Particle* p) const
{
    return std::find(links.cbegin(), links.cend(), p) != links.cend();
}

void Particle::add_link(Particle* p)
{
    if (p == this)
    {
        throw_runtime_error("Can't add link to yourself.");
    } else if (std::find(links.cbegin(), links.cend(), p) != links.cend())
    {
        throw_runtime_error("Link already exists.");
    }
    links.push_back(p);
}

void Particle::remove_link(Particle* p)
{
    auto e = std::find(links.begin(), links.end(), p);

    if (e == links.end())
    {
        throw_runtime_error("Removing a link that isn't there");
    }

    links.erase(e);
}

bool Particle::connected_to_frozen()
{
    for (auto& p : links)
    {
        if (p->frozen)
        {
            return true;
        }
    }
    return false;
}

Particle* Particle::get_next(const Particle* p)
{
    return get_next(p, p);
}

Particle* Particle::get_next(const Particle* p_0, const Particle* p_1)
{
    for (int i=0; i<links.size(); i++)
    {
        for (int j=0; j<p_1->links.size(); j++)
        {
            if ((links[i] == p_1->links[j])
                    and (links[i] != p_0)
                    and (links[i] != p_1))
                    //and (connected_to(links[i])))
            {
                return links[i];
            }
        }
    }

    throw_runtime_error("Can't find next link.");
    Particle* p = nullptr;
    return p;
}

bool Particle::good_loop()
{
    order_neighbors();
    std::sort(links.begin(), links.end());
    bool return_me = std::unique(links.begin(), links.end()) == links.end();
    order_neighbors();
    return return_me;
}

void Particle::order_neighbors()
{
    Particle* temp[links.size()];

    // perhaps set this to be a random element of links...
    temp[0] = links[0];
    temp[1] = get_next(temp[0]);

    for (int i=2; i<links.size(); i++)
    {
        temp[i] = get_next(temp[i-2], temp[i-1]);
    }

    std::copy(temp, temp + links.size(), links.begin());
}

void Particle::calculate_normal()
{
    order_neighbors();

    Vec3 new_normal;

    for (int i=0; i<links.size(); i++)
    {
        Vec3 c = links[i]->position;
        Vec3 d = links[(i+1)%links.size()]->position;
        c = d.cross(c);
        new_normal += c;
    }

    area = new_normal.squaredNorm();
    new_normal.normalize();

    if (new_normal.dot(normal) >= 0)
    {
        normal = new_normal;
    } else
    {
        normal = -new_normal;
        std::reverse(links.begin(), links.end());
    }
}

void Particle::calculate(const double spring_factor, const double planar_factor,
                         const double bulge_factor, const double spring_length)
{
    calculate_normal();
    add_bulge_force(bulge_factor, spring_length);
    add_spring_force(spring_factor, spring_length);
    add_planar_force(planar_factor);
}

void Particle::add_spring_force(const double spring_factor, const double spring_length)
{
    Vec3 target;
    for (auto l : links)
    {
        Vec3 d = position - l->position;
        d.normalize();
        d *= spring_length;
        d += l->position - position;
        target += d;
		/*
        Vec3 d = l->position - position;
        d.normalize();
        d *= spring_length;
        target += d;
		*/
    }

    // TODO test without averaging target
    target /= links.size();
    target *= spring_factor;
    delta += target;
}

void Particle::calculate_planar_target()
{
    planar_target.setZero();
    for (int i=0; i<links.size(); i++)
    {
        planar_target += links[i]->position;
    }
    planar_target /= (double ) links.size();
    planar_target = planar_target - position;
}

void Particle::add_planar_force(double planar_factor)
{
    calculate_planar_target();
    delta += planar_target * planar_factor;
}

void Particle::add_bulge_force(const double bulge_factor, const double spring_length)
{
    double bulge_dist = 0;
    double theta_l, theta_d, theta_c, radicand;
    for (int i=0; i<links.size(); i++)
    {
        Vec3 d = links[i]->position - position;
        theta_l = acos(d.dot( normal) / d.norm());
        theta_d = asin(d.norm()*sin(theta_l) / spring_length);
        theta_c = M_PI - theta_d - theta_l;

        if (!std::isfinite(theta_c))
        {
            continue;
        }

        radicand = std::pow(spring_length, 2) + d.squaredNorm()
            - 2.0*d.norm() * spring_length * cos(theta_c);
        if (radicand < 0.0)
        {
            radicand = 0;
        }
        bulge_dist += sqrt(radicand);
    }

    bulge_dist /= links.size();

    delta += normal*bulge_dist*bulge_factor;
}

void Particle::calculate_curvature()
{
    calculate_planar_target();
    curvature = -1.0 * planar_target.norm() * normal.dot(planar_target.normalized());
}

int Particle::find_shortest_axis()
{
    order_neighbors();
    double min_len{0};
    int shortest_idx = -1;
    for (int i=0; i<links.size(); i++)
    {
        double distance = (position - links[i]->position).norm();
        int opposite = (i + (links.size()/2)) % links.size();
        distance += (position - links[opposite]->position).norm();
        if (distance < min_len || i==0)
        {
            min_len = distance;
            shortest_idx = i;
        }
    }
    return shortest_idx;
}

int Particle::find_longest_axis(){
    order_neighbors();
    double max_len = std::numeric_limits<double >::max();
    int longest_index = 0;
    for (int i=0; i<links.size(); i++)
    {
        double distance = (position - links[i]->position).norm();
        int opposite = (i + (links.size()/2)) % links.size();
        distance += (position - links[opposite]->position).norm();
        if (distance > max_len || i==0)
        {
            max_len = distance;
            longest_index = i;
        }
    }
    return longest_index;
}

void Particle::set_links(Particle* baby, const bool longest)
{
    order_neighbors();

    // rotate the list so shortest axis is at front
    if (longest)
    {
        int shortest_idx = find_shortest_axis();
        std::rotate(links.begin(), links.begin() + shortest_idx, links.end());
    }

    int o_num_links = links.size();
    Particle* removeable[links.size()/2+1];

    for (int i=0; i<links.size()/2+1; i++)
    {
        removeable[i] = links[i];
    }

    for (int i=0; i<o_num_links/2 + 1; i++)
    {
        Particle* r = removeable[i];
        baby->add_link(r);
        r->add_link(baby);

        if ((r != removeable[0]) and (r != removeable[o_num_links/2]))
        {
            remove_link(r);
            r->remove_link(this);
        }
    }

    add_link(baby);
    baby->add_link(this);
}

void Particle::set_baby_parent_positions(Particle* baby)
{
    // calculate baby position
    Vec3 baby_avg = position;
    for (int i=0; i<baby->links.size(); i++)
    {
        baby_avg += baby->links[i]->position;
    }

    baby_avg /= baby->links.size() + 1;

    // calculate parent position
    Vec3 papa_avg = position;
    for (int i=0; i<links.size(); i++)
    {
        papa_avg += links[i]->position;
    }
    papa_avg /= links.size() + 1;

    // set positions
    baby->position = baby_avg;
    position = papa_avg;
}

void Particle::create_baby(Particle* baby)
{
    baby->food = 0;
    baby->normal = normal;
    baby->position = position;
    baby->inherited = inherited;

    // set misc values dealing with obscure food distribution methods
    if (special)
    {
        if (special_baby)
        {
            baby->special = true;
            special_baby = false;
        }
    } else
    {
        baby->generation = generation;
    }
}

void Particle::split(Particle* baby, const bool longest)
{
    // reset parent values
    food = 0;
    generation++;

    create_baby(baby);
    set_links(baby, longest);
    set_baby_parent_positions(baby);
    baby->calculate_normal();
    calculate_normal();
}
