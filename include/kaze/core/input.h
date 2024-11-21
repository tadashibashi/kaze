#pragma once
#include "lib.h"    // IWYU pragma: export
#include "events.h" // IWYU pragma: export
#include <kaze/internal/core/input/CursorConstants.h>
#include <kaze/internal/core/input/GamepadConstants.h>
#include <kaze/internal/core/input/KeyboardConstants.h>
#include <kaze/internal/core/input/MouseConstants.h>
#include <kaze/internal/core/input/InputMgr.h>

KAZE_PUBLIC_NS_BEGIN

/// Cursor visibility and behavior mode
using KAZE_NS_INTERNAL::CursorMode;

/// Standard OS cursor types
using KAZE_NS_INTERNAL::CursorType;

/// Buttons on a game controller
using KAZE_NS_INTERNAL::GamepadBtn;

/// Gradual axis inputs on a game controller
using KAZE_NS_INTERNAL::GamepadAxis;

/// Keyboard key
using KAZE_NS_INTERNAL::Key;

/// Buttons on a mouse
using KAZE_NS_INTERNAL::MouseBtn;

/// Mouse scroll orientation
using KAZE_NS_INTERNAL::MouseAxis;

using KAZE_NS_INTERNAL::InputMgr;

KAZE_PUBLIC_NS_END
