#pragma once

#include <tinyxml2.h>

#include <string>
#include <iostream>

#include "params.h"

using namespace tinyxml2;

namespace parameters
{

	inline bool XMLCheckResult(XMLError result)
	{
		if (result != XML_SUCCESS)
		{
			std::cout << "Error: " << result << std::endl;
			return false;
		}
		return true;
	}

	bool writeParams(const std::string& outputPath, const Parameters& out);

	bool readParams(const std::string& outputPath, Parameters& in);
}
