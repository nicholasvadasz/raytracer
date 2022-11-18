#pragma once

#include "geometry/primitive.h"
#include "ray.hpp"
#include "raytracescene.h"
#include "utils/rgba.h"
#include <glm/glm.hpp>

// A forward declaration for the RaytraceScene class

class RayTraceScene;

// A class representing a ray-tracer

class RayTracer {
public:
  struct Config {
    bool enableShadow = false;
    bool enableReflection = false;
    bool enableRefraction = false;
    bool enableTextureMap = false;
    bool enableTextureFilter = false;
    bool enableParallelism = false;
    bool enableSuperSample = false;
    bool enableAcceleration = false;
    bool enableDepthOfField = false;
  };

public:
  RayTracer(Config config);

  // Renders the scene synchronously.
  // The ray-tracer will render the scene and fill imageData in-place.
  // @param imageData The pointer to the imageData to be filled.
  // @param scene The scene to be rendered.
  void render(RGBA *imageData, const RayTraceScene &scene);
  void calcPhong(const glm::vec3 worldNormal, const glm::vec3 directionToCamera,
                 const glm::vec3 point, const SceneMaterial material,
                 const SceneLightData light, const float ka, const float kd,
                 const float ks, glm::vec4 &illumination, RGBA curColor,
                 float blend);
  glm::vec4 traceRay(Ray &reflectedRayInWorld, const RayTraceScene &scene,
                     glm::vec3 cameraPos, std::vector<glm::mat4> CTMs,
                     std::vector<glm::mat4> inverseCTMs,
                     std::vector<glm::mat3> transInverses,
                     std::vector<SceneLightData> lights,
                     std::vector<Primitive *> primitives, float ka, float kd,
                     float ks, int depth);

private:
  const Config m_config;
};
