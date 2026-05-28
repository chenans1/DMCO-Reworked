#pragma once

namespace settings {

    struct config {
        int dodgeBind = -1;
        float staminaCost = 20.0f;
    };

    config& Get();

    void RegisterMenu();
    void __stdcall RenderMenuPage();

    void load();
    void save();
    int toKeyCode(const RE::ButtonEvent& event);
}