#ifndef RENDERER_H
#define RENDERER_H

#include "Framebuffer.h"
#include "Model.h"
#include "Shader.h"
#include "Matrix4x4.h"

class Renderer {
public:
    int width, height;
    Framebuffer framebuffer;
    Shader shader;

    Renderer(int w, int h) : width(w), height(h), framebuffer(w, h) {}

    bool loadOBJ(const std::string& filename, Model& model) {
        return model.loadOBJ(filename);
    }

    void renderModel(const Model& model) {
        const auto& vertices = model.getVertices();
        const auto& faces = model.getFaces();
        
        int trianglesRendered = 0;
        
        for (const auto& face : faces) {
            Vec3 worldVerts[3];
            Vec3 normals[3] = {Vec3(0,0,1), Vec3(0,0,1), Vec3(0,0,1)};
            Vec2 texCoords[3] = {Vec2(0,0), Vec2(0,0), Vec2(0,0)};
            Vertex shaderVerts[3];
            Vec3 screenVerts[3];

            // Get vertex data and run vertex shader
            bool clipped = false;
            for (int i = 0; i < 3; ++i) {
                worldVerts[i] = model.getVertex(face.v[i]);
                if (face.vn[i] >= 0) normals[i] = model.getNormal(face.vn[i]);
                if (face.vt[i] >= 0) texCoords[i] = model.getTexCoord(face.vt[i]);

                shaderVerts[i] = shader.vertexShader(worldVerts[i], normals[i], texCoords[i]);
                
                // Clip test - if any vertex is too far behind or in front, skip triangle
                if (shaderVerts[i].position.z < -1.0f || shaderVerts[i].position.z > 1.0f) {
                    clipped = true;
                    break;
                }
                
                // Convert to screen coordinates but keep depth
                screenVerts[i].x = (shaderVerts[i].position.x + 1.0f) * width * 0.5f;
                screenVerts[i].y = (shaderVerts[i].position.y + 1.0f) * height * 0.5f;
                screenVerts[i].z = shaderVerts[i].position.z; // Keep NDC depth for z-buffer
            }
            
            if (clipped) continue;

            // Back-face culling in world space using face normals
            Vec3 worldEdge1 = worldVerts[1] - worldVerts[0];
            Vec3 worldEdge2 = worldVerts[2] - worldVerts[0];
            Vec3 faceNormal = worldEdge1.cross(worldEdge2);
            Vec3 viewDir = shader.cameraPos - worldVerts[0];
            
            if (faceNormal.dot(viewDir) > 0) { // Front-facing triangle
                // Check if any part of triangle is on screen
                bool onScreen = false;
                for (int i = 0; i < 3; i++) {
                    if (screenVerts[i].x >= -50 && screenVerts[i].x < width + 50 &&
                        screenVerts[i].y >= -50 && screenVerts[i].y < height + 50) {
                        onScreen = true;
                        break;
                    }
                }
                
                if (onScreen) {
                    drawTriangle(screenVerts[0], screenVerts[1], screenVerts[2], shaderVerts[0]);
                    trianglesRendered++;
                    
                    // Debug first few triangles
                    if (trianglesRendered <= 3) {
                        std::cout << "Triangle " << trianglesRendered << " screen vertices: ";
                        for (int i = 0; i < 3; i++) {
                            std::cout << "(" << screenVerts[i].x << "," << screenVerts[i].y << "," << screenVerts[i].z << ") ";
                        }
                        std::cout << std::endl;
                    }
                }
            }
        }
        
        std::cout << "Rendered " << trianglesRendered << " triangles" << std::endl;
    }

    void drawTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vertex& shaderVert) {
        // Use shader to compute color
        Color pixelColor = shader.fragmentShader(shaderVert);
        framebuffer.drawTriangle(v0, v1, v2, pixelColor);
    }

    void render() {
        // Just save the framebuffer - don't clear it as rendering has already happened
        framebuffer.saveToPPM("output.ppm");
    }
};

#endif

