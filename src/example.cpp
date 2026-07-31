#include <iostream>

#define CGGL_INCLUDE_RAYTRACING
#include "cggl.h"

int main(){
    cg::WindowSettings settings;
    settings.resizable = true;
    settings.antiAliasing = true;
    cg::Initialize("CGGL Window", cg::Vec2i(1920, 1080), settings);
    cg::SetFPSLimit(60);
    cg::ToggleVSync(false);

    cg::Raytracing::InitRaytracing();
    cg::DefineRayTracing();

    cg::SetBackgroundColor(cg::Color(2, 62, 138));

    cg::Texture tex("src/tex/container.jpg");
    tex.SetSize(cg::Vec2f(200, 200));

    cg::Texture tex2("src/tex/dice.png");
    tex2.SetSize(cg::Vec2f(400, 400));

    cg::Vec2f center(1920 / 2, 1080 / 2);
    tex.SetOrigin(center - cg::Vec2f(600.f, -200.f), true);
    tex2.SetOrigin(center, true);

    cg::Font font("src/fonts/OpenSans-Bold.ttf", 48);
    cg::Font smallerFont("src/fonts/OpenSans-Bold.ttf", 16);
    cg::PushFont(smallerFont);

    float roundedSize = 15;

    int amountOfShapes = 0;
    bool drawShapes = true;

    bool wireframeMode = true;

    time_t seed = time(NULL);

    cg::PushQuality(4);

    int raysPerPixel = 5, maxBounces = 5;
    float blurStrength = 0.5f, smoothness = 0.f;

    cg::Scene scene;

    scene.CreateSphere(cg::Vec3f(-5.f, -2.f, 10.f), 1.f, scene.CreateMaterial(cg::Color(0.f, 1.f, 0.f))); // Green Sphere
    scene.CreateSphere(cg::Vec3f(-2.0f, -1.5f, 10.f), 1.5f, scene.CreateMaterial(cg::Color(1.f, 1.f, 0.f))); // Yellow Sphere
    scene.CreateSphere(cg::Vec3f(1.5f, -1.5f, 10.f), 2.f, scene.CreateMaterial(cg::Color(1.f, 0.f, 0.f))); // Red Sphere
    scene.CreateSphere(cg::Vec3f(7.f, -3.f, 10.f), 3.f, scene.CreateMaterial(cg::Color(1.f, 1.f, 1.f), cg::Color(0.f, 0.f, 0.f, 0.f), smoothness)); // White Sphere

    scene.CreateSphere(cg::Vec3f(0.f, -101.0f, 0.f), 100.f, scene.CreateMaterial(cg::Color(0.f, 1.f, 1.f))); // Cyan, big Sphere

    scene.CreateSphere(cg::Vec3f(-10.f, 10.f, -25.f), 10.f, scene.CreateMaterial(cg::Color(), cg::Color(1.f, 1.f, 1.f, 5.f))); // Sun

    scene.LoadPly("src/models/monkey.ply");

    while (cg::WindowIsOpen()){
        if (cg::IsKeyUp(KEY_SPACE)) {
            cg::Raytracing::SetRaysPerPixels(raysPerPixel);
            cg::Raytracing::SetMaxBounces(maxBounces);
            cg::Raytracing::SetBlurStrength(blurStrength);

            cg::Raytracing::UpdateFreecam();

            cg::Raytracing::RayTrace(scene);
        }

        cg::SetBackgroundColor(cg::Color(0,0,0,0));
        cg::GUIStartSection("Control Panel", cg::Vec2f(1500,20), cg::Vec2f(400, 1000));
        cg::SetBackgroundColor(cg::Color(2, 62, 138));
        
        cg::PushFont(font);
        cg::GUIText("FPS: " + std::to_string((int)cg::GetAverageFPS()), FLAG_USE_SECONDARY_COLOR);
        cg::PopFont();

        cg::GUISlider("Rays Per Pixel", raysPerPixel, 0, 50);
        cg::GUISlider("Max Bounces", maxBounces, 0, 100);
        cg::GUISlider("Blur Strength", blurStrength, 0.f, 100.f);
        if (cg::GUIButton("Reset Accumulation")) cg::Raytracing::QueueClear();
        cg::GUIText("Press SPACE to stop raytracing", FLAG_USE_SECONDARY_COLOR);

        cg::GUIEndSection();

        cg::Draw();
        cg::NewFrame();
    }

    cg::Terminate();

    return 0;
}