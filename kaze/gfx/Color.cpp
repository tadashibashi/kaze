#include "Color.h"

KGFX_NS_BEGIN

// ----- grays -----
const Color Color::White = Color(255, 255, 255);
const Color Color::Gray = Color(128, 128, 128);
const Color Color::Black = Color(0, 0, 0);

// ----- primary colors -----
const Color Color::Red = Color(255, 0, 0);
const Color Color::Green = Color(0, 255, 0);
const Color Color::Blue = Color(0, 0, 255);

// ----- secondary colors -----
const Color Color::Cyan = Color(0, 255, 255);
const Color Color::Magenta = Color(255, 0, 255);
const Color Color::Yellow = Color(255, 255, 0);

// ----- tertiary colors -----
const Color Color::Orange = Color(255, 128, 0);
const Color Color::Chartreuse = Color(128, 255, 0);
const Color Color::SpringGreen = Color(0, 255, 128);
const Color Color::Azure = Color(0, 128, 255);
const Color Color::Violet = Color(128, 0, 255);
const Color Color::Rose = Color(255, 0, 128);

KAZE_NS_END
