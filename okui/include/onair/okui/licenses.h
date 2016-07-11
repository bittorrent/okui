#pragma once

#include "onair/okui/config.h"

#include <string>
#include <unordered_map>

namespace onair {
namespace okui {

/**
* @return mapping of library names to license text
*/
std::unordered_map<std::string, std::string> ThirdPartyLicenses();

}} // namespace onair::okui
