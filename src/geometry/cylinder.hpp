#pragma once

#include "primitive.h"
#include <iostream>

class Cylinder : public Primitive {
private:
  SceneMaterial material;
  glm::mat4 ctm;
  QImage texture;
  float repeatU;
  float repeatV;

public:
  Cylinder(SceneMaterial m, glm::mat4 c) {
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
    float a = direction.x * direction.x + direction.z * direction.z;
    float b = 2 * (origin.x * direction.x + origin.z * direction.z);
    float c = origin.x * origin.x + origin.z * origin.z - 0.25;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
      return -1;
    }
    float t1 = (-b + sqrt(discriminant)) / (2 * a);
    float t2 = (-b - sqrt(discriminant)) / (2 * a);
    float y1 = origin.y + t1 * direction.y;
    float y2 = origin.y + t2 * direction.y;
    float curT = -1;
    if (y1 >= -0.5 && y1 <= 0.5) {
      curT = t1;
    }
    if (y2 >= -0.5 && y2 <= 0.5) {
      if (curT == -1) {
        curT = t2;
      } else {
        curT = std::min(curT, t2);
      }
    }
    float t3 = (-0.5 - origin.y) / direction.y;
    float t4 = (0.5 - origin.y) / direction.y;
    float x3 = origin.x + t3 * direction.x;
    float z3 = origin.z + t3 * direction.z;
    float x4 = origin.x + t4 * direction.x;
    float z4 = origin.z + t4 * direction.z;
    if (x3 * x3 + z3 * z3 <= 0.25) {
      if (curT == -1) {
        curT = t3;
      } else {
        curT = std::min(curT, t3);
      }
    }
    if (x4 * x4 + z4 * z4 <= 0.25) {
      if (curT == -1) {
        curT = t4;
      } else {
        curT = std::min(curT, t4);
      }
    }
    return curT;
  }

  glm::mat4 getCTM() { return ctm; }
  SceneMaterial getMaterial() { return material; }

  glm::vec3 getNormal(glm::vec3 point) {
    glm::vec3 normal = glm::vec3(0, 0, 0);
    if (point.y >= .499) {
      normal = glm::vec3(0, 1, 0);
    } else if (point.y <= -0.499) {
      normal = glm::vec3(0, -1, 0);
    } else {
      normal = glm::normalize(glm::vec3(point.x, 0, point.z));
    }
    return normal;
  }

  RGBA getTextureColor(glm::vec3 point) {
    if (texture.isNull()) {
      return RGBA{0, 0, 0, 0};
    }
    float u, v;
    if (point.y >= 0.499) {
      u = (point.x + 0.5) / 1;
      v = (-point.z + 0.5) / 1;
    } else if (point.y <= -0.499) {
      u = (point.x + 0.5) / 1;
      v = (point.z + 0.5) / 1;
    } else {
      float theta = atan2(point.z, point.x);
      if (theta < 0) {
        u = -theta / (2 * M_PI);
      } else {
        u = 1 - (theta / (2 * M_PI));
      }
      v = (point.y + 0.5) / 1;
    }
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