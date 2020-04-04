/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : Interface for the InputSystem class
----------------------------------------------*/
#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include <array>
#include <unordered_map>
#include "InputBinding.h"
#include "Camera.h"

#include <Windows.h>

namespace Input {
    
    class InputSystem
    {
    private:
        // Keyboard States
        std::array<BYTE, 256> m_KeyboardCurrent;
        std::array<BYTE, 256> m_KeyboardPrevious;

        // Mouse States
        POINT m_MouseCurrent;
        POINT m_MousePrevious;

        // Uses GetAsyncKeyState to read in 256 bytes
        void GetKeyboardState();

        // returns the state of the key in enum form
        const KeyState GetKeyboardKeyState(const unsigned int a_keyCode) const;

        // returns true if the key is down
        inline const bool isPressed(int a_keyCode) const
        {
            return (GetAsyncKeyState(a_keyCode) & 0x8000) ? 1 : 0;
        }

        void UpdateKeymaps();
        
        // Main "Update method" for input system
        void GetInput();

    protected:
        std::unordered_map<GameCommands, Chord*> m_keyMap;

        virtual void SetDefaultKeyMap();

    public:
        InputSystem();
        virtual ~InputSystem();

        std::unordered_map<GameCommands, Chord*> m_activeKeyMap;

        // Main "Update" method
        void Frame(float dt, Camera* camera);

        // On WM_MOUSEMOVE message, trigger this method
        void OnMouseMove(short newX, short newY);

        // Returns the current mouse position as a POINT
        POINT GetMousePosition() const { return m_MouseCurrent; }

        // Returns the difference between current and previous as a std::pair
        std::pair<float, float> GetMouseDelta() const;

    };
}
#endif