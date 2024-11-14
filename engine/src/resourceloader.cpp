#include "lava/resourceloader.hpp"
#include "lava/rendering/data/mesh.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
namespace lava::resourceloader
{
    std::vector<char> readfile(const std::string &filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file: " + filepath);
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }
    std::vector<std::string_view> split(const std::string_view& line, char delimiter)
    {
        std::vector<std::string_view> result;
        size_t pos = 0;

        std::string_view tmp = std::string_view(line);
        while (tmp.length() > 0)
        {
            pos = tmp.find(delimiter);
            if (pos == std::string::npos)
            {
                result.push_back(tmp);
                return result;
            }
            std::string_view part = tmp.substr(0, pos);
            result.push_back(part);
            tmp = tmp.substr(pos + 1, tmp.length() - pos);
        }

        return result;
    }
    rendering::data::Mesh loadMesh(const std::string &filepath)
    {
        std::ifstream infile(filepath);

        std::string line;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;

        rendering::data::Mesh mesh;
        mesh.indices.clear();
        mesh.vertices.clear();
        uint32_t indices = 0;
        std::unordered_map<rendering::data::Vertex, uint32_t> uniqueVertices{};
        while (std::getline(infile, line))
        {
            std::istringstream iss(line);
            if(line[0] == '#') continue;
            auto parts = split(line, ' ');
            if (parts[0] == "v")
            {
                float x, y, z;
                x = static_cast<float>(std::atof(parts[1].data()));
                y = static_cast<float>(std::atof(parts[2].data()));
                z = static_cast<float>(std::atof(parts[3].data()));
                vertices.push_back(glm::vec3(x, y, z));
            }
            else if (parts[0] == "vn")
            {
                float x, y, z;
                x = static_cast<float>(std::atof(parts[1].data()));
                y = static_cast<float>(std::atof(parts[2].data()));
                z = static_cast<float>(std::atof(parts[3].data()));
                normals.push_back(glm::vec3(x, y, z));
            }
            else if (parts[0] == "vt")
            {
                float x, y;
                x = static_cast<float>(std::atof(parts[1].data()));
                y = static_cast<float>(std::atof(parts[2].data()));
                texcoords.push_back(glm::vec2(x, 1-y));
            }
            else if (parts[0] == "f")
            {
                for (size_t i = 1; i < parts.size(); i++)
                {
                    auto faceParts = split(parts[i], '/');
                    int v = std::atoi(faceParts[0].data());
                    int vt = std::atoi(faceParts[1].data());
                    int vn = std::atoi(faceParts[2].data());

                    rendering::data::Vertex vertex{};
                    vertex.position = vertices[v - 1];
                    vertex.normal = normals[vn - 1];
                    vertex.texCoord = texcoords[vt - 1];

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
                        mesh.vertices.push_back(vertex);
                    }
                    mesh.indices.push_back(uniqueVertices[vertex]);
                }
            }
        }
        return mesh;
    }

}
