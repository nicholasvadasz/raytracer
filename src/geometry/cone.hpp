#pragma once

#include "primitive.h"
#include <iostream>

class Cone : public Primitive {
private:
  SceneMaterial material;
  glm::mat4 ctm;

public:
  Cone(SceneMaterial m, glm::mat4 c) {
    this->material = m;
    this->ctm = c;
  };
  float intersect(Ray &ray) {
    glm::vec3 origin = ray.origin;
    glm::vec3 direction = ray.direction;
    float a = direction.x * direction.x - direction.y * direction.y * .25 +
              direction.z * direction.z;
    float b = 2 * origin.x * direction.x + 2 * origin.z * direction.z -
              .5 * origin.y * direction.y + .25 * direction.y;
    float c = origin.x * origin.x + origin.z * origin.z -
              .25 * origin.y * origin.y + .25 * origin.y - .0625;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
      return -1;
    }
    float t1 = (-b + sqrt(discriminant)) / (2 * a);
    float t2 = (-b - sqrt(discriminant)) / (2 * a);
    float curT = -1;
    if (t1 > 0 || t2 > 0) {
      curT = std::min(t1, t2);
    }
    float t3 = (-0.5 - origin.y) / direction.y;
    float x3 = origin.x + t3 * direction.x;
    float z3 = origin.z + t3 * direction.z;
    if (x3 * x3 + z3 * z3 <= 0.25) {
      if (curT == -1) {
        return t3;
      } else {
        return std::min(curT, t3);
      }
    }
    glm::vec3 point = origin + curT * direction;
    if (point.y > .5 || point.y < -.5) {
      return -1;
    }
    return curT;
  }

  glm::mat4 getCTM() { return ctm; }
  SceneMaterial getMaterial() { return material; }

  glm::vec3 getNormal(glm::vec3 point) {
    glm::vec3 normal = glm::normalize(glm::vec3(point.x, 0, point.z));
    if (point.y <= -.5) {
      normal = glm::vec3(0, -1, 0);
    } else {
      normal.y = .5;
    }
    return normal;
  }
};
