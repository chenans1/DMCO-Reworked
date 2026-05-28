#include "PCH.h"
#include "settings.h"
#include <SimpleIni.h>
#include <SKSEMenuFramework.h>

using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

static bool ini_bool(CSimpleIniA& ini, const char* section, const char* key, bool def) {
    return ini.GetLongValue(section, key, def ? 1L : 0L) != 0;
}

static float ini_float(CSimpleIniA& ini, const char* section, const char* key, float def) {
    return static_cast<float>(ini.GetDoubleValue(section, key, def));
}

namespace settings {
    //use atomics since IIRC the UI elements are usually not ran on main thread unlike rest of game logic
    static std::atomic_bool g_captureDodgeBind{false};
    static std::atomic_bool g_waitingRelease{false};

    static config cfg{};
    config& Get() { return cfg; }

    void load() {
        constexpr auto path = "Data/SKSE/Plugins/DodgeFramework.ini";
        CSimpleIniA ini;

        ini.SetUnicode(false);
        const SI_Error rc = ini.LoadFile(path);

        if (rc < 0) {
            log::warn("Could not load ini '{}'. Using defaults.", path);
            return;
        }
        auto& c = Get();
        c.dodgeBind = static_cast<int>(ini.GetLongValue("general", "dodgeBind", c.dodgeBind));
        c.staminaCost = ini_float(ini, "general", "staminaCost", c.staminaCost);
        c.percentCost = ini_bool(ini, "General", "usePercentCost", c.percentCost);
        //c.secondDodgeMult = ini_float(ini, "general", "secondDodgeMult", c.secondDodgeMult);
        c.useSprint = ini_bool(ini, "General", "useSprintKey", c.useSprint);
        c.sprintDelay = ini_float(ini, "general", "sprintHoldDuration", c.sprintDelay);

        log::info("Settings Loaded: dodgeBind={}, staminaCost={}", 
            c.dodgeBind, c.staminaCost);
    }

    void save() {
        constexpr const char* path = "Data/SKSE/Plugins/DodgeFramework.ini";
        auto& c = Get();
        CSimpleIniA ini;
        ini.SetUnicode(false);

        // on fail create new file
        (void)ini.LoadFile(path);

        ini.SetLongValue("general", "dodgeBind", c.dodgeBind);
        ini.SetDoubleValue("general", "staminaCost", static_cast<double>(c.staminaCost), "%.3f");
        ini.SetLongValue("general", "usePercentCost", c.percentCost);
        //ini.SetDoubleValue("general", "secondDodgeMult", static_cast<double>(c.secondDodgeMult), "%.3f");
        ini.SetLongValue("general", "useSprintKey", c.useSprint);
        ini.SetDoubleValue("general", "sprintHoldDuration", static_cast<double>(c.sprintDelay), "%.3f");


        const SI_Error rc = ini.SaveFile(path);
        if (rc < 0) {
            log::error("Failed to save ini '{}'. SI_Error={}", path, static_cast<int>(rc));
            return;
        }

        log::info("Saved ini '{}'", path);
    }


    //skse menu framework input
    bool __stdcall OnInput(RE::InputEvent* event) {
        if (!g_captureDodgeBind.load(std::memory_order_acquire)) {
            return false;  
        }

        if (!event) {
            return true;
        }
        auto* btn = event->AsButtonEvent();
        if (!btn) {
            return true;
        }

        auto& c = Get();
        
        if (g_waitingRelease.load(std::memory_order_acquire)) {
            if (!btn->IsDown()) {
                g_waitingRelease.store(false, std::memory_order_release);
            }
            return true;
        }

        if (btn->IsDown()) {
            return true;
        }

        // ESC = unbind and exit
        if (btn->device.get() == RE::INPUT_DEVICE::kKeyboard) {
            if (btn->GetIDCode() == 0x01) {
                c.dodgeBind = -1;
                g_captureDodgeBind.store(false, std::memory_order_release);
                g_waitingRelease.store(false, std::memory_order_release);
                return true;
            }
        }

        const int key = toKeyCode(*btn);

        if (key != 0) {
            c.dodgeBind = key;
            g_captureDodgeBind.store(false, std::memory_order_release);
            g_waitingRelease.store(false, std::memory_order_release);
        }

        return true;
    }

    void __stdcall RenderMenuPage() {
        auto& c = Get();
        static bool unsaved = false;

        const bool capturing = g_captureDodgeBind.load(std::memory_order_acquire);
        ImGuiMCP::Text("Dodge Keybind: %d", c.dodgeBind);
        if (capturing) {
            ImGuiMCP::SameLine();
            ImGuiMCP::TextUnformatted("Press a key... (ESC = unbind which disables)");

            if (ImGuiMCP::Button("Cancel")) {
                g_captureDodgeBind.store(false, std::memory_order_release);
                g_waitingRelease.store(false, std::memory_order_release);
            }
        } else {

            if (ImGuiMCP::Button("Rebind Dodge")) {
                g_captureDodgeBind.store(true, std::memory_order_release);
                g_waitingRelease.store(true, std::memory_order_release);
            }

            ImGuiMCP::SameLine();

            if (ImGuiMCP::Button("Clear Dodge Bind")) {
                c.dodgeBind = -1;
                unsaved = true;
            }
        
        }

        unsaved |= ImGuiMCP::DragFloat("Dodge Stamina Cost Requirement", &c.staminaCost, 1.0f, 0.0f, 100.0f, "%.2f");
        unsaved |= ImGuiMCP::Checkbox("Use Percent Cost", &c.percentCost);

        //test: sprint input button
        unsaved |= ImGuiMCP::Checkbox("Use Sprint Key", &c.useSprint);
        ImGuiMCP::TextUnformatted("Dodge is triggered on sprint key release. Holding the key for longer than duration below sprints instead.");
        unsaved |= ImGuiMCP::DragFloat("Hold Duration for sprint", &c.sprintDelay, 0.01f, 0.01f, 1.0f, "%.2f");

        

        if (ImGuiMCP::Button("Save")) {
            save();
            unsaved = false;
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button("Revert")) {
            load();
            unsaved = false;
        }

        if (unsaved) {
            ImGuiMCP::TextUnformatted("Unsaved changes");
        }
    }

    void RegisterMenu() {
        if (!SKSEMenuFramework::IsInstalled()) {
            SKSE::log::warn("SKSE Menu Framework not installed; skipping menu registration.");
            return;
        }
        log::info("RegisterMenu Installed()");
        SKSEMenuFramework::SetSection("DMCO");
        SKSEMenuFramework::AddSectionItem("Settings", RenderMenuPage);
        SKSEMenuFramework::AddInputEvent(OnInput);
    }

    int toKeyCode(const RE::ButtonEvent& event) {
        const auto device = event.device.get();
        const auto id = event.GetIDCode();

        switch (device) {
            case RE::INPUT_DEVICE::kKeyboard:
                return static_cast<int>(id);

            case RE::INPUT_DEVICE::kMouse:
                return static_cast<int>(id + SKSE::InputMap::kMacro_MouseButtonOffset);

            case RE::INPUT_DEVICE::kGamepad:
                return static_cast<int>(SKSE::InputMap::GamepadMaskToKeycode(id));

            default:
                return 0;
        }
    }
}
