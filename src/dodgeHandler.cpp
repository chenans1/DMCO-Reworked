#include "PCH.h"
#include "settings.h"

// adapted from https://github.com/ersh1/DodgeFramework/blob/master/src/Events.h
static bool isUIClosed() {
    const auto ui = RE::UI::GetSingleton();
    if (ui && !ui->GameIsPaused() && !ui->IsApplicationMenuOpen() && !ui->IsItemMenuOpen() &&
        !ui->IsMenuOpen(RE::InterfaceStrings::GetSingleton()->dialogueMenu)) {
        return true;
    }
    return false;
}

namespace dodge {

}