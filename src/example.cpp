#include <iostream>

#define CGGL_INCLUDE_RAYTRACING
#include "cggl.h"

int main(){
    cg::WindowSettings settings;
    settings.resizable = true;
    settings.antiAliasing = true;
    cg::Initialize("CGGL Window", cg::Vec2i(1920, 1080), settings);
    cg::SetFPSLimit(1000);
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

    int raysPerPixel = 1, maxBounces = 3, passesPerFrame = 1, debugView = 0, debugNormalization = 100;
    float blurStrength = 0.5f, smoothness = 0.f;
    bool enableFrameAccumulation = false;

    cg::Scene scene;

    //scene.CreateSphere(cg::Vec3f(-5.f, -2.f, 10.f), 1.f, scene.CreateMaterial(cg::Color(0.f, 1.f, 0.f))); // Green Sphere
    //scene.CreateSphere(cg::Vec3f(-2.0f, -1.5f, 10.f), 1.5f, scene.CreateMaterial(cg::Color(1.f, 1.f, 0.f))); // Yellow Sphere
    //scene.CreateSphere(cg::Vec3f(1.5f, -1.5f, 10.f), 2.f, scene.CreateMaterial(cg::Color(1.f, 0.f, 0.f))); // Red Sphere
    //scene.CreateSphere(cg::Vec3f(7.f, -3.f, 10.f), 3.f, scene.CreateMaterial(cg::Color(1.f, 1.f, 1.f), cg::Color(0.f, 0.f, 0.f, 0.f), smoothness)); // White Sphere

    //scene.CreateSphere(cg::Vec3f(0.f, -101.0f, 0.f), 100.f, scene.CreateMaterial(cg::Color(0.f, 1.f, 1.f))); // Cyan, big Sphere

    scene.CreateSphere(cg::Vec3f(50, 30.f, 0.f), 12.5f, scene.CreateMaterial(cg::Color(), cg::Color(1.f, 1.f, 1.f, 15.f))); // Sun

    cg::Object dragon = scene.LoadPly("src/models/dragon.ply", .0f,
        cg::Transform(cg::Vec3f(6.5f, 1.5f, 0), cg::Vec3f(0.f, 0.5f, 0.f), cg::Vec3f(2.f)), 16);
    //scene.LoadPly("src/models/dragon.ply", 0.f, cg::Transform(), 16);

    cg::Raytracing::SetCameraLookAt(cg::Vec3f());
    cg::Raytracing::SetCameraPos(cg::Vec3f(30, 10, 0));

    cg::Raytracing::LoadScene(scene);

    while (cg::WindowIsOpen()){
        double deltaTime = cg::GetDeltatime();

        cg::Raytracing::UpdateFreecam();
        dragon.Rotate(cg::Vec3f(0.f, deltaTime, 0.f));
        //dragon.Move(cg::Vec3f(deltaTime, 0.f, 0.f));
        if (cg::IsKeyUp(KEY_SPACE)) {
            cg::Raytracing::SetRaysPerPixels(raysPerPixel);
            cg::Raytracing::SetMaxBounces(maxBounces);
            cg::Raytracing::SetBlurStrength(blurStrength);
            cg::Raytracing::SetPassesPerFrame(passesPerFrame);
            cg::Raytracing::ToggleFrameAccumulation(enableFrameAccumulation);
            cg::Raytracing::SetDebugView(debugView);
            cg::Raytracing::SetDebugNormalization(debugNormalization);

            cg::Raytracing::LoadScene(scene);
            cg::Raytracing::RayTrace(scene);
        } else {
            cg::Draw(scene, cg::Raytracing::GetCameraPos(), cg::Raytracing::GetCameraLookAt());
        }

        cg::SetBackgroundColor(cg::Color(0,0,0,0));
        cg::GUIStartSection("Control Panel", cg::Vec2f(1500,20), cg::Vec2f(400, 1000));
        cg::SetBackgroundColor(cg::Color(2, 62, 138));
        
        cg::PushFont(font);
        cg::GUIText("FPS: " + std::to_string((int)cg::GetAverageFPS()), FLAG_USE_SECONDARY_COLOR);
        cg::PopFont();

        cg::GUISlider("Rays Per Pixel", raysPerPixel, 1, 50);
        cg::GUISlider("Max Bounces", maxBounces, 0, 100);
        cg::GUISlider("Passes Per Frame", passesPerFrame, 1, 50);
        cg::GUISlider("Blur Strength", blurStrength, 0.f, 100.f);
        cg::GUISlider("Debug View", debugView, 0, 3);
        cg::GUISlider("Debug Normalization", debugNormalization, 0, 2000);
        cg::GUICheckBox("Enable Frame Accumulation", enableFrameAccumulation);
        if (cg::GUIButton("Reset Accumulation")) cg::Raytracing::QueueClear();
        if (cg::GUIButton("Export Image")) cg::Raytracing::ExportImage("image.png");
        cg::GUIText("Press SPACE to stop raytracing", FLAG_USE_SECONDARY_COLOR);
        cg::GUIText("Debug views:");
        cg::GUIText("1: Triangle Tests");
        cg::GUIText("2: Box Tests");
        cg::GUIText("3: Both Tests");

        cg::GUIEndSection();

        cg::Draw();
        cg::NewFrame();
    }

    cg::Raytracing::Quit();

    cg::Terminate();

    return 0;
}