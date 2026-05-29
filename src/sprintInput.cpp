#include "PCH.h"

#include "dodgeHandler.h"
#include "sprintInput.h"
#include "settings.h"

using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;
//taken from: https://github.com/ersh1/DodgeFramework/blob/master/src/Hooks.cpp
//reworked as original implementation didn't seem to work without classic sprinting redone.
//almost certainly imcompatible if you use skyparkour sprint key parkour. 

namespace sprint {

    void Install() {
        log::info("Hooking sprintProcessEvent...");

        SprintHandlerHook::Hook();

        log::info("...success");
    }

    static bool bStoppingSprint = false;

    static bool IsPlayerSprinting(RE::PlayerCharacter* player) {
        if (!player) {
            return false;
        }

        /*bool isSprinting = false;
        player->GetGraphVariableBool("IsSprinting", isSprinting);
        return isSprinting;*/
        return player->IsSprinting();
    }

    void SprintHandlerHook::ProcessButton(
        RE::SprintHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data) {

        if (!a_this || !a_event || !a_data) {
            return;
        }

        if (!settings::useSprint()) {
            return _ProcessButton(a_this, a_event, a_data);
        }

        auto player = RE::PlayerCharacter::GetSingleton();
        if (!player) {
            return _ProcessButton(a_this, a_event, a_data);
        }

        auto* userEvents = RE::UserEvents::GetSingleton();
        if (!userEvents) {
            return _ProcessButton(a_this, a_event, a_data);
        }

        const auto userEvent = a_event->QUserEvent();

        /*if (userEvent != userEvents->sprint) {
            return _ProcessButton(a_this, a_event, a_data);
        }*/

        /*const bool isSprinting = IsPlayerSprinting(player);*/
        const bool isSprinting = player->GetPlayerRuntimeData().playerFlags.isSprinting;
        if (a_event->IsDown() && isSprinting) {
            bStoppingSprint = true;
        } else if (a_event->HeldDuration() < settings::sprintDelay()) {
            if (a_event->IsUp()) {
                dodge::dodge();
                bStoppingSprint = false;
            }
            return;
        } else if (!isSprinting && !bStoppingSprint) {
            a_event->heldDownSecs = 0.0f;
            //a_event->value = 0.0f;
        } else if (a_event->IsUp()) {
            bStoppingSprint = false;
        }

        _ProcessButton(a_this, a_event, a_data);
    }
    
}