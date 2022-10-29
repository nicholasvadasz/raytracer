#pragma once

#include "camera/camera.h"
#include "geometry/primitive.h"
#include "utils/scenedata.h"
#include "utils/sceneparser.h"

// A class representing a scene to be ray-traced

// Feel free to make your own design choices for RayTraceScene, the functions
// below are all optional / for your convenience. You can either implement and
// use these getters, or make your own design. If you decide to make your own
// design, feel free to delete these as TAs won't rely on them to grade your
// assignments.

class RayTraceScene {
private:
  Camera sceneCamera;
  SceneGlobalData sceneGlobalData;
  std::vector<Primitive *> scenePrimitives;
  std::vector<SceneLightData> lights;
  int sceneWidth;
  int sceneHeight;

public:
  RayTraceScene(int width, int height, const RenderData &metaData);

  const int &width() const;

  const int &height() const;

  const SceneGlobalData &getGlobalData() const;

  const Camera &getCamera() const;

  const std::vector<Primitive *> getPrimitives() const;

  const std::vector<SceneLightData> getLights() const;
};
