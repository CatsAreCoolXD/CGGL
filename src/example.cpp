#include <iostream>
#include "cggl.h"

int main(){
    cg::WindowSettings settings;
    settings.resizable = true;
    settings.antiAliasing = true;
    cg::Initialize("CGGL Window", cg::Vec2i(1920, 1080), settings);
    cg::SetFPSLimit(60);

    cg::SetBackgroundColor(cg::Color(70, 83, 98));

    //cg::SetRenderingMode(RENDERING_MODE_WIREFRAME);

    cg::Texture tex("src/tex/container.jpg");
    tex.SetSize(cg::Vec2f(200, 200));

    cg::Texture tex2("src/tex/dice.png");
    tex2.SetSize(cg::Vec2f(400, 400));

    cg::Vec2f center(1920 / 2, 1080 / 2);
    tex.SetOrigin(center - cg::Vec2f(600.f, -200.f), true);
    tex2.SetOrigin(center, true);

    float roundedSize = 15;

    bool wireframeMode = true;

    while (cg::WindowIsOpen()){
        if (cg::Input::IsKeyDown(KEY_UP)) roundedSize += 1.f;
        if (cg::Input::IsKeyDown(KEY_DOWN)) roundedSize -= 1.f;
        if (cg::Input::GetKeyDown(KEY_SPACE)) {
            wireframeMode = !wireframeMode;
            cg::SetRenderingMode(wireframeMode ? RENDERING_MODE_WIREFRAME : RENDERING_MODE_FILL);
        }

        if (cg::Input::GetMouseButtonDown(MOUSE_BUTTON_LEFT)){
            center.x -= 10.f;
        }
        if (cg::Input::GetMouseButtonDown(MOUSE_BUTTON_RIGHT)){
            center.x += 10.f;
        }

        roundedSize += cg::Input::GetMouseScroll().y;

        cg::Circle(cg::Input::GetMousePos(), 25, FLAG_CENTERED);

        cg::Circle(center - cg::Vec2f(600.f, 200.f), 100, FLAG_CENTERED);

        cg::UseTexture(tex);
        cg::UnfilledCircle(center - cg::Vec2f(600.f, -200.f), 100, 50, FLAG_CENTERED);
        cg::StopUsingTexture();

        cg::Rectangle(center + cg::Vec2f(0.f, 300.f), cg::Vec2f(250, 100), FLAG_CENTERED);
        cg::RoundedRectangle(center + cg::Vec2f(0.f, 100.f), cg::Vec2f(250, 100), roundedSize, FLAG_CENTERED);

        cg::UnfilledRectangle(center + cg::Vec2f(0.f, -300.f), cg::Vec2f(250, 100), 5, FLAG_CENTERED);
        cg::UnfilledRoundedRectangle(center + cg::Vec2f(0.f, -100.f), cg::Vec2f(250, 100), 5, roundedSize, FLAG_CENTERED);

        cg::Draw(tex2);
        
        cg::Square(center + cg::Vec2f(600.f, 300.f), 150, FLAG_CENTERED);
        cg::RoundedSquare(center + cg::Vec2f(600.f, 100.f), 150, roundedSize, FLAG_CENTERED);

        cg::UnfilledSquare(center + cg::Vec2f(600.f, -300.f), 150, 5, FLAG_CENTERED);
        cg::UnfilledRoundedSquare(center + cg::Vec2f(600.f, -100.f), 150, 5, roundedSize, FLAG_CENTERED);

        cg::Draw();
        cg::NewFrame();
    }

    cg::Terminate();

    return 0;
}