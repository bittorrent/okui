#pragma once

#include "onair/okui/Command.h"
#include "onair/okui/KeyCode.h"

#include <string>
#include <vector>

namespace onair {
namespace okui {

class Application;
class MenuItem;

class Menu {
public:
    Menu() {}
    Menu(const std::vector<MenuItem>& items) : _items(items) {}

    /**
    * Returns the top level items of the menu bar.
    */
    const std::vector<MenuItem>& items() const { return _items; }
    std::vector<MenuItem>& items() { return _items; }

    bool isEmpty() const { return _items.empty(); }

private:
    std::vector<MenuItem> _items;
};

class MenuItem {
public:
    enum class State {
        kOff,   // nothing
        kMixed, // dash
        kOn,    // check mark
    };

    /**
    * Creates a non-interactive separator item.
    */
    MenuItem() {}

    /**
    * Creates an item with the given label and command.
    */
    MenuItem(const char* label, Command command, KeyCode keyCode = KeyCode::kNone, KeyModifiers keyModifiers = KeyModifier::kNone)
        : _label(label), _command(command), _keyCode(keyCode), _keyModifiers(keyModifiers) {}

    /**
    * Creates an item with the given label and command.
    */
    MenuItem(const char* label, Command command, CommandContext commandContext, KeyCode keyCode = KeyCode::kNone, KeyModifiers keyModifiers = KeyModifier::kNone)
        : _label(label), _command(command), _commandContext(commandContext), _keyCode(keyCode), _keyModifiers(keyModifiers) {}

    /**
    * Creates an item with the given label and submenu.
    */
    MenuItem(const char* label, const Menu& submenu) : _label(label), _submenu(submenu) {}

    void setState(State state) { _state = state; }
    /**
    * Indicates whether or not the item is a separator rather than interactive item.
    */
    bool isSeparator() const { return _label.empty(); }

    /**
    * Returns the label for the item (if it's not a separator).
    */
    const std::string& label() const { return _label; }

    /**
    * Returns the command that the item should dispatch when selected.
    */
    const Command& command() const { return _command; }

    /**
    * Returns the context for the command that the item should dispatch when selected.
    */
    const CommandContext& commandContext() const { return _commandContext; }

    /**
    * Returns the key code that can be used to dispatch the menu item's command.
    */
    const KeyCode& keyCode() const { return _keyCode; }

    const KeyModifiers& keyModifiers() const { return _keyModifiers; }

    const Menu& submenu() const { return _submenu; }
    Menu& submenu() { return _submenu; }

    State state() const { return _state; }

private:
    std::string    _label;
    Command        _command = kCommandNone;
    CommandContext _commandContext;
    KeyCode        _keyCode = KeyCode::kNone;
    KeyModifiers   _keyModifiers = KeyModifier::kNone;
    Menu           _submenu;
    State          _state = State::kOff;
};

}}
