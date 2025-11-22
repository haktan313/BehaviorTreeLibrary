#include "BlackboardBase.h"

bool HBlackboard::GetBoolValue(const std::string& key) const
{
    if (m_BoolValues.find(key) != m_BoolValues.end())
        return m_BoolValues.at(key);
    return false;
}

int HBlackboard::GetIntValue(const std::string& key) const
{
    if (m_IntValues.find(key) != m_IntValues.end())
        return m_IntValues.at(key);
    return 0;
}

float HBlackboard::GetFloatValue(const std::string& key) const
{
    if (m_FloatValues.find(key) != m_FloatValues.end())
        return m_FloatValues.at(key);
    return 0.0f;
}

std::string HBlackboard::GetStringValue(const std::string& key) const
{
    if (m_StringValues.find(key) != m_StringValues.end())
        return m_StringValues.at(key);
    return "";
}

void HBlackboard::SetBoolValue(const std::string& key, bool value)
{
    if (m_BoolValues.find(key) != m_BoolValues.end())
        m_BoolValues[key] = value;
}

void HBlackboard::SetIntValue(const std::string& key, int value)
{
    if (m_IntValues.find(key) != m_IntValues.end())
        m_IntValues[key] = value;
}

void HBlackboard::SetFloatValue(const std::string& key, float value)
{
    if (m_FloatValues.find(key) != m_FloatValues.end())
        m_FloatValues[key] = value;
}

void HBlackboard::SetStringValue(const std::string& key, const std::string& value)
{
    if (m_StringValues.find(key) != m_StringValues.end())
        m_StringValues[key] = value;
}

void HBlackboard::CreateBoolValue(const std::string& key, bool value)
{
    m_BoolValues[key] = value;
}

void HBlackboard::CreateIntValue(const std::string& key, int value)
{
    m_IntValues[key] = value;
}

void HBlackboard::CreateFloatValue(const std::string& key, float value)
{
    m_FloatValues[key] = value;
}

void HBlackboard::CreateStringValue(const std::string& key, const std::string& value)
{
    m_StringValues[key] = value;
}
