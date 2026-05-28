#pragma once

using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

//vtable hooking method for stamina consumption, just in case someone (or me) adds support for NPC DMCO dodging.
namespace stamina {
    class processEventHook {
    public:
        static void Install() {
            log::info("attemping to install player::processEvent Hook...");
            REL::Relocation<std::uintptr_t> vtblPC{RE::VTABLE_PlayerCharacter[2]};

            _originalPC = vtblPC.write_vfunc(0x1, ProcessEvent_PC);
            log::info("installed processEvent hooks....");
        }
        

    private:
        static void HandleEvent(RE::BSAnimationGraphEvent* a_event);

        static RE::BSEventNotifyControl ProcessEvent_PC(
            RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink,
            RE::BSAnimationGraphEvent* a_event,
            RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) {
            HandleEvent(a_event);
            return _originalPC(a_sink, a_event, a_eventSource);
        }

        static inline REL::Relocation<decltype(ProcessEvent_PC)> _originalPC;
    };
}