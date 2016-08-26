#pragma once

#include "okui/config.h"

#include <string>

namespace okui {

struct DialogButton {
    explicit DialogButton(std::string label) : label{std::move(label)} {}
    static DialogButton Cancel(std::string label) { DialogButton r{std::move(label)}; r.isCancel = true; return r; }
    static DialogButton Destructive(std::string label) { DialogButton r{std::move(label)}; r.isDestructive = true; return r; }

    std::string label;
    bool isCancel = false;
    bool isDestructive = false;
};

} // namespace okui
