#pragma once

//class to handle dodge input. 
namespace dodge {
    class dodgeInputSink : public RE::BSTEventSink<RE::InputEvent*> {
    public:
        static dodgeInputSink& GetSingleton() {
            static dodgeInputSink singleton;
            return singleton;
        }

        RE::BSEventNotifyControl ProcessEvent(
            RE::InputEvent* const* a_events,
            RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

    private: 
        dodgeInputSink() = default;
        ~dodgeInputSink() = default;
        dodgeInputSink(const dodgeInputSink&) = delete;
        dodgeInputSink(dodgeInputSink&&) = delete;
        dodgeInputSink& operator=(const dodgeInputSink&) = delete;
        dodgeInputSink& operator=(dodgeInputSink&&) = delete;
    };
}