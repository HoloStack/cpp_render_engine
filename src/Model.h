#ifndef MODEL_H
#define MODEL_H

#include "Vec3.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct Face {
    int v[3];  // vertex indices
    int vt[3]; // texture coordinate indices
    int vn[3]; // normal indices
    
    Face() {
        for (int i = 0; i < 3; i++) {
            v[i] = vt[i] = vn[i] = -1;
        }
    }
};

class Model {
private:
    std::vector<Vec3> vertices;
    std::vector<Vec2> texCoords;
    std::vector<Vec3> normals;
    std::vector<Face> faces;

public:
    Model() {}

    bool loadOBJ(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                // Vertex
                float x, y, z;
                iss >> x >> y >> z;
                vertices.push_back(Vec3(x, y, z));
            }
            else if (prefix == "vt") {
                // Texture coordinate
                float u, v;
                iss >> u >> v;
                texCoords.push_back(Vec2(u, v));
            }
            else if (prefix == "vn") {
                // Normal
                float x, y, z;
                iss >> x >> y >> z;
                normals.push_back(Vec3(x, y, z));
            }
            else if (prefix == "f") {
                // Face
                Face face;
                std::string vertex;
                int vertexIndex = 0;
                
                while (iss >> vertex && vertexIndex < 3) {
                    std::istringstream vertexStream(vertex);
                    std::string index;
                    
                    // Parse vertex index
                    if (std::getline(vertexStream, index, '/')) {
                        face.v[vertexIndex] = std::stoi(index) - 1; // OBJ is 1-indexed
                    }
                    
                    // Parse texture coordinate index
                    if (std::getline(vertexStream, index, '/')) {
                        if (!index.empty()) {
                            face.vt[vertexIndex] = std::stoi(index) - 1;
                        }
                    }
                    
                    // Parse normal index
                    if (std::getline(vertexStream, index)) {
                        if (!index.empty()) {
                            face.vn[vertexIndex] = std::stoi(index) - 1;
                        }
                    }
                    
                    vertexIndex++;
                }
                
                if (vertexIndex == 3) {
                    faces.push_back(face);
                }
            }
        }

        file.close();
        std::cout << "Loaded model: " << vertices.size() << " vertices, " 
                  << faces.size() << " faces" << std::endl;
        return true;
    }

    // Generate normals if not provided
    void generateNormals() {
        if (!normals.empty()) return;
        
        normals.resize(vertices.size(), Vec3(0, 0, 0));
        
        for (const Face& face : faces) {
            Vec3 v0 = vertices[face.v[0]];
            Vec3 v1 = vertices[face.v[1]];
            Vec3 v2 = vertices[face.v[2]];
            
            Vec3 normal = (v1 - v0).cross(v2 - v0).normalize();
            
            normals[face.v[0]] = normals[face.v[0]] + normal;
            normals[face.v[1]] = normals[face.v[1]] + normal;
            normals[face.v[2]] = normals[face.v[2]] + normal;
        }
        
        for (Vec3& normal : normals) {
            normal = normal.normalize();
        }
    }

    // Accessors
    const std::vector<Vec3>& getVertices() const { return vertices; }
    const std::vector<Vec2>& getTexCoords() const { return texCoords; }
    const std::vector<Vec3>& getNormals() const { return normals; }
    const std::vector<Face>& getFaces() const { return faces; }
    
    Vec3 getVertex(int index) const {
        if (index >= 0 && (size_t)index < vertices.size()) {
            return vertices[index];
        }
        return Vec3(0, 0, 0);
    }
    
    Vec3 getNormal(int index) const {
        if (index >= 0 && (size_t)index < normals.size()) {
            return normals[index];
        }
        return Vec3(0, 0, 1);
    }
    
    Vec2 getTexCoord(int index) const {
        if (index >= 0 && (size_t)index < texCoords.size()) {
            return texCoords[index];
        }
        return Vec2(0, 0);
    }
};

#endif
