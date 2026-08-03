#include "scene.h"

#include "include/happly.h"

#include <GLFW/glfw3.h>

#include "shape.h"

namespace cg {
    namespace {
        double bvhCreationTimePoint;
    }

    void GrowBoundingBox(cg::BVHNode& node, cg::Vec3f v){
        cg::Vec3f pos(node.pos[0], node.pos[1], node.pos[2]), size(node.size[0], node.size[1], node.size[2]);
        cg::Vec3f min(pos - size), max(pos + size);

        // First time the bounding box is growing.
        if (min == cg::Vec3f(0,0,0) && max == cg::Vec3f(0,0,0) && pos == cg::Vec3f(0,0,0)) {
            pos = v;
            size = cg::Vec3f(0.1,0.1,0.1); // Always have a little bit of size
        } else {
            if (v.x < min.x) min.x = v.x;
            if (v.y < min.y) min.y = v.y;
            if (v.z < min.z) min.z = v.z;

            if (v.x > max.x) max.x = v.x;
            if (v.y > max.y) max.y = v.y;
            if (v.z > max.z) max.z = v.z;

            size = (max - min) / 2.f;
            pos = (max + min) / 2.f;
        }

        node.size[0] = size.x;
        node.size[1] = size.y;
        node.size[2] = size.z;

        node.pos[0] = pos.x;
        node.pos[1] = pos.y;
        node.pos[2] = pos.z;
    }

    void GrowBoundingBox(cg::BVHNode& node, float* p){
        cg::Vec3f pos(p[0], p[1], p[2]);
        GrowBoundingBox(node, pos);
    }

    /* OBJECT */

    // Todo: optimize
    void Object::RotateAround(cg::Vec3f pivot, cg::Vec3f rotation) {
        Vec3f sr(sinf(rotation.x), sinf(rotation.y), sinf(rotation.z));
        Vec3f cr(cosf(rotation.x), cosf(rotation.y), cosf(rotation.z));
        for (TriangleObject* tri = trianglesStart; tri != trianglesEnd + 1; tri++) {
            cg::Vec3f p1(tri->p1[0], tri->p1[1], tri->p1[2]), p2(tri->p2[0], tri->p2[1], tri->p2[2]), p3(tri->p3[0], tri->p3[1], tri->p3[2]);
            // Subtract pivot
            p1.x -= pivot.x; p1.y -= pivot.y; p1.z -= pivot.z;
            p2.x -= pivot.x; p2.y -= pivot.y; p2.z -= pivot.z;
            p3.x -= pivot.x; p3.y -= pivot.y; p3.z -= pivot.z;

            // X rotation
            tri->p1[0] = p1.x;
            tri->p1[1] = p1.y * cr.x - p1.z * sr.x;
            tri->p1[2] = p1.y * sr.x + p1.z * cr.x;

            tri->p2[0] = p2.x;
            tri->p2[1] = p2.y * cr.x - p2.z * sr.x;
            tri->p2[2] = p2.y * sr.x + p2.z * cr.x;

            tri->p3[0] = p3.x;
            tri->p3[1] = p3.y * cr.x - p3.z * sr.x;
            tri->p3[2] = p3.y * sr.x + p3.z * cr.x;

            p1 = Vec3f(tri->p1[0], tri->p1[1], tri->p1[2]); p2 = cg::Vec3f(tri->p2[0], tri->p2[1], tri->p2[2]); p3 = cg::Vec3f(tri->p3[0], tri->p3[1], tri->p3[2]);

            // Y rotation
            tri->p1[0] = p1.x * cr.y + p1.z * sr.y;
            tri->p1[1] = p1.y;
            tri->p1[2] = -p1.x * sr.y + p1.z * cr.y;

            tri->p2[0] = p2.x * cr.y + p2.z * sr.y;
            tri->p2[1] = p2.y;
            tri->p2[2] = -p2.x * sr.y + p2.z * cr.y;

            tri->p3[0] = p3.x * cr.y + p3.z * sr.y;
            tri->p3[1] = p3.y;
            tri->p3[2] = -p3.x * sr.y + p3.z * cr.y;

            p1 = cg::Vec3f(tri->p1[0], tri->p1[1], tri->p1[2]); p2 = cg::Vec3f(tri->p2[0], tri->p2[1], tri->p2[2]); p3 = cg::Vec3f(tri->p3[0], tri->p3[1], tri->p3[2]);

            // Z rotation
            tri->p1[0] = p1.x * cr.z - p1.y * sr.z;
            tri->p1[1] = p1.x * sr.z + p1.y * cr.z;
            tri->p1[2] = p1.z;

            tri->p2[0] = p2.x * cr.z - p2.y * sr.z;
            tri->p2[1] = p2.x * sr.z + p2.y * cr.z;
            tri->p2[2] = p2.z;

            tri->p3[0] = p3.x * cr.z - p3.y * sr.z;
            tri->p3[1] = p3.x * sr.z + p3.y * cr.z;
            tri->p3[2] = p3.z;

            // Add pivot back
            tri->p1[0] += pivot.x; tri->p1[1] += pivot.y; tri->p1[2] += pivot.z;
            tri->p2[0] += pivot.x; tri->p2[1] += pivot.y; tri->p2[2] += pivot.z;
            tri->p3[0] += pivot.x; tri->p3[1] += pivot.y; tri->p3[2] += pivot.z;
        }

        const std::vector<TriangleObject>& triangles = scene->GetTriangles();
        for (BVHNode* node = bvhStart; node < bvhEnd + 1; node++) {
            BVHNode newNode{};
            newNode.trianglesStart = node->trianglesStart;
            newNode.trianglesEnd = node->trianglesEnd;
            newNode.childIndex = node->childIndex;

            for (int i = newNode.trianglesStart; i <= newNode.trianglesEnd; i++) {
                TriangleObject triangle = triangles[i];
                cg::GrowBoundingBox(newNode, cg::Vec3f(triangle.p1[0], triangle.p1[1], triangle.p1[2]));
                cg::GrowBoundingBox(newNode, cg::Vec3f(triangle.p2[0], triangle.p2[1], triangle.p2[2]));
                cg::GrowBoundingBox(newNode, cg::Vec3f(triangle.p3[0], triangle.p3[1], triangle.p3[2]));
            }

            *node = newNode;
        }
    }

    void Object::Rotate(cg::Vec3f rotation) {
        Object::RotateAround(transform.position, rotation);
        transform.rotation = transform.rotation + rotation;
    }

    void Object::Move(cg::Vec3f movement) {
        for (TriangleObject* tri = trianglesStart; tri < trianglesEnd; tri++) {
            tri->p1[0] += movement.x; tri->p2[0] += movement.x; tri->p3[0] += movement.x;
            tri->p1[1] += movement.y; tri->p2[1] += movement.y; tri->p3[1] += movement.y;
            tri->p1[2] += movement.z; tri->p2[2] += movement.z; tri->p3[2] += movement.z;
        }

        for (BVHNode* node = bvhStart; node < bvhEnd; node++) {
            node->pos[0] += movement.x;
            node->pos[1] += movement.y;
            node->pos[2] += movement.z;
        }

        transform.position = transform.position + movement;
    }

    /* SCENE */

    int Scene::CreateMaterial(cg::Color color, cg::Color emissionColor, float smoothness){
        cg::Material mat;

        mat.color[0] = color.r;
        mat.color[1] = color.g;
        mat.color[2] = color.b;

        mat.emissionColor[0] = emissionColor.r;
        mat.emissionColor[1] = emissionColor.g;
        mat.emissionColor[2] = emissionColor.b;
        mat.emissionColor[3] = emissionColor.a;

        mat.smoothness = smoothness;

        materials.push_back(mat);

        return materials.size() - 1;
    }

    void Scene::CreateSphere(cg::Vec3f center, float radius, int materialIndex){
        cg::Sphere s{};
        s.center[0] = center.x;
        s.center[1] = center.y;
        s.center[2] = center.z;

        s.radius = radius;

        s.materialIndex = materialIndex;

        spheres.push_back(s);
    }

    void Scene::CreateTriangle(cg::Vec3f p1, cg::Vec3f p2, cg::Vec3f p3, int materialIndex){
        cg::TriangleObject t{};
        t.p1[0] = p1.x;
        t.p1[1] = p1.y;
        t.p1[2] = p1.z;

        t.p2[0] = p2.x;
        t.p2[1] = p2.y;
        t.p2[2] = p2.z;

        t.p3[0] = p3.x;
        t.p3[1] = p3.y;
        t.p3[2] = p3.z;

        t.materialIndex = materialIndex;

        triangles.push_back(t);
    }

    cg::Object Scene::LoadPly(std::string path, float smoothness, cg::Transform transform, int bvhDepthLimit){
        happly::PLYData plyIn(path);

        std::vector<std::array<double, 3>> meshVertexPositions = plyIn.getVertexPositions();
        std::vector<std::array<unsigned char, 3>> meshVertexColors = plyIn.getVertexColors();
        std::vector<std::vector<size_t>> meshFaceIndices = plyIn.getFaceIndices();

        // Combine vertices positions and colors
        std::vector<cg::Vertex> vertices(meshVertexPositions.size());
        for (int i = 0; i < meshVertexPositions.size(); i++){
            cg::Vertex& vertex = vertices[i];

            vertex.pos.x = -meshVertexPositions[i][0];
            vertex.pos.y = meshVertexPositions[i][1];
            vertex.pos.z = meshVertexPositions[i][2];

            vertex.color.r = meshVertexColors[i][0];
            vertex.color.g = meshVertexColors[i][1];
            vertex.color.b = meshVertexColors[i][2];
        }

        // Convert face indices to vertex indices
        std::vector<unsigned int> indices;
        for (const auto& face : meshFaceIndices) {
            if (face.size() < 3)
                continue;

            for (size_t i = 1; i + 1 < face.size(); ++i) {
                indices.push_back(face[0]);
                indices.push_back(face[i]);
                indices.push_back(face[i + 1]);
            }
        }

        return Scene::LoadMesh(vertices, indices, smoothness, transform, bvhDepthLimit);
    }

    cg::Object Scene::LoadMesh(std::vector<cg::Vertex> vertices, std::vector<unsigned int> indices, float smoothness, cg::Transform transform, int bvhDepthLimit){
        Mesh mesh{};
        mesh.triangleIndexStart = triangles.size();

        for (cg::Vertex& v : vertices){
            v.pos = v.pos * transform.size;
            v.pos = v.pos + transform.position;
        }

        std::cout << "Loading " << indices.size() / 3 << " triangles\n";
        for (int i = 0; i < indices.size() / 3; i++){
            unsigned int v0 = indices[i * 3 + 0];
            unsigned int v1 = indices[i * 3 + 1];
            unsigned int v2 = indices[i * 3 + 2];

            // Take the average color the three vertices
            cg::Color col;
            col = col + vertices[v0].color / 255.f;
            col = col + vertices[v1].color / 255.f;
            col = col + vertices[v2].color / 255.f;
            col = col / 3;

            Scene::CreateTriangle(vertices[v0].pos, vertices[v1].pos, vertices[v2].pos, Scene::CreateMaterial(col, cg::Color(), smoothness));
        }
        std::cout << "Loaded " << triangles.size() - mesh.triangleIndexStart << " Triangles" << std::endl;

        BVHNode rootNode{};
        
        for (cg::Vertex& v : vertices){
            cg::GrowBoundingBox(rootNode, v.pos);
        }

        mesh.triangleIndexEnd = triangles.size();

        rootNode.childIndex = 1;
        rootNode.trianglesStart = mesh.triangleIndexStart;
        rootNode.trianglesEnd = mesh.triangleIndexEnd;

        bvhCreationTimePoint = glfwGetTime();
        mesh.boundingBoxIndex = Scene::ConvertTrianglesToBVH(rootNode, bvhDepthLimit);
        meshes.push_back(mesh);

        cg::Object object(transform, triangles[mesh.triangleIndexStart], triangles[mesh.triangleIndexEnd - 1], bvhNodes[mesh.boundingBoxIndex], bvhNodes.back(), *this);
        object.Rotate(transform.rotation);
        return object;
    }

    /* BVH SYSTEM */

    cg::Vec3f GetTriangleCenter(cg::TriangleObject triangle){
        return (cg::Vec3f(triangle.p1[0], triangle.p1[1], triangle.p1[2]) + cg::Vec3f(triangle.p2[0], triangle.p2[1], triangle.p2[2]) + cg::Vec3f(triangle.p3[0], triangle.p3[1], triangle.p3[2])) / 3.f;
    }

    void Scene::Split(BVHNode& parent, int maxDepth){
        if (maxDepth <= 0) return;

        BVHNode childA{}, childB{};

        childA.trianglesStart = parent.trianglesStart;
        childB.trianglesStart = parent.trianglesStart;

        childA.trianglesEnd = parent.trianglesStart;
        childB.trianglesEnd = parent.trianglesStart;

        int longestAxis = parent.size[0] > parent.size[1] ? 0 : 1;
        if (parent.size[2] > parent.size[longestAxis]) longestAxis = 2;

        for (int i = parent.trianglesStart; i < parent.trianglesEnd; i++){
            bool triangleIsInFirstHalf = cg::GetTriangleCenter(triangles[i])[longestAxis] < parent.pos[longestAxis];
            BVHNode& node = triangleIsInFirstHalf ? childA : childB;
            node.trianglesEnd++;

            cg::GrowBoundingBox(node, triangles[i].p1);
            cg::GrowBoundingBox(node, triangles[i].p2);
            cg::GrowBoundingBox(node, triangles[i].p3);

            // Sort triangle into it's corresponding half
            if (triangleIsInFirstHalf){
                std::swap(triangles[i], triangles[node.trianglesEnd - 1]);

                // Increase triangle index of child b when adding triangles into child a
                childB.trianglesStart++;
                childB.trianglesEnd++;
            }
        }

        int childIndex = bvhNodes.size();
        parent.childIndex = childIndex;

        bvhNodes.push_back(childA);
        bvhNodes.push_back(childB);

        const int minTriangleAmount = 5;
        int childAIndex = childIndex + 0;
        int childBIndex = childIndex + 1;

        if (childA.trianglesEnd - childA.trianglesStart > minTriangleAmount) Scene::Split(bvhNodes[childAIndex], maxDepth - 1);
        if (childB.trianglesEnd - childB.trianglesStart > minTriangleAmount) Scene::Split(bvhNodes[childBIndex], maxDepth - 1);
    }

    int Scene::ConvertTrianglesToBVH(BVHNode& rootNode, int depthLimit){
        int i = bvhNodes.size();
        bvhNodes.push_back(rootNode);

        depthLimit = std::clamp(depthLimit, 0, 64);

        Scene::Split(bvhNodes[i], depthLimit);

        std::cout << "---- BVH INFORMATION ----\n";
        std::cout << "BVH Creation took " << glfwGetTime() - bvhCreationTimePoint << " seconds\n";
        std::cout << "BVH Nodes: " << bvhNodes.size() - i << std::endl; 
        int leafTriangleSum = 0;
        cg::Vec2i leafTriangleCounts(INT_MAX, INT_MIN);
        int leafNodesCount = 0;
        for (int j = i; j < bvhNodes.size(); j++){
            BVHNode& node = bvhNodes[j];
            int trianglesCount = node.trianglesEnd - node.trianglesStart;
            if (node.childIndex == 0) {
                leafTriangleSum += trianglesCount;
                leafNodesCount++;
                leafTriangleCounts.x = std::min(leafTriangleCounts.x, trianglesCount);
                leafTriangleCounts.y = std::max(leafTriangleCounts.y, trianglesCount);
            }
        }
        std::cout << "Leaf nodes: " << leafNodesCount << std::endl;
        std::cout << "Max Depth: " << depthLimit << std::endl;
        std::cout << "Minimum of " << leafTriangleCounts.x << " triangles" << std::endl;
        std::cout << "Maximum of " << leafTriangleCounts.y << " triangles" << std::endl;
        std::cout << "Average of " << leafTriangleSum / leafNodesCount << " triangles " << std::endl;
        std::cout << "-------------------------\n";

        return i;
    }
}
