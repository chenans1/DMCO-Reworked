#pragma once

namespace settings {

    struct config {
        int dodgeBind = -1;
        float staminaCost = 20.0f;
        bool useSprint = false;
        bool percentCost = false;
        float sprintDelay = 0.1f;
        float secondDodgeMult = 1.0f;
    };

    config& Get();

    inline int dodgeBind() { return Get().dodgeBind; }
    inline float staminaCost() { return Get().staminaCost; }
    inline float secondDodgeMult() { return Get().secondDodgeMult; }
    inline bool percentageCost() { return Get().percentCost; }
    inline bool useSprint() { return Get().useSprint; }
    inline float sprintDelay() { return Get().sprintDelay; }

    void RegisterMenu();
    void __stdcall RenderMenuPage();

    void load();
    void save();
    int toKeyCode(const RE::ButtonEvent& event);
}