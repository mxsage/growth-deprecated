#include "parametersIO.h"

namespace
{
	parameters::Shape shapeFromString(const std::string& shapeName)
	{
		if (shapeName == "OCTAHEDRON")
		{
			return parameters::Shape::OCTAHEDRON;
		}
		if (shapeName == "ICOSAHEDRON")
		{
			return parameters::Shape::ICOSAHEDRON;
		}
		if (shapeName == "TUBE")
		{
			return parameters::Shape::TUBE;
		}
		if (shapeName == "PLANE")
		{
			return parameters::Shape::PLANE;
		}
		if (shapeName == "OBJ")
		{
			return parameters::Shape::OBJ;
		}
		if (shapeName == "MULTIPLE")
		{
			return parameters::Shape::MULTIPLE;
		}
		if (shapeName == "ENVIRONMENT")
		{
			return parameters::Shape::ENVIRONMENT;
		}
		if (shapeName == "OBJ_TEX")
		{
			return parameters::Shape::OBJ_TEX;
		}
		if (shapeName == "PLY")
		{
			return parameters::Shape::PLY;
		}

		return parameters::Shape::UNKNOWN;
	}

	std::string stringFromShape(const parameters::Shape shape)
	{
		switch(shape)
		{
		case parameters::Shape::OCTAHEDRON:
			return "OCTAHEDRON";
		case parameters::Shape::ICOSAHEDRON:
			return "ICOSAHEDRON";
		case parameters::Shape::TUBE:
			return "TUBE";
		case parameters::Shape::PLANE:
			return "PLANE";
		case parameters::Shape::OBJ:
			return "OBJ";
		case parameters::Shape::MULTIPLE:
			return "MULTIPLE";
		case parameters::Shape::ENVIRONMENT:
			return "ENVIRONMENT";
		case parameters::Shape::OBJ_TEX:
			return "OBJ_TEX";
		case parameters::Shape::PLY:
			return "PLY";
		case parameters::Shape::UNKNOWN:
			return "UNKNOWN";
		}
	}

	parameters::Food foodFromString(const std::string& foodName)
	{
		if (foodName == "RANDOM")
		{
			return parameters::Food::RANDOM;
		}
		if (foodName == "AREA")
		{
			return parameters::Food::AREA;
		}
		if (foodName == "X_COORD")
		{
			return parameters::Food::X_COORD;
		}
		if (foodName == "RADIAL")
		{
			return parameters::Food::RADIAL;
		}
		if (foodName == "AGE")
		{
			return parameters::Food::AGE;
		}
		if (foodName == "COLLISIONS")
		{
			return parameters::Food::COLLISIONS;
		}
		if (foodName == "CURVATURE")
		{
			return parameters::Food::CURVATURE;
		}
		if (foodName == "INHERIT")
		{
			return parameters::Food::INHERIT;
		}
		if (foodName == "HYBRID")
		{
			return parameters::Food::HYBRID;
		}
		if (foodName == "SHIFT")
		{
			return parameters::Food::SHIFT;
		}
		if (foodName == "TENTACLE")
		{
			return parameters::Food::TENTACLE;
		}

		return parameters::Food::UNKNOWN;
	}

	std::string stringFromFood(const parameters::Food food)
	{
		switch(food)
		{
		case parameters::Food::RANDOM:
			return "RANDOM";
		case parameters::Food::AREA:
			return "AREA";
		case parameters::Food::X_COORD:
			return "X_COORD";
		case parameters::Food::RADIAL:
			return "RADIAL";
		case parameters::Food::AGE:
			return "AGE";
		case parameters::Food::COLLISIONS:
			return "COLLISIONS";
		case parameters::Food::CURVATURE:
			return "CURVATURE";
		case parameters::Food::INHERIT:
			return "INHERIT";
		case parameters::Food::HYBRID:
			return "HYBRID";
		case parameters::Food::SHIFT:
			return "SHIFT";
		case parameters::Food::TENTACLE:
			return "TENTACLE";
		case parameters::Food::UNKNOWN:
			return "UNKNOWN";
		}
	}

	parameters::Split splitFromString(const std::string& splitName)
	{
		if (splitName == "ZERO")
		{
			return parameters::Split::ZERO;
		}
		if (splitName == "LONG")
		{
			return parameters::Split::LONG;
		}
		if (splitName == "SHORT")
		{
			return parameters::Split::SHORT;
		}

		return parameters::Split::UNKNOWN;
	}

	std::string stringFromSplit(const parameters::Split split)
	{
		switch(split)
		{
		case parameters::Split::ZERO:
			return "ZERO";
		case parameters::Split::LONG:
			return "LONG";
		case parameters::Split::SHORT:
			return "SHORT";
		case parameters::Split::UNKNOWN:
			return "UNKNOWN";
		}
	}

}

namespace parameters
{

	Parameters default_params()
	{
		Parameters p;
		p.spring_factor = .1;
		p.spring_length = .05;
		p.planar_factor = 0.2;
		p.bulge_factor = 0.0;
		p.collision_factor = 0.4;
		p.collision_radius = .2;
		p.curvature_factor = 1;
		p.dampening = .2;
		p.threshold = 10;
		p.max_degree = 20;
		p.init_shape = Shape::PLY;
		p.food_mode = Food::AREA;
		p.split_mode = Split::LONG;
		p.filename = "../models/icosahedron.ply";

		return p;
	}

	template <typename T>
	void addElement(XMLDocument& xmlDoc, XMLNode* pRoot, const std::string& elementName,
					T element)
	{
        XMLElement* pElement = xmlDoc.NewElement(elementName.c_str());
        pElement->SetText(element);
        pRoot->InsertEndChild(pElement);
	}

    bool writeParams(const std::string& outputPath, const Parameters& out)
    {
        XMLDocument xmlDoc;
        XMLNode* pRoot = xmlDoc.NewElement("Root");
        xmlDoc.InsertFirstChild(pRoot);

		addElement(xmlDoc, pRoot, "spring_factor", out.spring_factor);
		addElement(xmlDoc, pRoot, "spring_length", out.spring_length);
		addElement(xmlDoc, pRoot, "planar_factor", out.planar_factor);
		addElement(xmlDoc, pRoot, "bulge_factor", out.bulge_factor);
		addElement(xmlDoc, pRoot, "collision_factor", out.collision_factor);
		addElement(xmlDoc, pRoot, "collision_radius", out.collision_radius);
		addElement(xmlDoc, pRoot, "dampening", out.dampening);
		addElement(xmlDoc, pRoot, "threshold", out.threshold);
		addElement(xmlDoc, pRoot, "max_degree", out.max_degree);
		addElement(xmlDoc, pRoot, "init_shape", stringFromShape(out.init_shape).c_str());
		addElement(xmlDoc, pRoot, "food_mode", stringFromFood(out.food_mode).c_str());
		addElement(xmlDoc, pRoot, "split_mode", stringFromSplit(out.split_mode).c_str());
		addElement(xmlDoc, pRoot, "save_frequency", out.save_frequency);
		addElement(xmlDoc, pRoot, "write_faces", out.write_faces);
		addElement(xmlDoc, pRoot, "write_cell_radius", out.write_cell_radius);
		addElement(xmlDoc, pRoot, "filename", out.filename.c_str());

        XMLError result = xmlDoc.SaveFile(outputPath.c_str());
        return XMLCheckResult(result);
    }

	bool readDouble(XMLNode* pRoot, const std::string name, double& doubleOut)
	{
		XMLElement* pElement = pRoot->FirstChildElement(name.c_str());
		if (pElement == nullptr)
		{
			return false;
		}

		double holder;
		XMLError eResult = pElement->QueryDoubleText(&holder);

		if (XMLCheckResult(eResult))
		{
			doubleOut = holder;
			return true;
		}

		return false;
	}

	bool readBool(XMLNode* pRoot, const std::string name, bool& boolOut)
	{
		XMLElement* pElement = pRoot->FirstChildElement(name.c_str());
		if (pElement == nullptr)
		{
			return false;
		}

		bool holder;
		XMLError eResult = pElement->QueryBoolText(&holder);

		if (XMLCheckResult(eResult))
		{
			boolOut = holder;
			return true;
		}

		return false;
	}

	bool readInt(XMLNode* pRoot, const std::string name, int& intOut)
	{
		XMLElement* pElement = pRoot->FirstChildElement(name.c_str());
		if (pElement == nullptr)
		{
			return false;
		}

		int holder;
		XMLError eResult = pElement->QueryIntText(&holder);

		if (XMLCheckResult(eResult))
		{
			intOut = holder;
			return true;
		}

		return false;
	}

	bool readString(XMLNode* pRoot, const std::string name, std::string& stringOut)
	{
		XMLElement* pElement = pRoot->FirstChildElement(name.c_str());
		if (pElement == nullptr)
		{
			return false;
		}

		const char* cResult;
		cResult = pElement->GetText();
		if (cResult != nullptr)
		{
			stringOut = cResult;
		}

		return true;
	}

    bool readParams(const std::string& inputPath, Parameters& in)
    {

		XMLDocument xmlDoc;
		XMLError result = xmlDoc.LoadFile(inputPath.c_str());

		if (!XMLCheckResult(result))
		{
			return false;
		}

		XMLNode* pRoot = xmlDoc.FirstChild();

		if (pRoot == nullptr)
		{
			return false;
		}

		readDouble(pRoot, "spring_factor", in.spring_factor);
		readDouble(pRoot, "spring_length", in.spring_length);
		readDouble(pRoot, "planar_factor", in.planar_factor);
		readDouble(pRoot, "bulge_factor", in.bulge_factor);
		readDouble(pRoot, "collision_factor", in.collision_factor);
		readDouble(pRoot, "collision_radius", in.collision_radius);
		readDouble(pRoot, "dampening", in.dampening);
		readDouble(pRoot, "threshold", in.threshold);

		readInt(pRoot, "max_degree", in.max_degree);

		std::string init_shape;
		readString(pRoot, "init_shape", init_shape);
		in.init_shape = shapeFromString(init_shape);

		std::string food_mode;
		readString(pRoot, "food_mode", food_mode);
		in.food_mode = foodFromString(food_mode);

		std::string split_mode;
		readString(pRoot, "split_mode", split_mode);
		in.split_mode = splitFromString(split_mode);

		readInt(pRoot, "save_frequency", in.save_frequency);

		readBool(pRoot, "write_faces", in.write_faces);
		readBool(pRoot, "write_cell_radius", in.write_cell_radius);

		std::string filename;
		readString(pRoot, "filename", filename);
		in.filename = filename;

		return true;
    }
}
