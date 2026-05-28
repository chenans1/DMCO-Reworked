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

    void SprintHandlerHook::ProcessButton(
        RE::SprintHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data) {

        if (!a_this || !a_event || !a_data) {
            return;
        }

        if (!settings::useSprint()) {
            return _ProcessButton(a_this, a_event, a_data);
        }

        auto* player = RE::PlayerCharacter::GetSingleton();
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

        //change sprintstop to keyrelease instead of down
        if (a_event->IsDown()) {
            //never let the game bother process this
            return;
        }

        // here we handle both stop sprint and dodgestart
        else if (a_event->IsUp()) {
            // if we are sprinting, we stop sprint on the keyrelease.
            if (player->IsSprinting()) {
                player->playerFlags.isSprinting = false;
                // player->NotifyAnimationGraph("StopSprint"sv);
            } else {
                // if we are NOT sprinting, and released fast enough, we must dodge.
                if (a_event->heldDownSecs <= settings::sprintDelay()) {
                    player->playerFlags.isSprinting == false;
                    dodge::dodge();
                    return;
                }
            }
        }

        //start sprint if held long enough
        else if (a_event->IsPressed()) {
            //if not sprinting: we sprint
            if (!player->IsSprinting()) {
                if (a_event->heldDownSecs > settings::sprintDelay()) {
                    auto stamina = player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);
                    if (stamina > 0.0f) {
                        player->playerFlags.isSprinting = true;
                        //player->NotifyAnimationGraph("SprintStart"sv);
                    } else {
                        RE::HUDMenu::FlashMeter(RE::ActorValue::kStamina);
                    }
                }
                return _ProcessButton(a_this, a_event, a_data);
            }
        }
        
        
        _ProcessButton(a_this, a_event, a_data);
    }
    
}