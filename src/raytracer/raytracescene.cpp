#include "raytracescene.h"
#include "camera/camera.h"
#include "geometry/cone.hpp"
#include "geometry/cube.hpp"
#include "geometry/cylinder.hpp"
#include "geometry/primitive.h"
#include "geometry/sphere.hpp"
#include "utils/sceneparser.h"

#include <iostream>
#include <stdexcept>

RayTraceScene::RayTraceScene(int width, int height, const RenderData &metaData)
    : sceneCamera(metaData, width, height) {
  sceneGlobalData = metaData.globalData;
  lights = metaData.lights;
  sceneWidth = width;
  sceneHeight = height;
  const std::vector<RenderShapeData> &shapes = metaData.shapes;
  for (int i = 0; i < shapes.size(); i++) {
    RenderShapeData shape = shapes[i];
    switch (shape.primitive.type) {
    case PrimitiveType::PRIMITIVE_SPHERE:
      scenePrimitives.push_back(
          new Sphere(shape.primitive.material, shape.ctm));
      break;
    case PrimitiveType::PRIMITIVE_CUBE:
      scenePrimitives.push_back(new Cube(shape.primitive.material, shape.ctm));
      break;
    case PrimitiveType::PRIMITIVE_CONE:
      scenePrimitives.push_back(new Cone(shape.primitive.material, shape.ctm));
      break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
      scenePrimitives.push_back(
          new Cylinder(shape.primitive.material, shape.ctm));
      break;
    default:
      throw std::runtime_error("unimplemented primitive type");
    }
  }
}

const int &RayTraceScene::width() const { return sceneWidth; }

const int &RayTraceScene::height() const { return sceneHeight; }

const SceneGlobalData &RayTraceScene::getGlobalData() const {
  return sceneGlobalData;
}

const Camera &RayTraceScene::getCamera() const { return sceneCamera; }

const std::vector<Primitive *> RayTraceScene::getPrimitives() const {
  return scenePrimitives;
}

const std::vector<SceneLightData> RayTraceScene::getLights() const {
  return lights;
}