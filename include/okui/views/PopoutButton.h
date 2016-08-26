#pragma once

#include "okui/config.h"

#include "okui/views/Button.h"
#include "okui/views/ImageButton.h"
#include "okui/views/PopoutFocus.h"

namespace okui {
namespace views {

template <typename BaseView>
using PopoutButton = PopoutFocus<Button<BaseView>>;

using PopoutImageButton = PopoutFocus<ImageButton>;

} } // namespace okui::views
