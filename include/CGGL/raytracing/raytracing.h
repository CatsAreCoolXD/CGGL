#ifndef CGGL_RAYTRACING_H
#define CGGL_RAYTRACING_H

#include "CGGL/types.h"
#include "CGGL/draw.h"
#include "CGGL/input.h"
#include "CGGL/shape.h"
#include "CGGL/draw.h"
#include "CGGL/scene.h"

#include <vector>

#include <glad/glad.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "CGGL/stb_image_write.h"
#include "GLFW/glfw3.h"

namespace cg {
    GLFWwindow* GetWindow();
    namespace Raytracing {
        namespace {
            cg::Shader raytracingShader;

            Framebuffer frameBuffers[2];

            GLuint VAO, VBO, EBO;

            int raysPerPixel = 5, maxBounces = 5, passesPerFrame = 1, debugView = 0, debugNormalization;
            float blurStrength = 0.f, waterLevel = 0.f;
            int frame = 0;
            float time = 1.f;

            cg::Vec3d cameraPos(-15,5,0), cameraLookAt(0,0,0);
            double flySpeed = 1.0;

            bool enableFreeCam = true, clearQueued = false, framebufferResizeQueued = false, enableFrameAccumulation = false, enableWater = false;

            void window_resize_callback(GLFWwindow* window, int width, int height){
                framebufferResizeQueued = true;
            }
        }

        cg::Vec3f GetCameraPos() {
            return cameraPos;
        }

        cg::Vec3f GetCameraLookAt() {
            return cameraLookAt;
        }

        void QueueClear(){
            clearQueued = true;
        }

        void QueueFramebufferResize(){
            framebufferResizeQueued = true;
        }

        void CreateFrameBuffers(){
            // Delete old textures
            frameBuffers[0].Delete();
            frameBuffers[1].Delete();

            Vec2i windowSize = cg::GetWindowSize();

            frameBuffers[0].Create(windowSize);
            frameBuffers[1].Create(windowSize);
        }

        void InitRaytracing(){
            raytracingShader.Create((std::string(CGGL_SHADER_DIR) + std::string("/raytracer-vert.glsl")).c_str(),
                (std::string(CGGL_SHADER_DIR) + std::string("/raytracer-frag.glsl")).c_str());
        
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            cg::Raytracing::CreateFrameBuffers();

            frameBuffers[0].Clear();
            frameBuffers[1].Clear();

            glfwSetWindowSizeCallback(cg::GetWindow(), window_resize_callback);
        }

        // Call this before calling RayTrace()
        void ClearFramebuffers(){
            int read = frame % 2;
            int write = !read;

            frameBuffers[read].Clear();
        }

        void ResetFrames(){
            frame = 0;
        }

        void SetRaysPerPixels(int value) {
            if (value == raysPerPixel) return;
            raysPerPixel = value;
            cg::Raytracing::ClearFramebuffers();
            frame = 0;
        }

        void SetMaxBounces(int value) {
            if (value == maxBounces) return;
            maxBounces = value;
            cg::Raytracing::ClearFramebuffers();
            frame = 0;
        }

        void SetPassesPerFrame(int value) {
            if (value == passesPerFrame) return;
            passesPerFrame = std::max(value, 1);
            ClearFramebuffers();
            frame = 0;
        }

        void SetBlurStrength(float value){
            if (value == blurStrength) return;
            blurStrength = value;
            cg::Raytracing::ClearFramebuffers();
            frame = 0;
        }

        void SetCameraPos(cg::Vec3d pos){
            if (pos == cameraPos) return;
            cameraPos = pos;
            cg::Raytracing::ClearFramebuffers();
            frame = 0;
        }

        void SetCameraLookAt(cg::Vec3d lookAt){
            if (lookAt == cameraLookAt) return;
            cameraLookAt = lookAt;
            cg::Raytracing::ClearFramebuffers();
            frame = 0;
        }

        void SetDebugView(int value) {
            if (value == debugView) return;
            debugView = std::clamp(value, 0, 3);
            cg::Raytracing::ClearFramebuffers();
            frame = 0;
        }

        void SetDebugNormalization(int value) {
            if (value == debugNormalization) return;
            debugNormalization = std::max(0, value);
            cg::Raytracing::ClearFramebuffers();
            frame = 0;
        }

        void ToggleFrameAccumulation(bool value) {
            enableFrameAccumulation = value;
        }
        
        void ToggleWater(bool value) {
            if (enableWater == value) return;
            enableWater = value;
            ClearFramebuffers();
            frame = 0;
        }

        void SetWaterLevel(float value) {
            if (waterLevel == value) return;
            waterLevel = value;
            ClearFramebuffers();
            frame = 0;
        }

        void UpdateFreecam()
        {
            if (cg::GetMouseButtonUp(MOUSE_BUTTON_RIGHT)) frame = 0;
            if (cg::IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                enableFreeCam = true;
                if (enableFrameAccumulation) cg::Raytracing::QueueClear();
            } else enableFreeCam = false;

            constexpr double sens = 1.f;
            flySpeed = std::max(0.0, flySpeed + cg::GetMouseScroll().y);

            static cg::Vec2d previousMousePos;
            static cg::Vec2d mouseAccum;

            cg::Vec2d mousePos = cg::GetMousePos() / cg::Vec2d(cg::GetWindowSize());

            cg::Vec2d mouseT = (mousePos - previousMousePos) * 2.0 * 3.14159 * sens;
            double deltaTime = cg::GetDeltaTime();

            previousMousePos = mousePos;

            if (enableFreeCam){
                mouseAccum = mouseAccum + mouseT;

                cg::Vec3d forward = cg::Vec3d(sin(mouseAccum.x), sin(mouseAccum.y), cos(mouseAccum.x));
                cg::Vec3d right = forward.Cross(cg::Vec3d(0, -1, 0));

                if (cg::IsKeyDown(KEY_W))
                    cameraPos = cameraPos + forward * flySpeed * deltaTime;
                if (cg::IsKeyDown(KEY_S))
                    cameraPos = cameraPos - forward * flySpeed * deltaTime;

                if (cg::IsKeyDown(KEY_D))
                    cameraPos = cameraPos + right * flySpeed * deltaTime;
                if (cg::IsKeyDown(KEY_A))
                    cameraPos = cameraPos - right * flySpeed * deltaTime;

                cameraLookAt = cameraPos + forward;
            }
        }

        /* RENDERING */

        void LoadScene(cg::Scene& scene){
            raytracingShader.Use();

            raytracingShader.SetBuffer(scene.GetAmountOfMaterials() * sizeof(Material), scene.GetMaterials().data(), 0u);
            raytracingShader.SetBuffer(scene.GetAmountOfSpheres() * sizeof(Sphere), scene.GetSpheres().data(), 1u);
            raytracingShader.SetBuffer(scene.GetAmountOfTriangles() * sizeof(TriangleObject), scene.GetTriangles().data(), 2u);
            raytracingShader.SetBuffer(scene.GetAmountOfBoxes() * sizeof(Box), scene.GetBoxes().data(), 3u);
            raytracingShader.SetBuffer(scene.GetAmountOfMeshes() * sizeof(Mesh), scene.GetMeshes().data(), 4u);
            raytracingShader.SetBuffer(scene.GetAmountOfBVHNodes() * sizeof(BVHNode), scene.GetBVHNodes().data(), 5u);

            raytracingShader.SetInt("amountOfMaterials", scene.GetAmountOfMaterials());
            raytracingShader.SetInt("amountOfSpheres", scene.GetAmountOfSpheres());
            raytracingShader.SetInt("amountOfTriangles", scene.GetAmountOfTriangles());
            raytracingShader.SetInt("amountOfBoxes", scene.GetAmountOfBoxes());
            raytracingShader.SetInt("amountOfMeshes", scene.GetAmountOfMeshes());
            raytracingShader.SetInt("amountOfBVHNodes", scene.GetAmountOfBVHNodes());

            raytracingShader.SetInt("raysPerPixel", raysPerPixel);
            raytracingShader.SetInt("maxBounces", maxBounces);
            raytracingShader.SetFloat("blurStrength", blurStrength);
            raytracingShader.SetInt("debugView", debugView);
            raytracingShader.SetInt("debugNormalization", debugNormalization);

            cg::Vec2i windowSize = cg::GetWindowSize();
            raytracingShader.SetInt("frame", 0);
            raytracingShader.SetInt("randomValue", rand());
            raytracingShader.SetFloats("resolution", cg::Vec2f(cg::GetWindowSize()));
            raytracingShader.SetFloats("cameraPos", cg::Vec3f(cameraPos));
            raytracingShader.SetFloats("cameraLookAt", cg::Vec3f(cameraLookAt));
            raytracingShader.SetBool("enableFrameAccumulation", enableFrameAccumulation);
            raytracingShader.SetBool("enableWater", enableWater);
            raytracingShader.SetFloat("waterLevel", waterLevel);
            raytracingShader.SetFloat("deltaTime", GetDeltaTime());
            raytracingShader.SetFloat("time", glfwGetTime());

            clearQueued = true;
        }

        void RayTrace(cg::Scene& scene){
            if (!enableFrameAccumulation) time += GetDeltaTime();
            if (clearQueued || !enableFrameAccumulation){
                clearQueued = false;

                cg::Raytracing::ClearFramebuffers();
                cg::Raytracing::ResetFrames();
            }

            if (framebufferResizeQueued) {
                framebufferResizeQueued = false;

                cg::Raytracing::InitRaytracing();
                cg::Raytracing::ClearFramebuffers();
                cg::Raytracing::ResetFrames();
            }

            for (int pass = 0; pass < passesPerFrame; pass++) {
                raytracingShader.Use();
                raytracingShader.SetInt("amountOfMaterials", scene.GetAmountOfMaterials());
                raytracingShader.SetInt("amountOfSpheres", scene.GetAmountOfSpheres());
                raytracingShader.SetInt("amountOfTriangles", scene.GetAmountOfTriangles());
                raytracingShader.SetInt("amountOfBoxes", scene.GetAmountOfBoxes());
                raytracingShader.SetInt("amountOfMeshes", scene.GetAmountOfMeshes());
                raytracingShader.SetInt("amountOfBVHNodes", scene.GetAmountOfBVHNodes());

                raytracingShader.SetInt("frame", frame);
                raytracingShader.SetInt("randomValue", rand() % 100);
                raytracingShader.SetInt("raysPerPixel", raysPerPixel);
                raytracingShader.SetInt("maxBounces", maxBounces);
                raytracingShader.SetInt("debugView", debugView);
                raytracingShader.SetInt("debugNormalization", debugNormalization);
                raytracingShader.SetFloat("blurStrength", blurStrength);
                raytracingShader.SetFloats("resolution", cg::Vec2f(cg::GetWindowSize()));
                raytracingShader.SetFloats("cameraPos", cg::Vec3f(cameraPos));
                raytracingShader.SetFloats("cameraLookAt", cg::Vec3f(cameraLookAt));
                raytracingShader.SetBool("enableFrameAccumulation", enableFrameAccumulation || pass > 0);
                raytracingShader.SetBool("enableWater", enableWater);
                raytracingShader.SetFloat("waterLevel", waterLevel);
                raytracingShader.SetFloat("deltaTime", GetDeltaTime());
                raytracingShader.SetFloat("time", time);

                constexpr float vertices[12] = {
                    -1.f, -1.f, 0.f,
                    -1.f, 1.f, 0.f,
                    1.f, 1.f, 0.f,
                    1.f, -1.f, 0.f
                };

                constexpr unsigned int indices[6] = {
                    0, 1, 2,
                    0, 3, 2
                };

                int read = frame % 2;
                int write = !read;

                // Write into the frame buffer
                frameBuffers[write].Clear();
                frameBuffers[write].Enable();

                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

                // Tell OpenGL how it should interpret vertex data
                // Positions
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                // Bind Textures
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, frameBuffers[read].GetTexture()->textureId);

                // Draw Triangles
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                // Unbind frame buffer
                frameBuffers[write].Disable();

                cg::PushNewTriangleBuffer();
                cg::Draw(*frameBuffers[write].GetTexture(), FLAG_NO_BORDER); // Draw a quad with the texture

                // Unbind textures
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);

                frame++;
            }
        }

        void ExportImage(std::string name){
            cg::Vec2i imageSize = cg::GetWindowSize();
            std::vector<unsigned char> pixels(imageSize.x * imageSize.y * 4);

            int write = frame % 2;
            int read = !write;

            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[read].framebufferId);
            glReadPixels(0, 0, imageSize.x, imageSize.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

            stbi_write_png(name.c_str(), imageSize.x, imageSize.y, 4, pixels.data(), imageSize.x * 4);
        }

        void Quit() {
            raytracingShader.DeleteShader();
            frameBuffers[0].Delete();
            frameBuffers[1].Delete();
        }
    }
}

#endif