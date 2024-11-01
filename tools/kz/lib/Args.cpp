#include "Args.h"

kz::Args::Args(int argc, char **argv) : m_opts(), m_values()
{
    std::optional<std::string> lastFlag;
    for (int i = 0; i < argc; ++i)
    {
        const char *arg = argv[i];
        if (!arg || arg[0] == '\0')  // just in case?
            continue;

        const bool isFlag = arg[0] == '-';
        if (isFlag)
        {
            int start = 0;
            while (arg[start] == '-') ++start;
                if (arg[start] == '\0')  // skip if just plain dashes
                    continue;

            lastFlag = (arg + start);
        }
        else
        {
            if (lastFlag)
            {
                m_opts[*lastFlag] = arg;
                lastFlag = {};
            }
            else
            {
                m_values.emplace_back(arg);
            }
        }
    }

    if (lastFlag)
    {
        m_opts[lastFlag.value()] = {};
    }
}

kz::Args::Args(Args &&other) noexcept :
    m_opts(std::move(other.m_opts)), m_values(std::move(other.m_values))
{

}

auto kz::Args::operator=(Args &&other) noexcept -> Args &
{
    if (this == &other) return *this;

    m_opts = std::move(other.m_opts);
    m_values = std::move(other.m_values);

    return *this;
}
