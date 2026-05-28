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
    //capture keybind
    /*inline std::atomic_bool g_captureBind{false};

    enum class CaptureTarget : std::uint8_t { None = 0, dodgeBind };
    inline std::atomic<CaptureTarget> g_captureTarget{CaptureTarget::None};
    inline std::atomic_bool g_waitingRelease{false};*/

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
    }
}
