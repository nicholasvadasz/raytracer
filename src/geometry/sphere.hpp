#pragma once

#include "primitive.h"
#include <iostream>

class Sphere : public Primitive {
private:
  SceneMaterial material;
  glm::mat4 ctm;
  QImage texture;
  float repeatU;
  float repeatV;

public:
  Sphere(SceneMaterial m, glm::mat4 c) {
    this->material = m;
    this->ctm = c;
    if (m.textureMap.filename != "") {
      repeatU = m.textureMap.repeatU;
      repeatV = m.textureMap.repeatV;
      texture = QImage(QString::fromStdString(m.textureMap.filename));
      if (texture.isNull()) {
        std::cout << "Failed to load texture: " << m.textureMap.filename
                  << std::endl;
      }
    }
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

  RGBA getTextureColor(glm::vec3 point) {
    if (texture.isNull()) {
      return RGBA{0, 0, 0, 0};
    }
    float theta = atan2(point.z, point.x);
    float u = 0;
    if (theta < 0) {
      u = -theta / (2 * M_PI);
    } else {
      u = 1 - (theta / (2 * M_PI));
    }
    float phi = asin(point.y / .5);
    float v = (phi / M_PI) + .5;
    if (u == 1) {
      u -= 1.0 / texture.width();
    }
    if (v == 0) {
      v -= 1.0 / texture.height();
    }
    QColor color = texture.pixelColor(
        (int)floor(u * texture.width() * repeatU) % texture.width(),
        (int)floor((1 - v) * texture.height() * repeatV) % texture.height());
    return RGBA{static_cast<uint8_t>(color.red()),
                static_cast<uint8_t>(color.green()),
                static_cast<uint8_t>(color.blue()), 255};
  }
};
