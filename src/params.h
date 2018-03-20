#pragma once
#include <string>

#define MAX_CAPACITY 40
#define MAX_POP 1000000

namespace parameters {

    enum class Shape {OCTAHEDRON,
				      ICOSAHEDRON,
					  TUBE,
				      PLANE,
				      OBJ,
					  MULTIPLE,
					  ENVIRONMENT,
				      OBJ_TEX,
					  PLY,
					  UNKNOWN};

    enum class Food {RANDOM,
					 AREA,
					 X_COORD,
					 RADIAL,
					 AGE,
					 COLLISIONS,
					 CURVATURE,
            		 INHERIT,
					 HYBRID,
					 SHIFT,
					 TENTACLE,
					 UNKNOWN};

    enum class Split {ZERO,
			  		  LONG,
					  SHORT,
					  UNKNOWN};

    struct Parameters
	{
        double spring_factor;
        double spring_length;
        double planar_factor;
        double bulge_factor;
        double curvature_factor;
        double collision_factor;
        double collision_radius;
		double dampening;
        double threshold;
        int max_degree;
		int collision_age_threshold;

        Shape init_shape;
        Food food_mode;
        Split split_mode;

        int save_frequency;
        bool write_faces;
		bool write_cell_radius;
        std::string filename;
    };
}
