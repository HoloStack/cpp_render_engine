#include "Renderer.h"
#include <iostream>
#include <cmath>

// Function declaration
void createTestCube(Renderer& renderer);

int main() {
    const int WIDTH = 800;
    const int HEIGHT = 600;
    
    // Create renderer
    Renderer renderer(WIDTH, HEIGHT);
    
    // Setup camera (Lesson 5: Moving the camera)
    Vec3 cameraPos(0, 0, 3);
    Vec3 cameraTarget(0, 0, 0);
    Vec3 cameraUp(0, 1, 0);
    
    // Setup perspective projection (Lesson 4: Perspective projection)
    float fov = 3.14159f / 4.0f; // 45 degrees
    float aspect = (float)WIDTH / (float)HEIGHT;
    float near = 50.0f;  // Improved for better depth precision
    float far = 1500.0f; // Reduced far plane for better depth precision
    
    // Setup matrices
    renderer.shader.viewMatrix = Matrix4x4::lookAt(cameraPos, cameraTarget, cameraUp);
    renderer.shader.projectionMatrix = Matrix4x4::perspective(fov, aspect, near, far);
    renderer.shader.modelMatrix = Matrix4x4(); // Identity for now
    renderer.shader.cameraPos = cameraPos;
    renderer.shader.updateMVP();
    
    // Setup lighting
    renderer.shader.lights.clear();
    
    // Add directional light
    Light dirLight;
    dirLight.type = 0; // Directional
    dirLight.direction = Vec3(-1, -1, -1).normalize();
    dirLight.color = Color(255, 255, 255);
    dirLight.intensity = 1.0f;
    renderer.shader.lights.push_back(dirLight);
    
    // Add point light
    Light pointLight;
    pointLight.type = 1; // Point
    pointLight.position = Vec3(200, 200, 200); // Adjusted for car model scale
    pointLight.color = Color(255, 200, 150);
    pointLight.intensity = 0.8f;
    renderer.shader.lights.push_back(pointLight);
    
    // Enable advanced features
    renderer.shader.enableShadows = true; // Lesson 7: Shadow mapping
    renderer.shader.enableAO = true;      // Lesson 8: Ambient occlusion
    
    // Create a simple test scene if no OBJ file is available
    std::cout << "Attempting to load OBJ file..." << std::endl;
    
    // Try to load the Volkswagen Beetle OBJ file
    bool objLoaded = false;
    // Try different OBJ files - prioritize the complete model
    std::vector<std::string> objFiles = {
        "uploads-files-5718873-Volkswagen+Beetle+1963_obj/Volkswagen Beetle 1963.obj",
        "uploads-files-5718873-Volkswagen+Beetle+1963_obj/Volkswagen Beetle 1963 Exploded.obj"
    };
    
    std::string objFile;
    Model model;
    for (const auto& file : objFiles) {
        if (renderer.loadOBJ(file, model)) {
            objFile = file;
            break;
        }
    }
    
    if (!objFile.empty()) {
        std::cout << "Successfully loaded OBJ file: " << objFile << std::endl;
        model.generateNormals();
        
        // Debug: Print model bounds
        const auto& vertices = model.getVertices();
        if (!vertices.empty()) {
            Vec3 minBounds = vertices[0];
            Vec3 maxBounds = vertices[0];
            for (const auto& vertex : vertices) {
                minBounds.x = std::min(minBounds.x, vertex.x);
                minBounds.y = std::min(minBounds.y, vertex.y);
                minBounds.z = std::min(minBounds.z, vertex.z);
                maxBounds.x = std::max(maxBounds.x, vertex.x);
                maxBounds.y = std::max(maxBounds.y, vertex.y);
                maxBounds.z = std::max(maxBounds.z, vertex.z);
            }
            std::cout << "Model bounds: Min(" << minBounds.x << ", " << minBounds.y << ", " << minBounds.z << ")";
            std::cout << " Max(" << maxBounds.x << ", " << maxBounds.y << ", " << maxBounds.z << ")" << std::endl;
            
            Vec3 center = Vec3((minBounds.x + maxBounds.x) * 0.5f, 
                              (minBounds.y + maxBounds.y) * 0.5f, 
                              (minBounds.z + maxBounds.z) * 0.5f);
            Vec3 size = maxBounds - minBounds;
            std::cout << "Model center: (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
            std::cout << "Model size: (" << size.x << ", " << size.y << ", " << size.z << ")" << std::endl;
            
            // Set material properties for the car
            renderer.shader.material.diffuse = Color(150, 150, 200);
            renderer.shader.material.specular = Color(255, 255, 255);
            renderer.shader.material.ambient = Color(30, 30, 50);
            
            // Adjust camera for car model - position camera to view the entire model
            float maxDim = std::max({size.x, size.y, size.z});
            // Move camera closer and at an angle for better view
            Vec3 cameraPos = center + Vec3(maxDim * 0.8f, maxDim * 0.3f, maxDim * 1.2f);
            Vec3 cameraTarget = center;
            
            renderer.shader.viewMatrix = Matrix4x4::lookAt(cameraPos, cameraTarget, cameraUp);
            renderer.shader.cameraPos = cameraPos;
            renderer.shader.updateMVP();
            
            // Clear and render the model
            renderer.framebuffer.clear(Color(20, 30, 50));
            renderer.renderModel(model);
            objLoaded = true;
        }
    }
    
    if (!objLoaded) {
        std::cout << "Creating test cube..." << std::endl;
        createTestCube(renderer);
    }
    
    // Render the scene
    std::cout << "Rendering..." << std::endl;
    renderer.render();
    
    std::cout << "Render complete! Output saved to output.ppm" << std::endl;
    
    return 0;
}

// Function to create a simple test cube
void createTestCube(Renderer& renderer) {
    // Clear framebuffer
    renderer.framebuffer.clear(Color(50, 50, 100));
    
    // Define cube vertices in screen space (simplified for demonstration)
    Vec3 cubeVerts[8] = {
        Vec3(-1, -1, -1), Vec3(1, -1, -1), Vec3(1, 1, -1), Vec3(-1, 1, -1),
        Vec3(-1, -1, 1),  Vec3(1, -1, 1),  Vec3(1, 1, 1),  Vec3(-1, 1, 1)
    };
    
    // Transform vertices to screen space
    for (int i = 0; i < 8; i++) {
        cubeVerts[i] = renderer.shader.mvpMatrix.transform(cubeVerts[i]);
        // Convert to screen coordinates
        cubeVerts[i].x = (cubeVerts[i].x + 1.0f) * renderer.width * 0.5f;
        cubeVerts[i].y = (cubeVerts[i].y + 1.0f) * renderer.height * 0.5f;
    }
    
    // Define cube faces (indices into cubeVerts array)
    int cubeFaces[12][3] = {
        // Front face
        {0, 1, 2}, {0, 2, 3},
        // Back face
        {4, 6, 5}, {4, 7, 6},
        // Left face
        {0, 3, 7}, {0, 7, 4},
        // Right face
        {1, 5, 6}, {1, 6, 2},
        // Top face
        {3, 2, 6}, {3, 6, 7},
        // Bottom face
        {0, 4, 5}, {0, 5, 1}
    };
    
    // Draw cube faces
    Color faceColors[6] = {
        Color(255, 0, 0),   // Red - front
        Color(0, 255, 0),   // Green - back
        Color(0, 0, 255),   // Blue - left
        Color(255, 255, 0), // Yellow - right
        Color(255, 0, 255), // Magenta - top
        Color(0, 255, 255)  // Cyan - bottom
    };
    
    for (int face = 0; face < 12; face++) {
        Color color = faceColors[face / 2];
        
        // Apply simple lighting
        float intensity = 0.7f + 0.3f * ((face % 6) / 6.0f);
        color = color * intensity;
        
        renderer.framebuffer.drawTriangle(
            cubeVerts[cubeFaces[face][0]],
            cubeVerts[cubeFaces[face][1]],
            cubeVerts[cubeFaces[face][2]],
            color
        );
    }
    
    // Draw wireframe for demonstration of Bresenham line algorithm
    Color wireframeColor(255, 255, 255);
    int edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0}, // Front face edges
        {4,5}, {5,6}, {6,7}, {7,4}, // Back face edges
        {0,4}, {1,5}, {2,6}, {3,7}  // Connecting edges
    };
    
    for (int i = 0; i < 12; i++) {
        Vec3 v1 = cubeVerts[edges[i][0]];
        Vec3 v2 = cubeVerts[edges[i][1]];
        renderer.framebuffer.drawLine((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, wireframeColor);
    }
}
