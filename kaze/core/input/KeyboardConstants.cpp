#include "KeyboardConstants.h"

KAZE_NS_BEGIN

static Array<Cstring, static_cast<Int>(Key::Count)> s_keyNames =
{
    "Esc",
    "Return",
    "Tab",
    "Space",
    "Backspace",
    "Up",
    "Down",
    "Left",
    "Right",
    "Insert",
    "Delete",
    "Home",
    "End",
    "PageUp",
    "PageDown",
    "Print",
    "Equal",
    "Minus",
    "LeftBracket",
    "RightBracket",
    "Semicolon",
    "Quote",
    "Comma",
    "Period",
    "Slash",
    "Backslash",
    "Tilde",
    "CapsLock",
    "NumLock",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "ShiftLeft",
    "ShiftRight",
    "AltLeft",
    "AltRight",
    "CtrlLeft",
    "CtrlRight",
    "MetaLeft",
    "MetaRight",
    "NumPad0",
    "NumPad1",
    "NumPad2",
    "NumPad3",
    "NumPad4",
    "NumPad5",
    "NumPad6",
    "NumPad7",
    "NumPad8",
    "NumPad9",
    "NumPadPlus",
    "NumPadMinus",
    "NumPadMultiply",
    "NumPadDivide",
    "NumPadDecimal",
    "NumPadEnter",
    "Key0",
    "Key1",
    "Key2",
    "Key3",
    "Key4",
    "Key5",
    "Key6",
    "Key7",
    "Key8",
    "Key9",
    "KeyA",
    "KeyB",
    "KeyC",
    "KeyD",
    "KeyE",
    "KeyF",
    "KeyG",
    "KeyH",
    "KeyI",
    "KeyJ",
    "KeyK",
    "KeyL",
    "KeyM",
    "KeyN",
    "KeyO",
    "KeyP",
    "KeyQ",
    "KeyR",
    "KeyS",
    "KeyT",
    "KeyU",
    "KeyV",
    "KeyW",
    "KeyX",
    "KeyY",
    "KeyZ",
};

Cstring toCstring(Key key)
{
    return s_keyNames[ static_cast<Int>(key) ];
}

KAZE_NS_END
