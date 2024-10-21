#include "BackendInitGuard.h"

#include "backend.h"

KAZE_NAMESPACE_BEGIN

static int s_timesInit;
static std::mutex s_lock;

BackendInitGuard::BackendInitGuard() : m_wasInit()
{
    std::lock_guard lockGuard(s_lock);

    if (s_timesInit == 0)
    {
        m_wasInit = backend::init();
    }
    else
    {
        m_wasInit = true;
    }

    if (m_wasInit)
        ++s_timesInit;
}

BackendInitGuard::~BackendInitGuard()
{
    std::lock_guard lockGuard(s_lock);
    if (m_wasInit)
    {
        --s_timesInit;
        if (s_timesInit <= 0)
            backend::shutdown();
    }
}

KAZE_NAMESPACE_END
