#pragma once

#include "primitive.h"
#include <iostream>

class Cube : public Primitive {
private:
  SceneMaterial material;
  glm::mat4 ctm;

public:
  Cube(SceneMaterial m, glm::mat4 c) {
    this->material = m;
    this->ctm = c;
  };
  float intersect(Ray &ray) {
    glm::vec3 origin = ray.origin;
    glm::vec3 direction = ray.direction;
    float tmin = -INFINITY;
    float tmax = INFINITY;
    for (int i = 0; i < 3; i++) {
      float t1 = (-0.5 - origin[i]) / direction[i];
      float t2 = (0.5 - origin[i]) / direction[i];
      if (t1 > t2) {
        float temp = t1;
        t1 = t2;
        t2 = temp;
      }
      if (t1 > tmin) {
        tmin = t1;
      }
      if (t2 < tmax) {
        tmax = t2;
      }
    }
    if (tmin > tmax) {
      return -1;
    }
    return tmin;
  }

  glm::mat4 getCTM() { return ctm; }
  SceneMaterial getMaterial() { return material; }

  glm::vec3 getNormal(glm::vec3 point) {
    glm::vec3 normal = glm::vec3(0, 0, 0);
    float x = point.x;
    float y = point.y;
    float z = point.z;
    if (x >= 0.499) {
      normal = glm::vec3(1, 0, 0);
    } else if (x <= -0.499) {
      normal = glm::vec3(-1, 0, 0);
    } else if (y >= 0.499) {
      normal = glm::vec3(0, 1, 0);
    } else if (y <= -0.499) {
      normal = glm::vec3(0, -1, 0);
    } else if (z >= 0.499) {
      normal = glm::vec3(0, 0, 1);
    } else if (z <= -0.499) {
      normal = glm::vec3(0, 0, -1);
    }
    return normal;
  }
};
