#ifndef SHADER_H
#define SHADER_H

#include "Vec3.h"
#include "Matrix4x4.h"

// Vertex shader output / Fragment shader input
struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;
    Vec3 worldPos;
    Vec3 color;
};

// Light structure
struct Light {
    Vec3 position;
    Vec3 direction;
    Color color;
    float intensity;
    int type; // 0 = directional, 1 = point, 2 = spot
    
    Light() : position(0, 0, 0), direction(0, -1, 0), color(255, 255, 255), 
              intensity(1.0f), type(0) {}
};

// Material properties
struct Material {
    Color diffuse;
    Color specular;
    Color ambient;
    float shininess;
    float roughness;
    
    Material() : diffuse(128, 128, 128), specular(255, 255, 255), 
                 ambient(32, 32, 32), shininess(32.0f), roughness(0.5f) {}
};

class Shader {
public:
    Matrix4x4 modelMatrix;
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    Matrix4x4 mvpMatrix;
    
    Vec3 cameraPos;
    std::vector<Light> lights;
    Material material;
    
    // Shadow mapping
    Matrix4x4 lightSpaceMatrix;
    bool enableShadows;
    
    // Ambient occlusion
    bool enableAO;
    float aoRadius;
    int aoSamples;

    Shader() : enableShadows(false), enableAO(false), aoRadius(1.0f), aoSamples(16) {
        // Default light
        Light defaultLight;
        defaultLight.direction = Vec3(0, -1, -1).normalize();
        defaultLight.intensity = 1.0f;
        lights.push_back(defaultLight);
    }

    void updateMVP() {
        mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    }

    // Vertex shader - transforms vertices to screen space
    Vertex vertexShader(const Vec3& vertex, const Vec3& normal, const Vec2& texCoord) {
        Vertex output;
        
        // Transform to world space
        output.worldPos = modelMatrix.transform(vertex);
        output.normal = modelMatrix.transform(normal, 0.0f).normalize(); // Transform normal (w=0)
        output.texCoord = texCoord;
        
        // Transform to clip space
        Vec3 clipPos = mvpMatrix.transform(vertex);
        
        // Store the clip space position (NDC coordinates)
        output.position = clipPos;
        
        return output;
    }

    // Fragment shader - calculates pixel color
    Color fragmentShader(const Vertex& vertex) {
        Vec3 finalColor(0, 0, 0);
        
        // Ambient lighting
        Vec3 ambient = Vec3(material.ambient.r, material.ambient.g, material.ambient.b) * (1.0f / 255.0f);
        finalColor = finalColor + ambient;
        
        for (const Light& light : lights) {
            Vec3 lightDir;
            float attenuation = 1.0f;
            
            if (light.type == 0) { // Directional light
                lightDir = light.direction * -1.0f;
            } else { // Point light
                lightDir = (light.position - vertex.worldPos).normalize();
                float distance = (light.position - vertex.worldPos).length();
                attenuation = 1.0f / (1.0f + 0.09f * distance + 0.032f * distance * distance);
            }
            
            // Diffuse lighting
            float diff = std::max(0.0f, vertex.normal.dot(lightDir));
            Vec3 diffuse = Vec3(material.diffuse.r, material.diffuse.g, material.diffuse.b) * (1.0f / 255.0f) * diff;
            
            // Specular lighting (Blinn-Phong)
            Vec3 viewDir = (cameraPos - vertex.worldPos).normalize();
            Vec3 halfwayDir = (lightDir + viewDir).normalize();
            float spec = std::pow(std::max(0.0f, vertex.normal.dot(halfwayDir)), material.shininess);
            Vec3 specular = Vec3(material.specular.r, material.specular.g, material.specular.b) * (1.0f / 255.0f) * spec;
            
            Vec3 lightColor = Vec3(light.color.r, light.color.g, light.color.b) * (1.0f / 255.0f);
            finalColor = finalColor + (diffuse + specular) * lightColor * light.intensity * attenuation;
        }
        
        // Apply ambient occlusion if enabled
        if (enableAO) {
            float ao = calculateAmbientOcclusion(vertex);
            finalColor = finalColor * ao;
        }
        
        // Clamp and convert to color
        finalColor.x = std::min(1.0f, finalColor.x);
        finalColor.y = std::min(1.0f, finalColor.y);
        finalColor.z = std::min(1.0f, finalColor.z);
        
        return Color(
            (unsigned char)(finalColor.x * 255),
            (unsigned char)(finalColor.y * 255),
            (unsigned char)(finalColor.z * 255)
        );
    }

    // Simple ambient occlusion calculation
    float calculateAmbientOcclusion(const Vertex& vertex) {
        // This is a simplified AO calculation
        // In a real implementation, you'd sample the depth buffer around the fragment
        float occlusion = 1.0f;
        
        // Sample points around the fragment in screen space
        // This is a placeholder - real AO would require depth buffer sampling
        return std::max(0.1f, occlusion);
    }

    // Shadow mapping functions
    float calculateShadow(const Vertex& vertex) {
        if (!enableShadows) return 1.0f;
        
        // Transform fragment to light space
        Vec3 lightSpacePos = lightSpaceMatrix.transform(vertex.worldPos);
        
        // Perspective divide and transform to [0,1] range
        lightSpacePos.x = lightSpacePos.x * 0.5f + 0.5f;
        lightSpacePos.y = lightSpacePos.y * 0.5f + 0.5f;
        
        // Simple shadow test (would need shadow map texture in real implementation)
        return 1.0f; // No shadow for now
    }

    // Normal mapping (Lesson 6bis)
    Vec3 calculateNormalFromMap(const Vertex& vertex, const Vec3& normalMapSample) {
        // Calculate tangent space vectors
        // This is simplified - real implementation would need tangent vectors from the model
        Vec3 normal = vertex.normal;
        Vec3 tangent = Vec3(1, 0, 0); // Simplified tangent
        Vec3 bitangent = normal.cross(tangent);
        
        // Transform normal from tangent space to world space
        Matrix4x4 tbn; // Would construct TBN matrix here
        return normal; // Simplified - return original normal
    }
};

#endif
