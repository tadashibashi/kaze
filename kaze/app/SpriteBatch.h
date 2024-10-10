#pragma once
#ifndef kaze_app_spritebatch_h_
#define kaze_app_spritebatch_h_

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

class SpriteBatch {
public:
    SpriteBatch();
    ~SpriteBatch();
private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif // kaze_app_spritebatch_h_
