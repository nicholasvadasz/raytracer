#pragma once

#include <glm/glm.hpp>
#include <iostream>

class Ray {
public:
  glm::vec3 origin;
  glm::vec3 direction;
  Ray(){};
  Ray(glm::vec3 origin, glm::vec3 direction) {
    this->origin = origin;
    this->direction = direction;
  }
};
