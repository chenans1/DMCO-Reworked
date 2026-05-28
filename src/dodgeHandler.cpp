#include "PCH.h"
#include "dodgeHandler.h"
#include "settings.h"
#include "utils.h"

//logic adapted from https://github.com/ersh1/DodgeFramework/blob/master/src/Events.h
//mostly just edits to work with my SKSE menu framework, some UI closed checks, etc. 

static bool isUIClosed() {
    const auto ui = RE::UI::GetSingleton();
    if (ui && !ui->GameIsPaused() && !ui->IsApplicationMenuOpen() && !ui->IsItemMenuOpen() &&
        !ui->IsMenuOpen(RE::InterfaceStrings::GetSingleton()->dialogueMenu)) {
        return true;
    }
    return false;
}
static bool areControlsEnabled() {
    const auto controlMap = RE::ControlMap::GetSingleton();
    const auto playerControls = RE::PlayerControls::GetSingleton();
    /*if (controlMap->IsFightingControlsEnabled() && playerControls->attackBlockHandler->inputEventHandlingEnabled) {
        return true;
    }
    return false;*/
    if (!controlMap->IsMovementControlsEnabled() || !controlMap->IsLookingControlsEnabled()) {
        return false;
    }
    return true;
}
namespace dodge {
    enum Direction : std::uint32_t {
        kNeutral = 0,
        kForward = 1,
        kRightForward = 2,
        kRight = 3,
        kRightBackward = 4,
        kBackward = 5,
        kLeftBackward = 6,
        kLeft = 7,
        kLeftForward = 8
    };

    // first check if we have enough stasmina to dodge
    // not bothering with handling dodge stamina consumption here, you'd need to check if the animation fired off
    // much better to simply use an animation event in the dodge anim itself to trigger stamina consumption.
    void dodge() {

        if (!isUIClosed() || !areControlsEnabled()) {
            return;
        }

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) {
            return;
        }

        auto playerControls = RE::PlayerControls::GetSingleton();
        if (!playerControls) {
            return;
        }

        auto stamina = player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);
        
        if (settings::percentageCost()) {
            auto max = player->AsActorValueOwner()->GetBaseActorValue(RE::ActorValue::kStamina);
            auto requiredStamina = (settings::staminaCost() / 100) * max;
            if (requiredStamina > stamina) {
                SKSE::log::info("not enough stamina");
                RE::HUDMenu::FlashMeter(RE::ActorValue::kStamina);
                return;
            }
        } else {
            const auto requiredStamina = settings::staminaCost();
            if (requiredStamina > stamina) {
                SKSE::log::info("not enough stamina");
                RE::HUDMenu::FlashMeter(RE::ActorValue::kStamina);
                return;
            }
            
        }
        
        /*if (requiredStamina > stamina) {
            SKSE::log::info("not enough stamina");
            RE::HUDMenu::FlashMeter(RE::ActorValue::kStamina);
            return;
        }*/
        // normalize input vector stuff
        auto normalizedInputDirection = Vec2Normalize(playerControls->data.prevMoveVec);
        if (normalizedInputDirection.x == 0.f && normalizedInputDirection.y == 0.f) {
            player->SetGraphVariableFloat("Dodge_Angle", PI);
            player->SetGraphVariableInt("Dodge_Direction", kNeutral);
            player->NotifyAnimationGraph("Dodge_N");
            player->NotifyAnimationGraph("Dodge");
            SKSE::log::info("neutral");
            return;
        }

        RE::NiPoint2 forwardVector(0.f, 1.f);
        float dodgeAngle = GetAngle(normalizedInputDirection, forwardVector);

        if (dodgeAngle >= -PI8 && dodgeAngle < PI8) {
            player->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
            player->SetGraphVariableInt("Dodge_Direction", kForward);
            player->NotifyAnimationGraph("Dodge_F");
            player->NotifyAnimationGraph("Dodge");
            SKSE::log::info("forward");
        } else if (dodgeAngle >= PI8 && dodgeAngle < 3 * PI8) {
            player->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
            player->SetGraphVariableInt("Dodge_Direction", kRightForward);
            player->NotifyAnimationGraph("Dodge_RF");
            player->NotifyAnimationGraph("Dodge");
            SKSE::log::info("right-forward");
        } else if (dodgeAngle >= 3 * PI8 && dodgeAngle < 5 * PI8) {
            player->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
            player->SetGraphVariableInt("Dodge_Direction", kRight);
            player->NotifyAnimationGraph("Dodge_R");
            player->NotifyAnimationGraph("Dodge");
            SKSE::log::info("right");
        } else if (dodgeAngle >= 5 * PI8 && dodgeAngle < 7 * PI8) {
            player->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
            player->SetGraphVariableInt("Dodge_Direction", kRightBackward);
            player->NotifyAnimationGraph("Dodge_RB");
            player->NotifyAnimationGraph("Dodge");
            SKSE::log::info("right-backward");
        } else if (dodgeAngle >= 7 * PI8 || dodgeAngle < 7 * -PI8) {
            player->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
            player->SetGraphVariableInt("Dodge_Direction", kBackward);
            player->NotifyAnimationGraph("Dodge_B");
            player->NotifyAnimationGraph("Dodge");
            SKSE::log::info("backward");
        } else if (dodgeAngle >= 7 * -PI8 && dodgeAngle < 5 * -PI8) {
            player->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
            player->SetGraphVariableInt("Dodge_Direction", kLeftBackward);
            player->NotifyAnimationGraph("Dodge_LB");
            player->NotifyAnimationGraph("Dodge");
            SKSE::log::info("left-backward");
        } else if (dodgeAngle >= 5 * -PI8 && dodgeAngle < 3 * -PI8) {
            player->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
            player->SetGraphVariableInt("Dodge_Direction", kLeft);
            player->NotifyAnimationGraph("Dodge_L");
            player->NotifyAnimationGraph("Dodge");
            SKSE::log::info("left");
        } else if (dodgeAngle >= 3 * -PI8 && dodgeAngle < -PI8) {
            player->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
            player->SetGraphVariableInt("Dodge_Direction", kLeftForward);
            player->NotifyAnimationGraph("Dodge_LF");
            player->NotifyAnimationGraph("Dodge");
            SKSE::log::info("left-forward");
        }
    }

    RE::BSEventNotifyControl dodgeInputHandler::ProcessEvent(
        RE::InputEvent* const* a_events,
        RE::BSTEventSource<RE::InputEvent*>*) {

        if (!a_events) {
            return RE::BSEventNotifyControl::kContinue;
        }

        const int dodgeBind = settings::dodgeBind();
        if (dodgeBind <= 0) {
            return RE::BSEventNotifyControl::kContinue;
        }

        for (auto ev = *a_events; ev != nullptr; ev = ev->next) {
            auto* btn = ev->AsButtonEvent();
            if (!btn || !btn->IsDown()) continue;
            const int input = settings::toKeyCode(*btn);
            
            if (input == dodgeBind) {
                dodge();
                return RE::BSEventNotifyControl::kContinue;
            }

            if (input != dodgeBind) continue;
            
        }
        return RE::BSEventNotifyControl::kContinue;
    }

}