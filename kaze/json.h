/// @file json.h
/// Contains includes, defines and helpers for json functionality
#pragma once
#ifndef kaze_json_h_
#define kaze_json_h_

#include <kaze/kaze.h>
#include <nlohmann/json.hpp>

KAZE_NAMESPACE_BEGIN

using json = nlohmann::json;

KAZE_NAMESPACE_END

#endif // kaze_json_h_
