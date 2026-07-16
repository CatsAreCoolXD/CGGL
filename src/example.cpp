#include <iostream>
#include "cggl.h"

int main(){
    cg::WindowSettings settings;
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

    while (cg::WindowIsOpen()){
        cg::Circle(center - cg::Vec2f(600.f, 200.f), 100, CENTERED);

        cg::UseTexture(tex);
        cg::UnfilledCircle(center - cg::Vec2f(600.f, -200.f), 100, 50, CENTERED);
        cg::StopUsingTexture();

        cg::Rectangle(center + cg::Vec2f(0.f, 300.f), cg::Vec2f(250, 100), CENTERED);
        cg::RoundedRectangle(center + cg::Vec2f(0.f, 100.f), cg::Vec2f(250, 100), 15, CENTERED);

        cg::UnfilledRectangle(center + cg::Vec2f(0.f, -300.f), cg::Vec2f(250, 100), 5, CENTERED);
        cg::UnfilledRoundedRectangle(center + cg::Vec2f(0.f, -100.f), cg::Vec2f(250, 100), 5, 15, CENTERED);

        cg::Draw(tex2);
        
        cg::Square(center + cg::Vec2f(600.f, 300.f), 150, CENTERED);
        cg::RoundedSquare(center + cg::Vec2f(600.f, 100.f), 150, 15, CENTERED);

        cg::UnfilledSquare(center + cg::Vec2f(600.f, -300.f), 150, 5, CENTERED);
        cg::UnfilledRoundedSquare(center + cg::Vec2f(600.f, -100.f), 150, 5, 7, CENTERED);

        cg::Draw();
        cg::NewFrame();
    }

    cg::Terminate();

    return 0;
}