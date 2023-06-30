#include "Result.h"

Result::~Result()
{
}

bool Result::IsSuccess() const
{
    return m_bIsSuccess;
}

const std::string& Result::GetMessageString() const
{
    return m_sMessage;
}

const std::string& Result::GetCaller() const
{
    return m_sClassName;
}
