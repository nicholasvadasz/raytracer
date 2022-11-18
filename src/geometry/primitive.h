#pragma once

#include "raytracer/ray.hpp"
#include "utils/rgba.h"
#include "utils/scenedata.h"
#include <QColor>
#include <QImage>

class Primitive {
public:
  virtual float intersect(Ray &ray) = 0;
  virtual glm::vec3 getNormal(glm::vec3 point) = 0;
  virtual glm::mat4 getCTM() = 0;
  virtual SceneMaterial getMaterial() = 0;
  virtual RGBA getTextureColor(glm::vec3 point) = 0;
};
