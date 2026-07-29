#include <iostream>
#include "cggl.h"

int main(){
    cg::WindowSettings settings;
    settings.resizable = true;
    settings.antiAliasing = true;
    cg::Initialize("CGGL Window", cg::Vec2i(1920, 1080), settings);
    cg::SetFPSLimit(1000);
    cg::ToggleVSync(false);

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

        cg::PushPrimaryColor(cg::Color(100, 100, 100));
        roundedSize += cg::Input::GetMouseScroll().y;
        srand(seed);
        for (int i = 0; i < amountOfShapes; i++){
            if (!drawShapes) break;
            int r = rand() % 11;
            cg::Vec2f pos(rand() % 1920, rand() % 1080);
            int s = rand() % 50 + 25;
            int s2 = rand() % 50 + 25;
            int thickness = 5;
            cg::Vec2f size(s, s2);
            unsigned int flag = FLAG_NO_BORDER | FLAG_CENTERED;
            switch (r)
            {
            case 0:
                cg::Square(pos, s, flag);
                break;
            case 1:
                cg::RoundedSquare(pos, s, roundedSize, flag);
                break;
            case 2:
                cg::UnfilledSquare(pos, s, thickness, flag);
                break;
            case 3:
                cg::UnfilledRoundedSquare(pos, s, thickness, roundedSize, flag);
                break;
            case 4:
                cg::Rectangle(pos, size, flag);
                break;
            case 5:
                cg::RoundedRectangle(pos, size, roundedSize, flag);
                break;
            case 6:
                cg::UnfilledRectangle(pos, size, thickness, flag);
                break;
            case 7:
                cg::UnfilledRoundedRectangle(pos, size, thickness, roundedSize, flag);
                break;
            case 8:
                cg::Circle(pos, s, flag);
                break;
            case 9:
                cg::UnfilledCircle(pos, s, thickness, flag);
                break;
            case 10:
                cg::SemiCircle(pos, s, s2 % 4, flag);
                break;
            case 11:
                cg::UnfilledSemiCircle(pos, s, thickness, s2 % 4, flag);
                break;
            default:
                break;
            }
        }
        cg::PopStyle();

        cg::GUIStartSection("Control Panel", cg::Vec2f(20,20), cg::Vec2f(400, 1000));
        
        cg::PushFont(font);
        cg::GUIText("FPS: " + std::to_string((int)cg::GetAverageFPS()), FLAG_USE_SECONDARY_COLOR);
        cg::PopFont();
        cg::GUIText("Vertices: " + std::to_string(cg::GetVerticesAmount()), FLAG_USE_SECONDARY_COLOR);
        cg::GUIText("Indices: " + std::to_string(cg::GetIndicesAmount()), FLAG_USE_SECONDARY_COLOR);
        cg::GUIText("Textures: " + std::to_string(cg::GetTexturesAmount()), FLAG_USE_SECONDARY_COLOR);
        if (cg::GUIButton("Button"))
            std::cout << "Button 1 Clicked\n";
        cg::GUISameLine();
        if (cg::GUIButton("B"))
            std::cout << "Button 2 Clicked\n";
        if (cg::GUIButton("This is an example of a really long button"))
            std::cout << "Button 3 Clicked\n";

        cg::GUISlider("Amount Of Shapes", amountOfShapes, 0, 1000);
        cg::GUICheckBox("Draw Shapes", drawShapes);

        cg::GUIEndSection();

        cg::Draw();
        cg::NewFrame();
    }

    cg::Terminate();

    return 0;
}