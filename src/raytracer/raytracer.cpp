#include "raytracer.h"
#include "geometry/primitive.h"
#include "ray.hpp"
#include "raytracescene.h"
#include <cmath>
#include <iostream>

RayTracer::RayTracer(Config config) : m_config(config) {}

RGBA toRGBA(const glm::vec4 &illumination) {
  uint8_t returnR = 255 * std::min(std::max(illumination[0], 0.0f), 1.0f);
  uint8_t returnG = 255 * std::min(std::max(illumination[1], 0.0f), 1.0f);
  uint8_t returnB = 255 * std::min(std::max(illumination[2], 0.0f), 1.0f);
  return RGBA{returnR, returnG, returnB, 255};
}

void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
  Camera camera = scene.getCamera();
  glm::mat4 inverseViewMatrix = glm::inverse(camera.getViewMatrix());
  std::vector<glm::mat4> CTMs;
  std::vector<glm::mat4> inverseCTMs;
  std::vector<SceneLightData> lights = scene.getLights();
  std::vector<Primitive *> primitives = scene.getPrimitives();
  SceneGlobalData globalData = scene.getGlobalData();
  int width = scene.width();
  int height = scene.height();
  float ka = globalData.ka;
  float kd = globalData.kd;
  float ks = globalData.ks;
  for (int i = 0; i < primitives.size(); i++) {
    CTMs.push_back(primitives[i]->getCTM());
    inverseCTMs.push_back(glm::inverse(primitives[i]->getCTM()));
  }
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      float x = (i + 0.5) / width - 0.5;
      float y = (height - 1 - j + 0.5) / height - 0.5;
      float heightAngle = camera.getHeightAngle();
      float widthAngle = camera.getWidthAngle();
      float k = 1;
      glm::vec3 origin = glm::vec3(inverseViewMatrix * glm::vec4(0, 0, 0, 1));
      glm::vec3 direction = glm::vec3(
          inverseViewMatrix * glm::vec4(2 * x * tan(widthAngle / 2),
                                        2 * y * tan(heightAngle / 2), -k, 0));
      float smallestT = INFINITY;
      for (int p = 0; p < primitives.size(); p++) {
        Primitive *primitive = primitives[p];
        Ray ray(inverseCTMs[p] * glm::vec4(origin, 1),
                inverseCTMs[p] * glm::vec4(direction, 0));
        float t = primitive->intersect(ray);
        if (t > 0 && t < smallestT) {
          smallestT = t;
          SceneMaterial material = primitive->getMaterial();
          glm::vec3 point = inverseCTMs[p] * glm::vec4(origin, 1) +
                            t * inverseCTMs[p] * glm::vec4(direction, 0);
          glm::vec3 normal = primitive->getNormal(point);
          glm::mat3 invTrans = glm::inverse(glm::transpose(CTMs[p]));
          glm::vec3 worldNormal = glm::normalize(invTrans * normal);
          glm::vec3 directionToCamera = glm::normalize(-direction);
          glm::vec4 illumination = glm::vec4(0, 0, 0, 1);
          illumination += material.cAmbient * ka;
          for (int l = 0; l < lights.size(); l++) {
            SceneLightData light = lights[l];
            glm::vec3 directionToLight = glm::normalize(-light.dir);
            glm::vec3 aboutNormal = glm::normalize(
                2 * glm::dot(worldNormal, directionToLight) * worldNormal -
                directionToLight);
            float diffuseFactor = glm::dot(worldNormal, directionToLight);
            float specularFactor = glm::dot(aboutNormal, directionToCamera);
            if (diffuseFactor > 0) {
              illumination +=
                  diffuseFactor * material.cDiffuse * kd * light.color;
            }
            if (specularFactor > 0) {
              illumination +=
                  static_cast<float>(pow(specularFactor, material.shininess)) *
                  material.cSpecular * ks * light.color;
            }
          }
          imageData[j * width + i] = toRGBA(illumination);
        }
      }
    }
  }
}