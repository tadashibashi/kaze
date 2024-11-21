#pragma once

#include <kaze/internal/core/lib.h>

KAZE_NS_BEGIN

/// Initializes and shutsdown backend as necessary
class BackendInitGuard {
public:
    BackendInitGuard();
    ~BackendInitGuard();
private:
    Bool m_wasInit;
};

KAZE_NS_END
