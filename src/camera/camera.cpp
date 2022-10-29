#include "camera.h"
#include "utils/sceneparser.h"
#include <iostream>
#include <stdexcept>

Camera::Camera(const RenderData &metaData, int width, int height) {
  glm::vec3 look = metaData.cameraData.look;
  glm::vec3 up = metaData.cameraData.up;
  glm::vec3 pos = metaData.cameraData.pos;
  glm::vec3 w = -glm::normalize(look);
  glm::vec3 temp = up - glm::dot(w, up) * w;
  glm::vec3 v = glm::normalize(temp);
  glm::vec3 u = glm::cross(v, w);
  glm::mat4 rotate = glm::mat4(u.x, v.x, w.x, 0, u.y, v.y, w.y, 0, u.z, v.z,
                               w.z, 0, 0, 0, 0, 1);
  viewMatrix = rotate * glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -pos.x,
                                  -pos.y, -pos.z, 1);
  heightAngle = metaData.cameraData.heightAngle;
  aspectRatio = (float)width / (float)height;
}

glm::vec3 Camera::getPos() const {
  glm::vec4 pos = viewMatrix * glm::vec4(0, 0, 0, 1);
  return glm::vec3(pos.x, pos.y, pos.z);
}

glm::mat4 Camera::getViewMatrix() const { return viewMatrix; }

glm::mat4 Camera::getInverseViewMatrix() const {
  return glm::inverse(viewMatrix);
}

float Camera::getAspectRatio() const { return aspectRatio; }

float Camera::getHeightAngle() const { return heightAngle; }

float Camera::getWidthAngle() const { return heightAngle * aspectRatio; }

float Camera::getFocalLength() const {
  throw std::runtime_error("not implemented");
}

float Camera::getAperture() const {
  throw std::runtime_error("not implemented");
}
