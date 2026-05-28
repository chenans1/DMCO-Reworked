#pragma once

namespace settings {

    struct config {
        int dodgeBind = -1;
        float staminaCost = 20.0f;
    };

    config& Get();
    inline int dodgeBind() { return Get().dodgeBind; }
    inline float staminaCost() { return Get().staminaCost; }
    void RegisterMenu();
    void __stdcall RenderMenuPage();

    void load();
    void save();
    int toKeyCode(const RE::ButtonEvent& event);
}