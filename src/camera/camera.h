#pragma once

#include <glm/glm.hpp>
#include <utils/sceneparser.h>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions
// below are all optional / for your convenience. You can either implement and
// use these getters, or make your own design. If you decide to make your own
// design, feel free to delete these as TAs won't rely on them to grade your
// assignments.

class Camera {
private:
  glm::mat4 viewMatrix;
  float aspectRatio;
  float heightAngle;

public:
  // Returns the view matrix for the current camera settings.
  // You might also want to define another function that return the inverse of
  // the view matrix.

  Camera(const RenderData &metaData, int width, int height);

  glm::vec3 getPos() const;

  glm::mat4 getViewMatrix() const;

  glm::mat4 getInverseViewMatrix() const;

  // Returns the aspect ratio of the camera.
  float getAspectRatio() const;

  // Returns the height angle of the camera in RADIANS.
  float getHeightAngle() const;

  float getWidthAngle() const;

  // Returns the focal length of this camera.
  // This is for the depth of field extra-credit feature only;
  // You can ignore if you are not attempting to implement depth of field.
  float getFocalLength() const;

  // Returns the focal length of this camera.
  // This is for the depth of field extra-credit feature only;
  // You can ignore if you are not attempting to implement depth of field.
  float getAperture() const;
};
