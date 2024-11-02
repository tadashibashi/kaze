/// \file json.h
/// Contains includes, defines and helpers for json functionality
#pragma once
#ifndef kaze_core_json_h_
#define kaze_core_json_h_

#include <kaze/core/lib.h>
#include <nlohmann/json.hpp>

KAZE_NS_BEGIN

using json = nlohmann::json;

KAZE_NS_END
