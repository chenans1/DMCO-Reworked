#pragma once

namespace settings {
    struct config {
        int dodgeBind = -1;
        float staminaCost = 20.0f;
    }
    config& Get();
}