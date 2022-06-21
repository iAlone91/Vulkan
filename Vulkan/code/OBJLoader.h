#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <array>

namespace OBJLoader
{
    typedef std::array<float, 3> vec3;
    typedef std::array<float, 2> vec2;

    struct indices
    {
        uint64_t vertexIndex;
        uint64_t normalIndex;
        uint64_t texcoordIndex;
    };

    struct mesh
    {
        std::vector<indices> indices;
    };


    struct model
    {
        std::string name;
        mesh mesh;
    };

    struct attrib
    {
        std::vector<float> vertices;
        std::vector<float> colors;
        std::vector<float> normals;
        std::vector<float> texcoords;
    }; 

	void loadOBJ(attrib& attrib, std::vector<model>& models , const std::string& modelpath)
	{
        uint16_t modelIndex = -1;
        std::array<float, 3> tempFloat{};
       

        std::stringstream ss;
        std::ifstream file(modelpath);
        std::string line{};
        std::string prefix{};


        if (!file.is_open())
            throw std::runtime_error("faild to open file: " + modelpath);

        while (std::getline(file, line))
        {
            ss.clear();
            ss.str(line);
            ss >> prefix;

            if (prefix == "#")
            {

            }
            else if (prefix == "o")
            {
                modelIndex++;
                model model{};
                ss >> model.name;
                models.push_back(model);
            }
            else if (prefix == "v")
            {

                ss >> tempFloat[0] >> tempFloat[1] >> tempFloat[2];
                attrib.vertices.push_back(tempFloat[0]);
                attrib.vertices.push_back(tempFloat[1]);
                attrib.vertices.push_back(tempFloat[2]);
            }
            else if (prefix == "vt")
            {
               
                ss >> tempFloat[0] >> tempFloat[1];
                attrib.texcoords.push_back(tempFloat[0]);
                attrib.texcoords.push_back(tempFloat[1]);

            }
            else if (prefix == "vn")
            {
           
                ss >> tempFloat[0] >> tempFloat[1] >> tempFloat[2];
                attrib.normals.push_back(tempFloat[0]);
                attrib.normals.push_back(tempFloat[1]);
                attrib.normals.push_back(tempFloat[2]);
            }
            else if (prefix == "usemtl")
            {

            }
            else if (prefix == "s")
            {

            }
            else if (prefix == "f")
            {
                indices indices{};
                int counter{};
                size_t index{};
                while (ss >> index)
                {
                    if (counter == 0)
                        indices.vertexIndex = index - 1; // -1 becuse obj indexes start at 1 and we want it to start att 0;
                    else if (counter == 1)
                        indices.texcoordIndex = index - 1;
                    else if (counter == 2)
                    {
                        indices.normalIndex = index - 1;
                        models[modelIndex].mesh.indices.push_back(indices);
                    }
                    
                    if (ss.peek() == '/')
                    {
                        counter++;
                        ss.ignore(1, '/');
                    }
                    else if (ss.peek() == ' ')
                    {
                        counter++;
                        ss.ignore(1, ' ');
                    }

                    if (counter > 2)
                    {
                        counter = 0;
                    }

                }
                
            }
            else
            {

            }
        }

	}


};