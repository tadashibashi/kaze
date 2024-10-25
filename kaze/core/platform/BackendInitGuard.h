#pragma once
#ifndef kaze_tk_backendinitguard_h_
#define kaze_tk_backendinitguard_h_

#include <kaze/core/lib.h>

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

#endif // kaze_tk_backendinitguard_h_
