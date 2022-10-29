#pragma once

#include "primitive.h"
#include <iostream>

class Sphere : public Primitive {
private:
  SceneMaterial material;
  glm::mat4 ctm;

public:
  Sphere(SceneMaterial m, glm::mat4 c) {
    this->material = m;
    this->ctm = c;
  };

  float intersect(Ray &ray) {
    glm::vec3 origin = ray.origin;
    glm::vec3 direction = ray.direction;
    float a = direction.x * direction.x + direction.y * direction.y +
              direction.z * direction.z;
    float b = 2 * (origin.x * direction.x + origin.y * direction.y +
                   origin.z * direction.z);
    float c =
        origin.x * origin.x + origin.y * origin.y + origin.z * origin.z - 0.25;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
      return -1;
    }
    float t1 = (-b + sqrt(discriminant)) / (2 * a);
    float t2 = (-b - sqrt(discriminant)) / (2 * a);
    if (t1 > 0 || t2 > 0) {
      return std::min(t1, t2);
    }
    return -1;
  }

  glm::mat4 getCTM() { return ctm; }
  SceneMaterial getMaterial() { return material; }

  glm::vec3 getNormal(glm::vec3 point) {
    glm::vec3 normal = glm::normalize(glm::vec3(point.x, point.y, point.z));
    return normal;
  }
};