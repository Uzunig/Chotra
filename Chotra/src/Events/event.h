#pragma once

namespace Chotra {

    enum class EventType {

        None = 0,

        WindowResize,
        WindowClose,

        KeyPressed,
        KeyReleasd,

        MouseButtonPrssed,
        MouseButtonReleased,
        MouseMoved,


        EventCount
    };


    struct Event {

        virtual ~Event() = default;
        virtual EventType GetEventType() const = 0;
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
}