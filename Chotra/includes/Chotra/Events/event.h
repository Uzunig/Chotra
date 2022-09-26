#pragma once

#include <functional>
#include <array>

namespace Chotra {

    enum class EventType {

        None = 0,

        WindowResize,
        WindowClose,

        KeyPressed,
        KeyReleased,

        MouseButtonPrssed,
        MouseButtonReleased,
        MouseMoved,


        EventCount
    };


    struct Event {

        virtual ~Event() = default;
        virtual EventType GetEventType() const = 0;
    };

    class EventDispatcher {
    public:
        template<typename EventType>

        void addEventListener(std::function<void(EventType&)> callback) {
            auto baseCallback = [func = std::move(callback)](Event& e) {
                if (e.GetEventType() == EventType::eventType) {
                    func(static_cast<EventType&>(e));
                }
            };
            eventCallbacks[static_cast<size_t>(EventType::eventType)] = std::move(baseCallback);
        }

        void Dispatch(Event& event) {
            auto& callback = eventCallbacks[static_cast<size_t>(event.GetEventType())];
            if (callback) {
                callback(event);
            }
        }

    private:
        std::array<std::function<void(Event&)>, static_cast<size_t>(EventType::EventCount)> eventCallbacks;
    };

    struct WindowResizeEvent : public Event {

        double width;
        double height;

        static const EventType eventType = EventType::WindowResize;

        WindowResizeEvent(const unsigned int newWidth, const unsigned int newHeight) 
            : width(newWidth), height(newHeight) {

                                    
        }

        virtual EventType GetEventType() const override{

            return eventType;
        }
    };

    struct WindowCloseEvent : public Event {

        static const EventType eventType = EventType::WindowClose;
                
        virtual EventType GetEventType() const override {

            return eventType;
        }
    };

    struct KeyPressedEvent : public Event { //TO DO: optimizatize

        int key; 
        int scancode; 
        int action; 
        int mods;

        static const EventType eventType = EventType::KeyPressed;

        KeyPressedEvent(int key, int scancode, int action, int mods)
            : key(key), scancode(scancode), action(action), mods(mods) {

        }

        virtual EventType GetEventType() const override {

            return eventType;
        }
    };

    struct KeyReleasedEvent : public Event { //TO DO: optimizatize

        int key;
        int scancode;
        int action;
        int mods;

        static const EventType eventType = EventType::KeyReleased;

        KeyReleasedEvent(int key, int scancode, int action, int mods)
            : key(key), scancode(scancode), action(action), mods(mods) {

        }

        virtual EventType GetEventType() const override {

            return eventType;
        }
    };

    struct MouseMovedEvent : public Event {

        double x;
        double y;

        static const EventType eventType = EventType::MouseMoved;
        
        MouseMovedEvent(const double newX, const double newY)
            : x(newX), y(newY) {

        }

        virtual EventType GetEventType() const override{
            return eventType;
        }
    };
} // namspace Chotra