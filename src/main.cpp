#include "main.h"


int main()
{

    std::string outputPath = "./";

    // initialize and set simulation parameters
    Parameters p = default_params();

    Simulation s;

    std::atomic<bool> continue_computing{true};
    std::atomic<bool> pause{false};
    std::atomic<bool> save_every_frame{false};
    std::atomic<bool> reset{true};

	std::mutex write_mesh;

    std::atomic<bool> redraw{false};

    const auto save_mesh = [&]()
    {
        std::ostringstream ss;
        ss << outputPath + "frames/frame" << s.frame_num << ".ply";
        {
            std::lock_guard<std::mutex> lock(write_mesh);
            igl::writePLY(ss.str(), s.V, s.F);
        }

        std::cout << "Saved frame to [" << ss.str() << "]" << std::endl;
    };

    const auto save_parameters = [&]()
    {
        std::ostringstream ss;
        ss << outputPath + "parameters/parameters" << s.frame_num << ".xml";
        parameters::writeParams(ss.str(), p);
    };

    // lambda function to advance the simulation one tick
    const auto tick = [&]()
    {
        while (continue_computing)
        {
            if (reset)
            {
                s.init(p);
                reset = false;
            }
            if (pause)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            else
            {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
                s.set_parameters(p);
                s.update();
                {
                    std::lock_guard<std::mutex> lock(write_mesh);
                    s.set_matrices();
                }

				if (save_every_frame)
				{
					save_mesh();
                    save_parameters();
				}
                redraw = true;
            }
        }
    };

    // launch the computation thread
    std::thread computation_thread(tick);

    // define the libigl viewer
    igl::viewer::Viewer v;

    // Extend viewer menu
    v.callback_init = [&](igl::viewer::Viewer& v)
    {
        v.ngui->addWindow(Eigen::Vector2i(10, 10), "Controls");

        v.ngui->addGroup("Function");
        v.ngui->addButton("Reset",[&]()
            {
                reset = !reset;
            }
        );

        v.ngui->addButton("Load Mesh", [&](){
                p.filename = nanogui::file_dialog( { {"ply", "Polygon File Format"}}, false);
                });

        v.ngui->addButton("Save Mesh", [&]()
            {
                save_mesh();
            }
        );

        v.ngui->addButton("Load Parameters", [&](){
                parameters::readParams(nanogui::file_dialog( { {"xml", "Extensible Markup Language"}},
                            false), p);
                });

        v.ngui->addButton("Save Parameters", [&]()
            {
                parameters::writeParams(outputPath + "parameters/savedParameters.xml", p);
            }
        );

        // TODO: Make this function cleaner
        v.ngui->addVariable<bool>("Pause",[&](bool val)
            {
                if (val) {
                    pause = true;
                } else {
                    pause = false;
                }
            },[&]() {
                if (pause) {
                    return true;
                } else {
                    return false;
                }
            }
        );

        // TODO: Make this function cleaner
        v.ngui->addVariable<bool>("Save Sequence",[&](bool val)
            {
                if (val) {
                    save_every_frame = true;
                } else {
                    save_every_frame = false;
                }
            },[&]() {
                if (save_every_frame) {
                    return true;
                } else {
                    return false;
                }
            }
        );

        v.ngui->addGroup("Parameters");

        v.ngui->addVariable("Threshold", p.threshold)->setSpinnable(true);
        v.ngui->addVariable("Collision Radius", p.collision_radius)->setSpinnable(true);
        v.ngui->addVariable("Collision Factor", p.collision_factor)->setSpinnable(true);
        v.ngui->addVariable("Spring Length", p.spring_length)->setSpinnable(true);
        v.ngui->addVariable("Spring Factor", p.spring_factor)->setSpinnable(true);
        v.ngui->addVariable("Planar Factor", p.planar_factor)->setSpinnable(true);
        v.ngui->addVariable("Bulge Factor", p.bulge_factor)->setSpinnable(true);
        v.ngui->addVariable("Dampening", p.dampening)->setSpinnable(true);
        v.ngui->addVariable("Curvature Factor", p.curvature_factor)->setSpinnable(true);
        v.ngui->addVariable("Max Collision Age", p.collision_age_threshold)->setSpinnable(true);

        v.ngui->addVariable("Food", p.food_mode, true)->setItems({"RANDOM",
					 "AREA", "X_COORD","RADIAL","AGE","COLLISIONS",
					 "CURVE","INHERIT","HYBRID","SHIFT"}
					 );

        v.ngui->addVariable("Split", p.split_mode, true)->setItems({"ZERO","LONG","SHORT"});

        // Generate menu
        v.screen->performLayout();

        return false;
    };

    // lambda function to set mesh before draw if it has updated
    v.callback_pre_draw =
        [&](igl::viewer::Viewer& v)->bool
        {
            if (redraw)
            {
                v.data.clear();
				{
					std::lock_guard<std::mutex> lock(write_mesh);
					v.data.set_mesh(s.V, s.F);
					v.data.set_normals(s.N);
				}
                redraw = false;
            }
            return false;
        };

    // save when button is pressed
    v.callback_key_down =
        [&](igl::viewer::Viewer& viewer, unsigned char key, int modifier)->bool
        {
            if (key == ' ')
            {
                save_mesh();
            }
            return false;
        };

    v.core.is_animating = false;

    // launch the viewer
    v.launch();

    // stop computation
    continue_computing = false;

    // attempt to join threads
    if (computation_thread.joinable())
    {
        computation_thread.join();
    }

    return 1;
}
