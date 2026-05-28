#include "PCH.h"

#include "dodgeHandler.h"
#include "sprintInput.h"
#include "settings.h"

using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;
//taken from: https://github.com/ersh1/DodgeFramework/blob/master/src/Hooks.cpp

namespace sprint {
    void Install() {
        log::info("Hooking sprintProcessEvent...");

        SprintHandlerHook::Hook();

        log::info("...success");
    }

    static bool bStoppingSprint = false;

    void SprintHandlerHook::ProcessButton(
        RE::SprintHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data) {

        if (settings::useSprint()) {
            auto playerCharacter = RE::PlayerCharacter::GetSingleton();
            auto userEvent = a_event->QUserEvent();
            auto userEvents = RE::UserEvents::GetSingleton();

            if (a_event->IsDown() && playerCharacter->playerFlags.isSprinting) {  // stopping sprint
                bStoppingSprint = true;
            } else if (a_event->HeldDuration() < settings::sprintDelay()) {
                if (a_event->IsUp()) {
                    dodge::dodge();
                    bStoppingSprint = false;
                }
                return;
            } else if (playerCharacter && playerCharacter->playerFlags.isSprinting && !bStoppingSprint) {
                a_event->heldDownSecs = 0.f;
            } else if (a_event->IsUp()) {
                bStoppingSprint = false;
            }
        }

        _ProcessButton(a_this, a_event, a_data);
    }
    
}