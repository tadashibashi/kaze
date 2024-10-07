#pragma once
#ifndef kaze_app_backendinitguard_h_
#define kaze_app_backendinitguard_h_

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

/// Initializes and shutsdown backend as necessary
class BackendInitGuard {
public:
    BackendInitGuard();
    ~BackendInitGuard();
private:
    Bool m_wasInit;
};

KAZE_NAMESPACE_END

#endif