#ifndef CGGL_RAYTRACING_H
#define CGGL_RAYTRACING_H

#include "../types.h"
#include "../draw.h"
#include "../input.h"
#include "../shape.h"
#include "../draw.h"
#include "../window.h"

#include <vector>

#include <glad/glad.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

// Todo: Make Ray Tracing work with window resizing

namespace cg {
    GLFWwindow* GetWindow();
    namespace Raytracing {
        namespace {
            cg::Shader raytracingShader;

            cg::Texture noiseTex, tex;
            GLuint accumTex[2];
            GLuint frameBuffers[2];

            GLuint VAO, VBO, EBO;

            int raysPerPixel = 5, maxBounces = 5;
            float blurStrength = 0.f;
            int frame = 0;

            cg::Vec3d cameraPos(-15,5,0), cameraLookAt(0,0,0);
            double flySpeed = 1.0;

            bool enableFreeCam = true, clearQueued = false, framebufferResizeQueued = false;

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
            glDeleteFramebuffers(2, frameBuffers);
            glDeleteTextures(2, accumTex);

            glGenTextures(2, accumTex);

            cg::Vec2i windowSize = cg::GetWindowSize();

            for (int i = 0; i < 2; i++){
                glBindTexture(GL_TEXTURE_2D, accumTex[i]);

                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RGB,
                    windowSize.x,
                    windowSize.y,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    NULL
                );

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glBindTexture(GL_TEXTURE_2D, 0);
            }

            glGenFramebuffers(2, frameBuffers);

            for (int i = 0; i < 2; i++)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]);

                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_2D,
                    accumTex[i],
                    0
                );

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
                    throw std::runtime_error("Error when creating frame buffers!");
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }

        void InitRaytracing(){
            raytracingShader.Create("src/raytracing/vert.glsl", "src/raytracing/frag.glsl");
            noiseTex.LoadImage("src/raytracing/noise.png", REPEAT);

            tex.SetOrigin(cg::Vec2f(0,0));
            tex.SetSize(cg::GetWindowSize());
            tex.SetTint(cg::Color(255,255,255));

            tex.offset.y = -1.f;
            tex.flip.y = 0.f;
        
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            cg::Raytracing::CreateFrameBuffers();

            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[0]);
            glClear(GL_COLOR_BUFFER_BIT);

            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[1]);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwSetWindowSizeCallback(cg::GetWindow(), window_resize_callback);
        }

        // Call this before calling RayTrace()
        void ClearFrameBuffers(){
            int read = frame % 2;
            int write = !read;

            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[read]);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        void ResetFrames(){
            frame = 0;
        }

        void SetRaysPerPixels(int value) {
            if (value == raysPerPixel) return;
            raysPerPixel = value;
            cg::Raytracing::ClearFrameBuffers();
            frame = 0;
        }

        void SetMaxBounces(int value) {
            if (value == maxBounces) return;
            maxBounces = value;
            cg::Raytracing::ClearFrameBuffers();
            frame = 0;
        }

        void SetBlurStrength(float value){
            if (value == blurStrength) return;
            blurStrength = value;
            cg::Raytracing::ClearFrameBuffers();
            frame = 0;
        }

        void SetCameraPos(cg::Vec3d pos){
            if (pos == cameraPos) return;
            cameraPos = pos;
            cg::Raytracing::ClearFrameBuffers();
            frame = 0;
        }

        void SetCameraLookAt(cg::Vec3d lookAt){
            if (lookAt == cameraLookAt) return;
            cameraLookAt = lookAt;
            cg::Raytracing::ClearFrameBuffers();
            frame = 0;
        }

        void UpdateFreecam()
        {
            if (cg::GetMouseButtonUp(MOUSE_BUTTON_RIGHT)) frame = 0;
            if (cg::IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                enableFreeCam = true;
                cg::Raytracing::QueueClear();
            } else enableFreeCam = false;

            constexpr double sens = 1.f;
            flySpeed = std::max(0.0, flySpeed + cg::GetMouseScroll().y);

            static cg::Vec2d previousMousePos;
            static cg::Vec2d mouseAccum;

            cg::Vec2d mousePos = cg::GetMousePos() / cg::Vec2d(cg::GetWindowSize());

            cg::Vec2d mouseT = (mousePos - previousMousePos) * 2.0 * 3.14159 * sens;
            double deltaTime = cg::GetDeltatime();

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

        void LoadScene(cg::Scene& scene){
            raytracingShader.Use();

            srand(time(NULL));
            raytracingShader.SetArray(scene.GetAmountOfMaterials() * sizeof(Material), scene.GetMaterials().data(), 0u);
            raytracingShader.SetArray(scene.GetAmountOfSpheres() * sizeof(Sphere), scene.GetSpheres().data(), 1u);
            raytracingShader.SetArray(scene.GetAmountOfTriangles() * sizeof(TriangleObject), scene.GetTriangles().data(), 2u);
            raytracingShader.SetArray(scene.GetAmountOfBoxes() * sizeof(Box), scene.GetBoxes().data(), 3u);
            raytracingShader.SetArray(scene.GetAmountOfMeshes() * sizeof(Mesh), scene.GetMeshes().data(), 4u);
            raytracingShader.SetArray(scene.GetAmountOfBVHNodes() * sizeof(BVHNode), scene.GetBVHNodes().data(), 5u);

            raytracingShader.SetInt("amountOfMaterials", scene.GetAmountOfMaterials());
            raytracingShader.SetInt("amountOfSpheres", scene.GetAmountOfSpheres());
            raytracingShader.SetInt("amountOfTriangles", scene.GetAmountOfTriangles());
            raytracingShader.SetInt("amountOfBoxes", scene.GetAmountOfBoxes());
            raytracingShader.SetInt("amountOfMeshes", scene.GetAmountOfMeshes());
            raytracingShader.SetInt("amountOfBVHNodes", scene.GetAmountOfBVHNodes());

            raytracingShader.SetInt("raysPerPixel", raysPerPixel);
            raytracingShader.SetInt("maxBounces", maxBounces);
            raytracingShader.SetFloat("blurStrength", blurStrength);

            cg::Vec2i windowSize = cg::GetWindowSize();
            raytracingShader.SetInt("frame", 0);
            raytracingShader.SetFloats("resolution", cg::Vec2f(cg::GetWindowSize()));
            raytracingShader.SetFloats("cameraPos", cg::Vec3f(cameraPos));
            raytracingShader.SetFloats("cameraLookAt", cg::Vec3f(cameraLookAt));
            raytracingShader.SetBool("enableFrameAccumulation", !enableFreeCam);
        }

        void RayTrace(cg::Scene& scene){
            if (clearQueued){
                clearQueued = false;

                cg::Raytracing::ClearFrameBuffers();
                cg::Raytracing::ResetFrames();
            }

            if (framebufferResizeQueued) {
                framebufferResizeQueued = false;

                cg::Raytracing::InitRaytracing();
                cg::Raytracing::ClearFrameBuffers();
                cg::Raytracing::ResetFrames();
            }

            raytracingShader.Use();
            raytracingShader.SetInt("amountOfMaterials", scene.GetAmountOfMaterials());
            raytracingShader.SetInt("amountOfSpheres", scene.GetAmountOfSpheres());
            raytracingShader.SetInt("amountOfTriangles", scene.GetAmountOfTriangles());
            raytracingShader.SetInt("amountOfBoxes", scene.GetAmountOfBoxes());
            raytracingShader.SetInt("amountOfMeshes", scene.GetAmountOfMeshes());
            raytracingShader.SetInt("amountOfBVHNodes", scene.GetAmountOfBVHNodes());
            
            raytracingShader.SetInt("frame", frame);
            raytracingShader.SetInt("raysPerPixel", raysPerPixel);
            raytracingShader.SetInt("maxBounces", maxBounces);
            raytracingShader.SetFloat("blurStrength", blurStrength);
            raytracingShader.SetFloats("resolution", cg::Vec2f(cg::GetWindowSize()));
            raytracingShader.SetFloats("cameraPos", cg::Vec3f(cameraPos));
            raytracingShader.SetFloats("cameraLookAt", cg::Vec3f(cameraLookAt));
            raytracingShader.SetBool("enableFrameAccumulation", !enableFreeCam);

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
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[write]);
            glClearColor(0.1f, 0.1f, 0.1f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

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
            glBindTexture(GL_TEXTURE_2D, accumTex[read]);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, noiseTex.textureId);

            // Draw Triangles
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Unbind frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            tex.textureId = accumTex[write];
            cg::PushNewTriangleBuffer();
            cg::Draw(tex, FLAG_NO_BORDER); // Draw a quad with the texture

            // Unbind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

            frame++;
        }

        void ExportImage(std::string name){
            cg::Vec2i imageSize = cg::GetWindowSize();
            std::vector<unsigned char> pixels(imageSize.x * imageSize.y * 4);

            int write = frame % 2;
            int read = !read;

            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[read]);
            glReadPixels(0, 0, imageSize.x, imageSize.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

            stbi_write_png(name.c_str(), imageSize.x, imageSize.y, 4, pixels.data(), imageSize.x * 4);
        }
    }
}

#endif