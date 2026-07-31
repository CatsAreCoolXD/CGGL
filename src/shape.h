#ifndef SHAPE_H
#define SHAPE_H

#include "types.h"

#define FLAG_CENTERED 0b1000u
#define FLAG_NO_BORDER 0b0100u
#define FLAG_USE_SECONDARY_COLOR 0b0010u

#define TOP_RIGHT 0
#define BOTTOM_RIGHT 1
#define BOTTOM_LEFT 2
#define TOP_LEFT 3

namespace cg {
    /* 2d SHAPES */

    // Square but never has a border
    void Quad(cg::Vec2f pos, int size, unsigned int flags = 0);
    void Square(cg::Vec2f pos, int size, unsigned int flags = 0);
    void UnfilledSquare(cg::Vec2f pos, int size, int thickness, unsigned int flags = 0);
    void RoundedSquare(cg::Vec2f pos, int size, int roundedSize, unsigned int flags = 0);
    void UnfilledRoundedSquare(cg::Vec2f pos, int size, int thickness, int roundedSize, unsigned int flags = 0);

    void Rectangle(cg::Vec2f pos, cg::Vec2f size, unsigned int flags = 0);
    void UnfilledRectangle(cg::Vec2f pos, cg::Vec2f size, int thickness, unsigned int flags = 0);

    void RoundedRectangle(cg::Vec2f pos, cg::Vec2f size, int roundedSize, unsigned int flags = 0);
    void UnfilledRoundedRectangle(cg::Vec2f pos, cg::Vec2f size, int thickness, int roundedSize, unsigned int flags = 0);

    void Circle(cg::Vec2f pos, int radius, unsigned int flags = 0);
    void UnfilledCircle(cg::Vec2f pos, int radius, int thickness, unsigned int flags = 0);

    void SemiCircle(cg::Vec2f center, int radius, int direction, unsigned int flags = 0);
    void UnfilledSemiCircle(cg::Vec2f center, int radius, int direction, int thickness, unsigned int flags = 0);

    void Triangle(cg::Vertex* vertices);

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

    void Triangle3D(cg::Vertex* vertices);

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

            // Load a .ply 3d mesh.
            void LoadPly(std::string path);

            void LoadMesh(std::vector<cg::Vertex> vertices, std::vector<unsigned int> indices);
        private:
            std::vector<Material> materials;
            std::vector<Sphere> spheres;
            std::vector<TriangleObject> triangles;
            std::vector<Box> boxes;
            std::vector<Mesh> meshes;
    };
}

#endif