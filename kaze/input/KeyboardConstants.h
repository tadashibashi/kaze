#pragma once
#ifndef kaze_platform_keyboardconstants_h_
#define kaze_platform_keyboardconstants_h_

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

enum class ModKey : Uint
{
    None = 0,
    Shift = 1u,
    Ctrl = 1u << 1u,
    Alt = 1u << 2u,
    Meta = 1u << 3u
};

constexpr ModKey operator |(const ModKey a, const ModKey b) noexcept
{
    return static_cast<ModKey>(static_cast<Uint>(a) | static_cast<Uint>(b));
}

constexpr ModKey &operator |=(ModKey &a, const ModKey b) noexcept
{
    return a = a | b;
}

enum class Key {
    Unknown = -1,
    Escape,
    Return,
    Tab,
    Space,
    Backspace,
    Up,
    Down,
    Left,
    Right,
    Insert,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,
    Print,
    Equal,
    Minus,
    BracketLeft,
    BracketRight,
    Semicolon,
    Quote,
    Comma,
    Period,
    Slash,
    Backslash,
    BackTick,
    CapsLock,
    NumLock,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    ShiftLeft,
    ShiftRight,
    AltLeft,
    AltRight,
    CtrlLeft,
    CtrlRight,
    MetaLeft,
    MetaRight,

    NumPad0,
    NumPad1,
    NumPad2,
    NumPad3,
    NumPad4,
    NumPad5,
    NumPad6,
    NumPad7,
    NumPad8,
    NumPad9,

    NumPadPlus,
    NumPadMinus,
    NumPadMultiply,
    NumPadDivide,
    NumPadDecimal,
    NumPadEnter,

    Number0,
    Number1,
    Number2,
    Number3,
    Number4,
    Number5,
    Number6,
    Number7,
    Number8,
    Number9,

    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    Count
};

Cstring toCstring(Key key);

KAZE_NAMESPACE_END

#endif // kaze_platform_key_h_
