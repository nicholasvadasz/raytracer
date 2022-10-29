#include "sceneparser.h"
#include "glm/gtx/transform.hpp"
#include "scenefilereader.h"

#include <chrono>
#include <iostream>
#include <memory>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
  ScenefileReader fileReader = ScenefileReader(filepath);
  bool success = fileReader.readXML();
  if (!success) {
    return false;
  }
  renderData.shapes.clear();
  auto root = fileReader.getRootNode();
  parseScene(root, glm::mat4(1.f), renderData.shapes);
  renderData.globalData = fileReader.getGlobalData();
  renderData.cameraData = fileReader.getCameraData();
  renderData.lights = fileReader.getLights();
  return true;
}

void SceneParser::parseScene(SceneNode *node, glm::mat4 ctm,
                             std::vector<RenderShapeData> &shapes) {
  for (auto &transformation : node->transformations) {
    switch (transformation->type) {
    case TransformationType::TRANSFORMATION_ROTATE:
      ctm *= glm::rotate(transformation->angle, transformation->rotate);
      break;
    case TransformationType::TRANSFORMATION_SCALE:
      ctm *= glm::scale(transformation->scale);
      break;
    case TransformationType::TRANSFORMATION_TRANSLATE:
      ctm *= glm::translate(transformation->translate);
      break;
    case TransformationType::TRANSFORMATION_MATRIX:
      ctm *= transformation->matrix;
      break;
    }
  }
  for (auto &primitive : node->primitives) {
    RenderShapeData shape;
    shape.primitive = *primitive;
    shape.ctm = ctm;
    shapes.push_back(shape);
  }
  for (auto &child : node->children) {
    parseScene(child, ctm, shapes);
  }
}
