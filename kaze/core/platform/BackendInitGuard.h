#pragma once

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
