#pragma once

//class to handle dodge input. 
namespace dodge {
    class dodgeInputHandler : public RE::BSTEventSink<RE::InputEvent*> {
    public:
        static dodgeInputHandler& GetSingleton() {
            static dodgeInputHandler singleton;
            return singleton;
        }

        RE::BSEventNotifyControl ProcessEvent(
            RE::InputEvent* const* a_events,
            RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

    private: 
        dodgeInputHandler() = default;
        ~dodgeInputHandler() = default;
        dodgeInputHandler(const dodgeInputHandler&) = delete;
        dodgeInputHandler(dodgeInputHandler&&) = delete;
        dodgeInputHandler& operator=(const dodgeInputHandler&) = delete;
        dodgeInputHandler& operator=(dodgeInputHandler&&) = delete;
    };
}