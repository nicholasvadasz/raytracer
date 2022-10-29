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
  void print() {
    std::cout << "Ray: " << std::endl;
    std::cout << "Origin: " << origin.x << ", " << origin.y << ", " << origin.z
              << std::endl;
    std::cout << "Direction: " << direction.x << ", " << direction.y << ", "
              << direction.z << std::endl;
  }
};