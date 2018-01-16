#include "main.h"

int main()
{
    /* initialize and set simulation parameters */
    Parameters p;
	parameters::readParams(outputPath + "parameters/savedParameters.xml", p);

	std::cout << p.filename << std::endl;

    Simulation s;
    s.init(p);

    /* should the simulation continue computing? */
    std::atomic<bool> continue_computing{true};
    std::atomic<bool> pause{false};

	/* mutex to protect mesh */
	std::mutex write_mesh;

    /* should the viewer redraw? */
    std::atomic<bool> redraw{false};

    /* lambda function to advance the simulation one tick */
    const auto tick = [&]()
    {
        while (continue_computing)
        {
            if (!pause)
            {
                s.update();
                {
                    std::lock_guard<std::mutex> lock(write_mesh);
                    s.set_matrices();
                }
                redraw = true;
            }
            // else sleep for a tiny bit??
        }
    };

    /* launch the computation thread */
    std::thread computation_thread(tick);

    /* define the libigl viewer */
    igl::viewer::Viewer v;

    // Extend viewer menu
    v.callback_init = [&](igl::viewer::Viewer& v)
    {
        v.ngui->addWindow(Eigen::Vector2i(10, 10), "Controls");

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

        v.ngui->addButton("Save Mesh", [&]()
            {
                std::ostringstream ss;
                ss << outputPath + "build/frames/frame" << s.frame_num << ".ply";
				std::lock_guard<std::mutex> lock(write_mesh);
                igl::writePLY(ss.str(), s.V, s.F);
            }
        );

        // Generate menu
        v.screen->performLayout();

        return false;
    };

    /* lambda function to set mesh before draw if it has updated */
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

    /* save when button is pressed */
    v.callback_key_down =
        [&](igl::viewer::Viewer& viewer, unsigned char key, int modifier)->bool
        {
            if (key == ' ')
            {
                std::cout << "Pressed space." << std::endl;
            }
            return false;
        };

    v.core.is_animating = true;

    /* launch the viewer */
    v.launch();

    /* stop computation */
    continue_computing = false;

    /* attempt to join threads */
    if (computation_thread.joinable())
    {
        computation_thread.join();
    }

    return 1;
}
