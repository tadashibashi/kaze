#pragma once
#ifndef nova_macros_json_h_
#define nova_macros_json_h_

#include <nova/nova.h>
#include <nlohmann/json.hpp>

NOVA_NAMESPACE_BEGIN

using json = nlohmann::json;

NOVA_NAMESPACE_END

#endif // nova_macros_json_h_
