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

void RayTracer::calcPhong(const glm::vec3 worldNormal,
                          const glm::vec3 directionToCamera,
                          const glm::vec3 point, const SceneMaterial material,
                          const SceneLightData light, const float ka,
                          const float kd, const float ks,
                          glm::vec4 &illumination, RGBA curColor, float blend) {
  switch (light.type) {
  case LightType::LIGHT_DIRECTIONAL: {
    glm::vec3 directionToLight = glm::normalize(-light.dir);
    glm::vec3 aboutNormal = glm::normalize(
        2 * glm::dot(worldNormal, directionToLight) * worldNormal -
        directionToLight);
    float diffuseFactor = glm::dot(worldNormal, directionToLight);
    float specularFactor = glm::dot(aboutNormal, directionToCamera);
    if (diffuseFactor > 0) {
      illumination +=
          diffuseFactor *
          (blend * glm::vec4(curColor.r / 255.0 * 2.0, curColor.g / 255.0 * 2.0,
                             curColor.b / 255.0 * 2.0,
                             curColor.a / 255.0 * 2.0) +
           material.cDiffuse * (1 - blend)) *
          kd * light.color;
    }
    if (specularFactor > 0) {
      illumination +=
          static_cast<float>(pow(specularFactor, material.shininess)) *
          material.cSpecular * ks * light.color;
    }
    break;
  }
  case LightType::LIGHT_POINT: {
    float distance = glm::distance(glm::vec3(light.pos), point);
    float attenuation = std::min(
        1.0f, 1.0f / (light.function[0] + light.function[1] * distance +
                      light.function[2] * distance * distance));
    glm::vec3 directionToLight = glm::normalize(glm::vec3(light.pos) - point);
    glm::vec3 aboutNormal = glm::normalize(
        2 * glm::dot(worldNormal, directionToLight) * worldNormal -
        directionToLight);
    float diffuseFactor = glm::dot(worldNormal, directionToLight);
    float specularFactor = glm::dot(aboutNormal, directionToCamera);
    if (diffuseFactor > 0) {
      illumination +=
          diffuseFactor *
          (blend * glm::vec4(curColor.r / 255.0 * 2.0, curColor.g / 255.0 * 2.0,
                             curColor.b / 255.0 * 2.0,
                             curColor.a / 255.0 * 2.0) +
           material.cDiffuse * (1 - blend)) *
          kd * light.color * attenuation;
    }
    if (specularFactor > 0) {
      illumination +=
          static_cast<float>(pow(specularFactor, material.shininess)) *
          material.cSpecular * ks * light.color * attenuation;
    }
    break;
  }
  case LightType::LIGHT_SPOT: {
    glm::vec3 currentDirection = glm::normalize(point - glm::vec3(light.pos));
    float distance = glm::distance(glm::vec3(light.pos), point);
    float fatt = std::min(
        1.0f, 1.0f / (light.function[0] + light.function[1] * distance +
                      light.function[2] * distance * distance));
    float x = glm::acos(
        glm::dot(currentDirection, glm::normalize(glm::vec3(light.dir))));
    float thetaOuter = light.angle;
    float thetaInner = light.angle - light.penumbra;
    float attenuation = 0;
    if (x <= thetaInner) {
      attenuation = 1;
    } else if (thetaInner < x && x <= thetaOuter) {
      attenuation =
          1 - (-2 * pow((x - thetaInner) / (thetaOuter - thetaInner), 3) +
               3 * pow((x - thetaInner) / (thetaOuter - thetaInner), 2));
    } else {
      attenuation = 0;
    }
    glm::vec3 directionToLight = glm::normalize(glm::vec3(light.pos) - point);
    glm::vec3 aboutNormal = glm::normalize(
        2 * glm::dot(worldNormal, directionToLight) * worldNormal -
        directionToLight);
    float diffuseFactor = glm::dot(worldNormal, directionToLight);
    float specularFactor = glm::dot(aboutNormal, directionToCamera);
    if (diffuseFactor > 0) {
      illumination +=
          diffuseFactor *
          (blend * glm::vec4(curColor.r / 255.0 * 2.0, curColor.g / 255.0 * 2.0,
                             curColor.b / 255.0 * 2.0,
                             curColor.a / 255.0 * 2.0) +
           material.cDiffuse * (1 - blend)) *
          kd * light.color * attenuation;
    }
    if (specularFactor > 0) {
      illumination +=
          static_cast<float>(pow(specularFactor, material.shininess)) *
          material.cSpecular * ks * light.color * attenuation * fatt;
    }
    break;
  }
  default:
    break;
  }
}

glm::vec4 RayTracer::traceRay(Ray &reflectedRayInWorld,
                              const RayTraceScene &scene, glm::vec3 cameraPos,
                              std::vector<glm::mat4> CTMs,
                              std::vector<glm::mat4> inverseCTMs,
                              std::vector<glm::mat3> transInverses,
                              std::vector<SceneLightData> lights,
                              std::vector<Primitive *> primitives, float ka,
                              float kd, float ks, int depth) {
  if (depth == 0) {
    return glm::vec4(0, 0, 0, 0);
  }
  float smallestT = INFINITY;
  glm::vec3 worldIntersectionPoint;
  glm::vec4 myIllumination(0, 0, 0, 0);
  for (int p = 0; p < primitives.size(); p++) {
    Primitive *primitive = primitives[p];
    Ray ray(inverseCTMs[p] * glm::vec4(reflectedRayInWorld.origin, 1),
            inverseCTMs[p] * glm::vec4(reflectedRayInWorld.direction, 0));
    float t = primitive->intersect(ray);
    if (t > 0 && t < smallestT) {
      smallestT = t;
      SceneMaterial material = primitive->getMaterial();
      glm::vec3 objectPoint =
          inverseCTMs[p] * glm::vec4(reflectedRayInWorld.origin, 1) +
          t * inverseCTMs[p] * glm::vec4(reflectedRayInWorld.direction, 0);
      glm::vec3 normal = primitive->getNormal(objectPoint);
      glm::mat3 invTrans = transInverses[p];
      glm::vec3 worldNormal = glm::normalize(invTrans * normal);
      glm::vec3 directionToCamera = glm::normalize(cameraPos - objectPoint);
      myIllumination = glm::vec4(0, 0, 0, 0);
      myIllumination += material.cAmbient * ka;
      for (int l = 0; l < lights.size(); l++) {
        SceneLightData light = lights[l];
        worldIntersectionPoint = CTMs[p] * glm::vec4(objectPoint, 1);
        glm::vec3 shadowRayDirection = glm::vec3(0, 0, 0);
        if (light.type == LightType::LIGHT_POINT ||
            light.type == LightType::LIGHT_SPOT) {
          shadowRayDirection =
              glm::normalize(glm::vec3(light.pos) - worldIntersectionPoint);
        } else {
          shadowRayDirection = -glm::normalize(glm::vec3(light.dir));
        }
        Ray shadowRay =
            Ray(worldIntersectionPoint + 0.001f * shadowRayDirection,
                shadowRayDirection);
        bool obstructed = false;
        for (int p = 0; p < primitives.size(); p++) {
          Primitive *primitive = primitives[p];
          Ray shadowRayObject =
              Ray(inverseCTMs[p] * glm::vec4(shadowRay.origin, 1),
                  inverseCTMs[p] * glm::vec4(shadowRay.direction, 0));
          float t = primitive->intersect(shadowRayObject);
          if (t > 0) {
            obstructed = true;
            break;
          }
        }
        if (!obstructed) {
          RGBA curColor = primitive->getTextureColor(objectPoint);
          float blend = material.blend;
          calcPhong(worldNormal, directionToCamera, worldIntersectionPoint,
                    material, light, ka, kd, ks, myIllumination, curColor,
                    blend);
        }
      }
      if (depth >= 1 && material.cReflective != glm::vec4(0, 0, 0, 0)) {
        Ray reflectedRay =
            Ray(worldIntersectionPoint + 0.001f * worldNormal,
                glm::reflect(reflectedRayInWorld.direction, worldNormal));
        myIllumination +=
            material.cReflective * ks *
            traceRay(reflectedRay, scene, cameraPos, CTMs, inverseCTMs,
                     transInverses, lights, primitives, ka, kd, ks, depth - 1);
      }
    }
  }
  return myIllumination;
}

void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {
#pragma omp parallel
  Camera camera = scene.getCamera();
  glm::mat4 inverseViewMatrix = glm::inverse(camera.getViewMatrix());
  std::vector<glm::mat4> CTMs;
  std::vector<glm::mat4> inverseCTMs;
  std::vector<glm::mat3> transInverses;
  std::vector<SceneLightData> lights = scene.getLights();
  std::vector<Primitive *> primitives = scene.getPrimitives();
  SceneGlobalData globalData = scene.getGlobalData();
  int width = scene.width();
  int height = scene.height();
  float ka = globalData.ka;
  float kd = globalData.kd;
  float ks = globalData.ks;
  for (int i = 0; i < primitives.size(); i++) {
    glm::mat4 ctm = primitives[i]->getCTM();
    glm::mat4 inverseCTM = glm::inverse(ctm);
    CTMs.push_back(ctm);
    inverseCTMs.push_back(inverseCTM);
    transInverses.push_back(glm::transpose(inverseCTM));
  }
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      float x = (i + 0.5) / width - 0.5;
      float y = (height - 1 - j + 0.5) / height - 0.5;
      float heightAngle = camera.getHeightAngle();
      float widthAngle = camera.getWidthAngle();
      float k = 1;
      glm::vec3 worldIntersectionPoint;
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
          glm::vec3 objectPoint = inverseCTMs[p] * glm::vec4(origin, 1) +
                                  t * inverseCTMs[p] * glm::vec4(direction, 0);
          glm::vec3 normal = primitive->getNormal(objectPoint);
          glm::mat3 invTrans = transInverses[p];
          glm::vec3 worldNormal = glm::normalize(invTrans * normal);
          glm::vec3 directionToCamera = glm::normalize(-direction);
          glm::vec4 illumination = glm::vec4(0, 0, 0, 0);
          illumination += material.cAmbient * ka;
          for (int l = 0; l < lights.size(); l++) {
            SceneLightData light = lights[l];
            worldIntersectionPoint = CTMs[p] * glm::vec4(objectPoint, 1);
            glm::vec3 shadowRayDirection = glm::vec3(0, 0, 0);
            if (light.type == LightType::LIGHT_POINT ||
                light.type == LightType::LIGHT_SPOT) {
              shadowRayDirection =
                  glm::normalize(glm::vec3(light.pos) - worldIntersectionPoint);
            } else {
              shadowRayDirection = -glm::normalize(glm::vec3(light.dir));
            }
            Ray shadowRay =
                Ray(worldIntersectionPoint + 0.001f * shadowRayDirection,
                    shadowRayDirection);
            bool obstructed = false;
            for (int p = 0; p < primitives.size(); p++) {
              Primitive *primitive = primitives[p];
              Ray shadowRayObject =
                  Ray(inverseCTMs[p] * glm::vec4(shadowRay.origin, 1),
                      inverseCTMs[p] * glm::vec4(shadowRay.direction, 0));
              float t = primitive->intersect(shadowRayObject);
              if (t > 0) {
                obstructed = true;
                break;
              }
            }
            if (!obstructed) {
              RGBA curColor = primitive->getTextureColor(objectPoint);
              float blend = material.blend;
              calcPhong(worldNormal, directionToCamera, worldIntersectionPoint,
                        material, light, ka, kd, ks, illumination, curColor,
                        blend);
            }
          }
          if (material.cReflective != glm::vec4(0, 0, 0, 0)) {
            Ray incidentRay = Ray(worldIntersectionPoint, -directionToCamera);
            Ray reflectedRay =
                Ray(worldIntersectionPoint,
                    glm::reflect(incidentRay.direction, worldNormal));
            illumination +=
                material.cReflective * ks *
                traceRay(reflectedRay, scene, origin, CTMs, inverseCTMs,
                         transInverses, lights, primitives, ka, kd, ks, 4);
          }
          if (material.cTransparent != glm::vec4(0, 0, 0, 0)) {
            float n1 = 1;
            float n2 = material.ior;
            float n = n1 / n2;
            Ray incidentRay = Ray(worldIntersectionPoint, -directionToCamera);
            glm::vec3 normal = worldNormal;
            float cosTheta1 = glm::dot(incidentRay.direction, normal);
            float cosTheta2 = sqrt(1 - n * n * (1 - cosTheta1 * cosTheta1));
            glm::vec3 refractedDirection = n * incidentRay.direction +
                                           (n * cosTheta1 - cosTheta2) * normal;
            Ray refractedRay =
                Ray(worldIntersectionPoint + .001f * refractedDirection,
                    refractedDirection);
            illumination +=
                material.cTransparent * ks *
                traceRay(refractedRay, scene, origin, CTMs, inverseCTMs,
                         transInverses, lights, primitives, ka, kd, ks, 4);
          }
          imageData[j * width + i] = toRGBA(illumination);
        }
      }
    }
  }
}
