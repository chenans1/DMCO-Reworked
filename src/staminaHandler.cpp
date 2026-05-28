#include "PCH.h"

#include "staminaHandler.h"
#include "settings.h"

using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace stamina {
    void processEventHook::HandleEvent(RE::BSAnimationGraphEvent* a_event) {
        if (!a_event || !a_event->holder || !a_event->tag.data()) return;
        auto* holder = const_cast<RE::TESObjectREFR*>(a_event->holder);
        if (!holder) return;
        auto* actor = holder ? holder->As<RE::Actor>() : nullptr;
        if (!actor) return;

        const auto& tag = a_event->tag;
        const auto& payload = a_event->payload;

        if (tag == "PIE"sv) {
            if (payload == "$DMCO_staminaCost"sv) {
                RE::ActorValueOwner* actorAV = actor->AsActorValueOwner();
                if (!actorAV) {
                    log::warn("No actor value"); return;
                }
                if (settings::percentageCost()) {
                    auto max = actorAV->GetBaseActorValue(RE::ActorValue::kStamina);
                    auto cost = (settings::staminaCost() / 100) * max;
                    actorAV->DamageActorValue(RE::ActorValue::kStamina, cost);
                } else {
                    actorAV->DamageActorValue(RE::ActorValue::kStamina, settings::staminaCost());
                }
                log::info("Damaged Stamina");
            }
        }
    }
}