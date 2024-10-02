#include "errors.h"
#include <utility>

KAZE_NAMESPACE_BEGIN

thread_local Error s_curError{};

Error::Error(Error &&other) noexcept :
    code(other.code), message(std::move(other.message)), line(other.line), file(other.file)
{

}

Error &Error::operator=(Error &&other) noexcept
{
    this->message = std::move(other.message);
    this->code = other.code;
    this->line = other.line;
    this->file = other.file;

    return *this;
}

Error getError() noexcept
{
    const auto moved = std::move(s_curError);
    s_curError = {};

    return moved;
}

const Error &setError(const StringView message, const Error::Code code, const Cstring filename, const int line) noexcept
{
    s_curError = Error{code, String(message), filename, line};
    return s_curError;
}

void clearError() noexcept
{
    s_curError = {};
}

bool hasError() noexcept
{
    return s_curError.code != Error::Code::Ok;
}

KAZE_NAMESPACE_END
