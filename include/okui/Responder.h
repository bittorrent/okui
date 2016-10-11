#pragma once

#include "okui/config.h"

#include "okui/Command.h"
#include "okui/Controller.h"
#include "okui/KeyCode.h"
#include "okui/MouseButton.h"
#include "okui/Point.h"

namespace okui {

/**
* Responder objects are capable of receiving and responding to events and commands.
*
* Events are sent to the corresponding window's "first responder", which is usually
* the view that currently has focus. If the view doesn't handle the event it passes
* up to its superview and so on until it eventually reaches the window, and finally
* the application.
*
* When implementing responder methods, you should always invoke the superclass's
* implementation if you're not handling the event yourself. For example, if you
* handle ascii key presses, but not special key presses like "page down", you
* should invoke the superclass's keyDown implementation when you receive the
* key presses that you don't handle.
*/
class Responder {
public:
    virtual ~Responder() {}

    /**
    * Subclasses should almost always implement this to indicate who the next responder should be.
    */
    virtual Responder* nextResponder() { return nullptr; }

    virtual void analogInput(const Controller& controller, size_t axis, double value) {
        if (auto next = nextResponder()) { next->analogInput(controller, axis, value); }
    }

    virtual void buttonDown(const Controller& controller, size_t button) {
        if (auto next = nextResponder()) { next->buttonDown(controller, button); }
    }

    virtual void buttonUp(const Controller& controller, size_t button) {
        if (auto next = nextResponder()) { next->buttonUp(controller, button); }
    }

    virtual void keyDown(KeyCode key, KeyModifiers modifiers, bool repeat) {
        if (auto next = nextResponder()) { next->keyDown(key, modifiers, repeat); }
    }

    virtual void keyUp(KeyCode key, KeyModifiers modifiers, bool repeat) {
        if (auto next = nextResponder()) { next->keyUp(key, modifiers, repeat); }
    }

    virtual void touchDown(size_t finger, Point<double> position, double pressure) {
        if (auto next = nextResponder()) { next->touchDown(finger, position, pressure); }
    }

    virtual void touchUp(size_t finger, Point<double> position, double pressure) {
        if (auto next = nextResponder()) { next->touchUp(finger, position, pressure); }
    }

    virtual void touchMovement(size_t finger, Point<double> position, Point<double> distance, double pressure) {
        if (auto next = nextResponder()) { next->touchMovement(finger, position, distance, pressure); }
    }

    virtual void textInput(const std::string& text) {
        if (auto next = nextResponder()) { next->textInput(text); }
    }

    virtual bool canHandleCommand(Command command) { return false; }

    virtual void handleCommand(Command command, CommandContext context = {}) {
        if (auto next = nextResponder()) { next->handleCommand(command, context); }
    }

    void chainHandleCommand(Command command, CommandContext context = {}) {
        if (canHandleCommand(command)) {
            handleCommand(command, context);
        } else if (auto next = nextResponder()) {
            next->chainHandleCommand(command, context);
        }
    }

    bool chainCanHandleCommand(Command command) {
        return canHandleCommand(command) || (nextResponder() && nextResponder()->chainCanHandleCommand(command));
    }
};

} // namespace okui
