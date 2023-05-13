#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "../extern/tinyL/tiny_obj_loader.h"
#include "../extern/glm/glm/glm.hpp"
#include <stdio.h>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        perror("USAGE: objToMesh input.obj output.mesh\n");
        return 0;
    }
    
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, argv[2])) {
        perror("could not read obj\n");
        return 0;
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.normal = {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            };
            vertices.push_back(vertex);
            indices.push_back(indices.size());
        
        }
        break;
    }

    FILE * f = fopen(argv[3], "w");

    uint32_t vSize = vertices.size();
    uint32_t iSize = indices.size();

    fwrite(&vSize, sizeof(uint32_t), 1, f);
    fwrite(&iSize, sizeof(uint32_t), 1, f);
    fwrite(vertices.data(), sizeof(Vertex), vertices.size(), f);
    fwrite(indices.data(), sizeof(uint32_t), indices.size(), f);

    fclose(f);
    return 0;
}

