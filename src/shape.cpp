#include "shape.h"
#include "window.h"
#include "style.h"
#include "types.h"
#include "draw.h"

#include <iostream>
#include <cassert>

#include "include/happly.h"

namespace cg {
    /* 2d shapes */

    void Quad(cg::Vec2f pos, int size, unsigned int flags){
        cg::Vec2f origin = flags & FLAG_CENTERED ? (pos - cg::Vec2f(size / 2.f, size / 2.f)) : pos;
        if (flags & FLAG_CENTERED) flags &= ~FLAG_CENTERED;

        cg::Rectangle(origin, cg::Vec2f(size, size), flags | FLAG_NO_BORDER);
    }

    void Square(cg::Vec2f pos, int size, unsigned int flags){
        cg::Rectangle(pos, cg::Vec2f(size, size), flags);
    }

    void UnfilledSquare(cg::Vec2f pos, int size, int thickness, unsigned int flags){
        cg::UnfilledRectangle(pos, cg::Vec2f(size, size), thickness, flags);
    }

    void RoundedSquare(cg::Vec2f pos, int size, int roundedSize, unsigned int flags){
        cg::RoundedRectangle(pos, cg::Vec2f(size, size), roundedSize, flags);
    }

    void UnfilledRoundedSquare(cg::Vec2f pos, int size, int thickness, int roundedSize, unsigned int flags){
        cg::UnfilledRoundedRectangle(pos, cg::Vec2f(size, size), thickness, roundedSize, flags);
    }

    void Rectangle(cg::Vec2f pos, cg::Vec2f size, unsigned int flags){
        const cg::Style style = cg::GetCurrentStyle();

        const cg::Color color = flags & FLAG_USE_SECONDARY_COLOR ? style.secondaryColor : style.primaryColor;

        if (flags & FLAG_CENTERED) pos = pos - cg::Vec2f(size.x / 2.f, size.y / 2.f);
        flags &= ~FLAG_CENTERED;

        cg::Vertex bottomLeft(pos, color);
        cg::Vertex topLeft(pos + cg::Vec2f(0, size.y), color);
        cg::Vertex topRight(pos + size, color);
        cg::Vertex bottomRight(pos + cg::Vec2f(size.x, 0), color);

        cg::Vertex firstTriangle[3] = {
            bottomLeft, topLeft, topRight
        };
        cg::Triangle(firstTriangle);

        cg::Vertex secondTriangle[3] = {
            bottomLeft, bottomRight, topRight
        };
        cg::Triangle(secondTriangle);

        if (style.border && !(flags & FLAG_NO_BORDER)){
            cg::UnfilledRectangle(pos, size, style.borderThickness, flags | FLAG_NO_BORDER | FLAG_USE_SECONDARY_COLOR);
        }
    }

    void RoundedRectangle(cg::Vec2f pos, cg::Vec2f size, int roundedSize, unsigned int flags){
        const cg::Style style = cg::GetCurrentStyle();

        const cg::Color color = flags & FLAG_USE_SECONDARY_COLOR ? style.secondaryColor : style.primaryColor;

        cg::Vec2f origin(pos);
        if (flags & FLAG_CENTERED) origin = pos - cg::Vec2f(size.x / 2.f, size.y / 2.f);
        flags &= ~FLAG_CENTERED;

        // Rounded edges
        cg::Vertex bottomLeft(origin + cg::Vec2f(roundedSize, 0.f), color);
        cg::Vertex topLeft(origin + cg::Vec2f(roundedSize, size.y), color);
        cg::Vertex topRight(origin + size - cg::Vec2f(roundedSize, 0.f), color);
        cg::Vertex bottomRight(origin - cg::Vec2f(roundedSize - size.x, 0.f), color);

        cg::Rectangle(origin + cg::Vec2f(0.f, roundedSize), cg::Vec2f(roundedSize, size.y - roundedSize * 2.f), flags | FLAG_NO_BORDER);
        cg::Rectangle(origin + cg::Vec2f(size.x - roundedSize, 0.f) + cg::Vec2f(0.f, roundedSize), cg::Vec2f(roundedSize, size.y - roundedSize * 2.f), flags | FLAG_NO_BORDER);

        cg::SemiCircle(origin + cg::Vec2f(size.x - roundedSize, size.y - roundedSize), roundedSize, TOP_RIGHT, flags | FLAG_NO_BORDER);
        cg::SemiCircle(origin + cg::Vec2f(roundedSize, size.y - roundedSize), roundedSize, TOP_LEFT, flags | FLAG_NO_BORDER);
        cg::SemiCircle(origin + cg::Vec2f(size.x - roundedSize, roundedSize), roundedSize, BOTTOM_RIGHT, flags | FLAG_NO_BORDER);
        cg::SemiCircle(origin + cg::Vec2f(roundedSize, roundedSize), roundedSize, BOTTOM_LEFT, flags | FLAG_NO_BORDER);

        cg::Vertex firstTriangle[3] = {
            bottomLeft, topLeft, topRight
        };
        cg::Triangle(firstTriangle);

        cg::Vertex secondTriangle[3] = {
            bottomLeft, bottomRight, topRight
        };
        cg::Triangle(secondTriangle);

        if (style.border && !(flags & FLAG_NO_BORDER)){
            cg::UnfilledRoundedRectangle(origin, size, style.borderThickness, roundedSize, flags | FLAG_USE_SECONDARY_COLOR);
        }
    }
    
    void UnfilledRectangle(cg::Vec2f pos, cg::Vec2f size, int thickness, unsigned int flags){
        if (flags & FLAG_CENTERED) pos = pos - cg::Vec2f(size.x / 2.f, size.y / 2.f);
        flags &= ~FLAG_CENTERED;

        cg::Vec2f bottomLeft(pos);
        cg::Vec2f topLeft(pos + cg::Vec2f(thickness, size.y));
        cg::Vec2f topRight(pos + size - cg::Vec2f(0.f, thickness));
        cg::Vec2f bottomRight(pos + cg::Vec2f(size.x - thickness, thickness));

        cg::Rectangle(bottomLeft, bottomRight - bottomLeft, flags | FLAG_NO_BORDER);
        cg::Rectangle(topLeft, topRight - topLeft, flags | FLAG_NO_BORDER);

        bottomRight = bottomRight - cg::Vec2f(0.f, thickness);

        cg::Rectangle(bottomLeft, topLeft - bottomLeft, flags | FLAG_NO_BORDER);
        cg::Rectangle(bottomRight, topRight - bottomRight, flags | FLAG_NO_BORDER);
    }

    void UnfilledRoundedRectangle(cg::Vec2f pos, cg::Vec2f size, int thickness, int roundedSize, unsigned int flags){
        const cg::Style style = cg::GetCurrentStyle();
        const cg::Color color = flags & FLAG_USE_SECONDARY_COLOR ? style.secondaryColor : style.primaryColor;

        cg::Vec2f origin(pos);
        if (flags & FLAG_CENTERED) origin = pos - cg::Vec2f(size.x / 2.f, size.y / 2.f);
        flags &= ~FLAG_CENTERED;

        cg::Vec2f bottomLeft(origin + cg::Vec2f(roundedSize, 0.f));
        cg::Vec2f topLeft(origin + cg::Vec2f(roundedSize, size.y));
        cg::Vec2f topRight(origin + cg::Vec2f(size.x - roundedSize, size.y - thickness));
        cg::Vec2f bottomRight(origin + cg::Vec2f(size.x - roundedSize, thickness));

        cg::Rectangle(bottomLeft, bottomRight - bottomLeft, flags | FLAG_NO_BORDER);
        cg::Rectangle(topLeft, topRight - topLeft, flags | FLAG_NO_BORDER);

        bottomLeft = cg::Vec2f(origin + cg::Vec2f(0.f, roundedSize));
        topLeft = cg::Vec2f(origin + cg::Vec2f(thickness, size.y - roundedSize));
        bottomRight = cg::Vec2f(origin + cg::Vec2f(size.x, roundedSize));
        topRight = cg::Vec2f(origin + cg::Vec2f(size.x - thickness, size.y - roundedSize));

        cg::Rectangle(bottomLeft, topLeft - bottomLeft, flags | FLAG_NO_BORDER);
        cg::Rectangle(bottomRight, topRight - bottomRight, flags | FLAG_NO_BORDER);

        cg::UnfilledSemiCircle(origin + cg::Vec2f(roundedSize, roundedSize), roundedSize, BOTTOM_LEFT, thickness, flags);
        cg::UnfilledSemiCircle(origin + cg::Vec2f(roundedSize, size.y - roundedSize), roundedSize, TOP_LEFT, thickness, flags);
        cg::UnfilledSemiCircle(origin + cg::Vec2f(size.x - roundedSize, roundedSize), roundedSize, BOTTOM_RIGHT, thickness, flags);
        cg::UnfilledSemiCircle(origin + cg::Vec2f(size.x - roundedSize, size.y - roundedSize), roundedSize, TOP_RIGHT, thickness, flags);
    }

    void Circle(cg::Vec2f pos, int radius, unsigned int flags){
        if (radius == 0) return;
        cg::Vec2f center = flags & FLAG_CENTERED ? pos : (pos + cg::Vec2f(radius / 2.f, radius / 2.f));

        cg::SemiCircle(center, radius, TOP_RIGHT, flags);
        cg::SemiCircle(center, radius, BOTTOM_RIGHT, flags);
        cg::SemiCircle(center, radius, BOTTOM_LEFT, flags);
        cg::SemiCircle(center, radius, TOP_LEFT, flags);
    }

    void UnfilledCircle(cg::Vec2f pos, int radius, int thickness, unsigned int flags){
        if (radius == 0) return;
        cg::Vec2f center = flags & FLAG_CENTERED ? pos : (pos + cg::Vec2f(radius / 2.f, radius / 2.f));

        cg::UnfilledSemiCircle(center, radius, thickness, TOP_RIGHT, flags);
        cg::UnfilledSemiCircle(center, radius, thickness, BOTTOM_RIGHT, flags);
        cg::UnfilledSemiCircle(center, radius, thickness, BOTTOM_LEFT, flags);
        cg::UnfilledSemiCircle(center, radius, thickness, TOP_LEFT, flags);
    }

    void SemiCircle(cg::Vec2f center, int radius, int direction, unsigned int flags){
        if (radius == 0) return;
        const cg::Style style = cg::GetCurrentStyle();
        const cg::Color color = flags & FLAG_USE_SECONDARY_COLOR ? style.secondaryColor : style.primaryColor;

        float step = PI / (float)style.quality / 2.f;
        float offset = step * (float)style.quality * (float)direction;

        cg::Vertex centerVertex(center, color);
        for (int i = 0; i < style.quality; i++){
            cg::Vertex pos1(center + cg::Vec2f(sin(step * i + offset) * radius, cos(step * i + offset) * radius), color);
            cg::Vertex pos2(center + cg::Vec2f(sin(step * (i+1) + offset) * radius, cos(step * (i+1) + offset) * radius), color);

            cg::Vertex triangle[3] = {
                centerVertex, pos1, pos2
            };

            cg::Triangle(triangle);
        }

        if (style.border && !(flags & FLAG_NO_BORDER))
            cg::UnfilledSemiCircle(center, radius, direction, style.borderThickness, flags | FLAG_USE_SECONDARY_COLOR);
    }

    void UnfilledSemiCircle(cg::Vec2f center, int radius, int direction, int thickness, unsigned int flags){
        if (radius == 0) return;
        const cg::Style style = cg::GetCurrentStyle();
        const cg::Color color = flags & FLAG_USE_SECONDARY_COLOR ? style.secondaryColor : style.primaryColor;

        float step = PI / (float)style.quality / 2.f;
        float offset = step * (float)style.quality * (float)direction;

        cg::Vertex centerVertex(center, color);
        for (int i = 0; i < style.quality; i++){
            cg::Vertex outer1(center + cg::Vec2f(sin(step * i + offset) * radius, cos(step * i + offset) * radius), color);
            cg::Vertex outer2(center + cg::Vec2f(sin(step * (i+1) + offset) * radius, cos(step * (i+1) + offset) * radius), color);
            cg::Vec2f dirToCenter1 = (cg::Vec2f(outer1.pos) - center);
            cg::Vec2f dirToCenter2 = (cg::Vec2f(outer2.pos) - center);

            cg::Vertex inner1(cg::Vec2f(outer1.pos) - dirToCenter1 / radius * thickness, color);
            cg::Vertex inner2(cg::Vec2f(outer2.pos) - dirToCenter2 / radius * thickness, color);

            cg::Vertex triangle1[3] = {
                outer1, outer2, inner1
            };
            cg::Triangle(triangle1);

            cg::Vertex triangle2[3] = {
                inner1, inner2, outer2
            };
            cg::Triangle(triangle2);
        }
    }

    void Triangle(cg::Vertex* vertices){
        // Convert pixel positions to normalized device coordinates (NDC) and convert color values from 0-255 to 0-1
        int width, height;
        glfwGetWindowSize(cg::GetWindow(), &width, &height);

        if (width == 0 || height == 0) return;

        cg::Vertex vertices3D[3];
        for (int i = 0; i < 3; i++){
            vertices3D[i].pos.x = ((float)vertices[i].pos.x / (float)width - 0.5f) * 2.f;
            vertices3D[i].pos.y = ((float)vertices[i].pos.y / (float)height - 0.5f) * 2.f;
            vertices3D[i].pos.z = 0.f; // 2D triangle so set the z value to zero

            vertices3D[i].color.r = (float)vertices[i].color.r / 255.f;
            vertices3D[i].color.g = (float)vertices[i].color.g / 255.f;
            vertices3D[i].color.b = (float)vertices[i].color.b / 255.f;
            vertices3D[i].color.a = (float)vertices[i].color.a / 255.f;
        }
        
        cg::Triangle3D(vertices3D);
    }

    /* 3d shapes */

    void Triangle3D(cg::Vertex* vertices){
        // Convert vectors to an array
        if (!cg::IsUsingTexture()){
            float triangle[21];
            for (int i = 0; i < 3; i++){
                triangle[i * 7 + 0] = cg::clamp11(vertices[i].pos.x);
                triangle[i * 7 + 1] = cg::clamp11(vertices[i].pos.y);
                triangle[i * 7 + 2] = cg::clamp11(vertices[i].pos.z);

                triangle[i * 7 + 3] = cg::clamp01(vertices[i].color.r);
                triangle[i * 7 + 4] = cg::clamp01(vertices[i].color.g);
                triangle[i * 7 + 5] = cg::clamp01(vertices[i].color.b);
                triangle[i * 7 + 6] = cg::clamp01(vertices[i].color.a);
            }
            cg::PushTriangle(triangle);
        } else {
            cg::Vec2f texOrigin = cg::GetTextureOrigin();
            cg::Vec2f texSize = cg::GetTextureSize();
            cg::Color tint = cg::GetTextureTint();
            cg::Vec2f flip = cg::GetTextureFlip();
            cg::Vec2f offset = cg::GetTextureOffset();
            bool renderingGlyph = cg::IsUsingTexture() && cg::GetCurrentTexture()->IsGlyph();
            float triangle[27];
            for (int i = 0; i < 3; i++){
                triangle[i * 9 + 0] = cg::clamp11(vertices[i].pos.x);
                triangle[i * 9 + 1] = cg::clamp11(vertices[i].pos.y);
                triangle[i * 9 + 2] = cg::clamp11(vertices[i].pos.z);

                triangle[i * 9 + 3] = cg::clamp01(tint.r / 255.f);
                triangle[i * 9 + 4] = cg::clamp01(tint.g / 255.f);
                triangle[i * 9 + 5] = cg::clamp01(tint.b / 255.f);
                triangle[i * 9 + 6] = cg::clamp01(tint.a / 255.f);

                triangle[i * 9 + 7] = (triangle[i * 9 + 0] - texOrigin.x) / texSize.x + offset.x;
                triangle[i * 9 + 8] = (triangle[i * 9 + 1] - texOrigin.y) / texSize.y + offset.y;
                if (flip.y == 1.f) triangle[i * 9 + 8] *= -1.f;
                if (renderingGlyph && triangle[i * 9 + 7] < 0.01f) triangle[i * 9 + 7] = 0.0f;
                if (renderingGlyph && triangle[i * 9 + 8] < 0.01f) triangle[i * 9 + 8] = 0.0f;
            }
            cg::PushTriangle(triangle);
        }
    }

    int Scene::CreateMaterial(cg::Color color, cg::Color emissionColor, float smoothness){
        Material mat;

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
        Sphere s;
        s.center[0] = center.x;
        s.center[1] = center.y;
        s.center[2] = center.z;

        s.radius = radius;

        s.materialIndex = materialIndex;

        spheres.push_back(s);
    }

    void Scene::CreateTriangle(cg::Vec3f p1, cg::Vec3f p2, cg::Vec3f p3, int materialIndex){
        TriangleObject t;
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

    void Scene::LoadPly(std::string path){
        happly::PLYData plyIn(path, true);

        std::vector<std::array<double, 3>> meshVertexPositions = plyIn.getVertexPositions();
        std::vector<std::array<unsigned char, 3>> meshVertexColors = plyIn.getVertexColors();
        std::vector<std::vector<size_t>> meshFaceIndices = plyIn.getFaceIndices();

        // Combine vertices positions and colors
        std::vector<cg::Vertex> vertices(meshVertexPositions.size());
        for (int i = 0; i < meshVertexPositions.size(); i++){
            cg::Vertex& vertex = vertices[i];

            vertex.pos.x = meshVertexPositions[i][0];
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

        Scene::LoadMesh(vertices, indices);
    }

    void Scene::LoadMesh(std::vector<cg::Vertex> vertices, std::vector<unsigned int> indices){
        Mesh mesh;
        mesh.triangleIndexStart = triangles.size();

        std::cout << "Loading " << indices.size() / 3 << " triangles\n";
        for (int i = 0; i < indices.size() / 3; i++){
            int v0 = indices[i * 3 + 0];
            int v1 = indices[i * 3 + 1];
            int v2 = indices[i * 3 + 2];

            // Take the average color the three vertices
            cg::Color col;
            col = col + vertices[v0].color / 255.f;
            col = col + vertices[v1].color / 255.f;
            col = col + vertices[v2].color / 255.f;
            col = col / 3;

            Scene::CreateTriangle(vertices[v0].pos, vertices[v1].pos, vertices[v2].pos, Scene::CreateMaterial(col));
        }
        std::cout << "Loaded Triangles" << std::endl;

        Box boundingBox;
        
        cg::Vec3f min = vertices[0].pos;
        cg::Vec3f max = vertices[0].pos;
        for (cg::Vertex& v : vertices){
            if (v.pos.x < min.x) min.x = v.pos.x;
            if (v.pos.y < min.y) min.y = v.pos.y;
            if (v.pos.z < min.z) min.z = v.pos.z;
            
            if (v.pos.x > max.x) max.x = v.pos.x;
            if (v.pos.y > max.y) max.y = v.pos.y;
            if (v.pos.z > max.z) max.z = v.pos.z;
        }

        cg::Vec3f boxSize = cg::Vec3f(max - min) / 2.f;
        boundingBox.size[0] = boxSize.x;
        boundingBox.size[1] = boxSize.y;
        boundingBox.size[2] = boxSize.z;

        cg::Vec3f boxPos = cg::Vec3f(min + max) / 2.f;
        boundingBox.pos[0] = boxPos.x;
        boundingBox.pos[1] = boxPos.y;
        boundingBox.pos[2] = boxPos.z;

        boxes.push_back(boundingBox);

        mesh.boundingBoxIndex = boxes.size() - 1;
        mesh.triangleIndexEnd = triangles.size();

        meshes.push_back(mesh);
    }
}