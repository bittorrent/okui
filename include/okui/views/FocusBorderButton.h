#pragma once

#include <okui/config.h>

#include <okui/views/Button.h>
#include <okui/views/ImageButton.h>
#include <okui/views/FocusBorder.h>

namespace okui::views {

template <typename BaseView>
using FocusBorderButton = FocusBorder<Button<BaseView>>;

using FocusBorderImageButton = FocusBorder<ImageButton>;

} // namespace okui::views
