A project completed for Brown University's CS1230

## Design
***
Following the basic parsing and variable setting, `raytracescene` first converts the content parsed by `sceneparser` in one meaningful way, changing the `RenderShapeData` objects into of type `Primitive`. `Primitive` is an abstract class with subclasses `Cube`, `Sphere`, `Cylinder`, and `Cone`. These primitive each contain their CTM, material, and implemented virtual functions...

1 : `float intersect(Ray ray)` : `intersect` will return the `t` for a `Ray` if it intersects with the primitive. If it does not intersect, it will return -1. This is the most important function for the raytracer, as it is used to determine the whether a pixel ought have light applied to it.

2 : `vec3 getNormal(glm::vec3 point)` : `getNormal` will return the normal vector of the primitive at the given point. This is used to determine how the light is cast on the surface of the primitive.

Note also the custom `Ray` class, which essentially serves as a wrapper for the `origin` and `direction` pair.

For each of use and readability, the `Primitive` classes assume that the `Ray` that is passed in is in object space.  This is done by multiplying the `Ray`'s `origin` and `direction` by the inverse of the `CTM` of the primitive (as well as the inverse view matrix). This is done in `raytracer` prior to calling `intersect` and `getNormal`.
