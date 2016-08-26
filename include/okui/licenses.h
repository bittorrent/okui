#pragma once

#include "okui/config.h"

#include <string>
#include <unordered_map>

namespace okui {

/**
* @return mapping of library names to license text
*/
std::unordered_map<std::string, std::string> ThirdPartyLicenses();

} // namespace okui
