#include "input.h"

namespace cg {
    namespace Input {
        namespace {
            int keyStates[GLFW_KEY_LAST];
            int buttonStates[GLFW_MOUSE_BUTTON_LAST];

            cg::Vec2d scrollDeltaNew(0, 0), scrollDeltaOld(0, 0);
        };

        int* GetKeyStates() {
            return keyStates;
        }

        int* GetMouseButtonStates(){
            return buttonStates;
        }

        bool IsKeyDown(int key) {
            return glfwGetKey(cg::GetWindow(), key) == GLFW_PRESS;
        }

        bool IsKeyUp(int key) { return !cg::Input::IsKeyDown(key); }

        bool GetKeyDown(int key) { return keyStates[key] == INPUTSTATE_PRESSED; }

        bool GetKeyUp(int key) { return keyStates[key] == INPUTSTATE_RELEASED; }

        int GetKeyState(int key) { return keyStates[key]; }

        void UpdateInputStates() {
            GLFWwindow* window = cg::GetWindow();
            for (int i = 0; i < GLFW_KEY_LAST; i++) {
                int keyState = glfwGetKey(window, i);
                if (keyStates[i] == INPUTSTATE_PRESSED && keyState == GLFW_RELEASE) keyStates[i] = INPUTSTATE_RELEASED;
                else if (keyStates[i] == INPUTSTATE_PRESSED) keyStates[i] = INPUTSTATE_DOWN;
                else if (keyStates[i] == INPUTSTATE_RELEASED && keyState == GLFW_PRESS) keyStates[i] = INPUTSTATE_PRESSED;
                else if (keyStates[i] == INPUTSTATE_RELEASED) keyStates[i] = INPUTSTATE_UP;
                else if (keyStates[i] == INPUTSTATE_UP && keyState == GLFW_PRESS) keyStates[i] = INPUTSTATE_PRESSED;
                else if (keyStates[i] == INPUTSTATE_DOWN && keyState == GLFW_RELEASE) keyStates[i] = INPUTSTATE_RELEASED;
            }

            for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++) {
                int buttonState = glfwGetMouseButton(window, i);
                if (buttonStates[i] == INPUTSTATE_PRESSED && buttonState == GLFW_RELEASE) buttonStates[i] = INPUTSTATE_RELEASED;
                else if (buttonStates[i] == INPUTSTATE_PRESSED) buttonStates[i] = INPUTSTATE_DOWN;
                else if (buttonStates[i] == INPUTSTATE_RELEASED && buttonState == GLFW_PRESS) buttonStates[i] = INPUTSTATE_PRESSED;
                else if (buttonStates[i] == INPUTSTATE_RELEASED) buttonStates[i] = INPUTSTATE_UP;
                else if (buttonStates[i] == INPUTSTATE_UP && buttonState == GLFW_PRESS) buttonStates[i] = INPUTSTATE_PRESSED;
                else if (buttonStates[i] == INPUTSTATE_DOWN && buttonState == GLFW_RELEASE) buttonStates[i] = INPUTSTATE_RELEASED;
            }

            scrollDeltaOld = scrollDeltaNew;
            scrollDeltaNew = cg::Vec2d(0,0);
        }

        bool IsMouseButtonDown(int button) {
            return glfwGetMouseButton(cg::GetWindow(), button) == GLFW_PRESS;
        }

        bool IsMouseButtonUp(int button) { return !cg::Input::IsMouseButtonDown(button); }

        bool GetMouseButtonDown(int button) { return buttonStates[button] == INPUTSTATE_PRESSED; }

        bool GetMouseButtonUp(int button) { return buttonStates[button] == INPUTSTATE_RELEASED; }

        int GetMouseButtonState(int button) { return buttonStates[button]; }

        cg::Vec2d GetMousePos(){
            double x, y;
            int h;
            GLFWwindow* window = cg::GetWindow();
            glfwGetCursorPos(window, &x, &y);
            glfwGetWindowSize(window, NULL, &h);
            return cg::Vec2d(x, h - y);
        }

        cg::Vec2d GetMouseScroll(){
            return scrollDeltaOld;
        }

        void SetMouseScroll(cg::Vec2d delta){
            scrollDeltaNew = delta;
        }
    }
    
}