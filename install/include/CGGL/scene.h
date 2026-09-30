#ifndef SCENE_H
#define SCENE_H

#include "types.h"

namespace cg {
    /* 3d SHAPES */

    struct Material {
        float color[3];
        float _offset1;

        float emissionColor[4];

        float smoothness = 0.f;
        float _offset2[3];
    };

    struct Sphere {
        float center[3];
        float radius;
        int materialIndex;
        int _offset[3];
    };

    struct TriangleObject {
        float p1[3];
        float _offset1;
        float p2[3];
        float _offset2;
        float p3[3];
        int materialIndex;
    };

    struct Box {
        float pos[3];
        float _offset1;
        float size[3];
        int materialIndex;
    };

    struct Mesh {
        int triangleIndexStart;
        int triangleIndexEnd;
        int boundingBoxIndex;
    };

    struct BVHNode {
        float min[3];
        int childIndex;

        float max[3];
        int trianglesStart;

        int trianglesEnd;
        int _offset[3];
    };

    void GrowBoundingBox(cg::BVHNode& node, cg::Vec3f v);

    class Transform {
        public:
            Transform() : position(cg::Vec3f()), rotation(cg::Vec3f()), size(cg::Vec3f(1,1,1)) {}
            Transform(cg::Vec3f position, cg::Vec3f rotation = cg::Vec3f(), cg::Vec3f size = cg::Vec3f(1,1,1)) : 
                position(position), rotation(rotation), size(size) {}

            cg::Vec3f position, rotation, size;
    };

    class Scene;
    class Object {
        public:
            Object() {}
            Object(cg::Transform transform, cg::TriangleObject& trianglesStart, cg::TriangleObject& trianglesEnd, BVHNode& bvhStart, BVHNode& bvhEnd, Scene& scene) {
                this->trianglesStart = &trianglesStart;
                this->trianglesEnd = &trianglesEnd;

                this->bvhStart = &bvhStart;
                this->bvhEnd = &bvhEnd;
                this->scene = &scene;

                this->transform = transform;
            }

            void RotateAround(cg::Vec3f pivot, cg::Vec3f rotation) const;
            void Rotate(cg::Vec3f rotation);

            void Move(cg::Vec3f movement);

            cg::Transform transform;

            cg::TriangleObject* trianglesStart = nullptr;
            cg::TriangleObject* trianglesEnd = nullptr;

            BVHNode* bvhStart = nullptr;
            BVHNode* bvhEnd = nullptr;

            Scene* scene = nullptr;
    };

    class Scene {
        public:
            Scene() {}

            int CreateMaterial(cg::Color color, cg::Color emissionColor = cg::Color(0.f, 0.f, 0.f, 0.f), float smoothness = 0.f);

            void CreateSphere(cg::Vec3f center, float radius, int materialIndex);
            void CreateTriangle(cg::Vec3f p1, cg::Vec3f p2, cg::Vec3f p3, int materialIndex);

            void ConvertSpheresToTriangles();

            std::vector<Material>& GetMaterials() { return materials; }
            int GetAmountOfMaterials() const { return materials.size(); }

            std::vector<Sphere>& GetSpheres() { return spheres; }
            int GetAmountOfSpheres() const { return spheres.size(); }

            std::vector<TriangleObject>& GetTriangles() { return triangles; }
            int GetAmountOfTriangles() const { return triangles.size(); }

            std::vector<Box>& GetBoxes() { return boxes; }
            int GetAmountOfBoxes() const { return boxes.size(); }

            std::vector<Mesh>& GetMeshes() { return meshes; }
            int GetAmountOfMeshes() const { return meshes.size(); }

            std::vector<BVHNode>& GetBVHNodes() { return bvhNodes; }
            int GetAmountOfBVHNodes() const { return bvhNodes.size(); }

            // Load a .ply 3d mesh.
            cg::Object LoadPly(std::string path, float smoothness = 0.f, cg::Transform transform = cg::Transform(), int bvhDepthLimit = 24);

            cg::Object LoadMesh(std::vector<cg::Vertex> vertices, std::vector<unsigned int> indices, float smoothness = 0.f, cg::Transform transform = cg::Transform(), int bvhDepthLimit = 24);

            // You should fiddle around a bit with the depthLimit to get the best performance
            int ConvertTrianglesToBVH(cg::BVHNode& rootNode, int depthLimit = 24);
        private:
            std::vector<Material> materials;
            std::vector<Sphere> spheres;
            std::vector<TriangleObject> triangles;
            std::vector<Box> boxes;
            std::vector<Mesh> meshes;
            std::vector<BVHNode> bvhNodes;

            void Split(BVHNode& parent, int maxDepth);
    };
}

#endif