#pragma once

#include "simulation.h"
#include "params.h"
#include "parametersIO.h"
#include "utils.h"

#include <atomic>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <thread>
#include <sstream>

#include <igl/viewer/Viewer.h>
#include <igl/writePLY.h>

#include <nanogui/formhelper.h>
#include <nanogui/screen.h>
