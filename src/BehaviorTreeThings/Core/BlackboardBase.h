#pragma once
#include <string>
#include <unordered_map>

class HBlackboard
{
public:
    HBlackboard() = default;
    virtual ~HBlackboard() = default;
    
    bool GetBoolValue(const std::string& key) const;
    int GetIntValue(const std::string& key) const;
    float GetFloatValue(const std::string& key) const;
    const std::string& GetStringValue(const std::string& key) const;

    bool IsValuesChanged() const { return m_bValuesChanged; }
    void ClearValuesChangedFlag() { m_bValuesChanged = false; }

    const std::unordered_map<std::string, bool>& GetBoolValues() const { return m_BoolValues; }
    const std::unordered_map<std::string, int>& GetIntValues() const { return m_IntValues; }
    const std::unordered_map<std::string, float>& GetFloatValues() const { return m_FloatValues; }
    const std::unordered_map<std::string, std::string>& GetStringValues() const { return m_StringValues; }
    
    void SetBoolValue(const std::string& key, bool value);
    void SetIntValue(const std::string& key, int value);
    void SetFloatValue(const std::string& key, float value);
    void SetStringValue(const std::string& key, const std::string& value);
    void DrawImGui();
protected:
    void CreateBoolValue(const std::string& key, bool value);
    void CreateIntValue(const std::string& key, int value);
    void CreateFloatValue(const std::string& key, float value);
    void CreateStringValue(const std::string& key, const std::string& value);

private:
    std::unordered_map<std::string, bool> m_BoolValues;
    std::unordered_map<std::string, int> m_IntValues;
    std::unordered_map<std::string, float> m_FloatValues;
    std::unordered_map<std::string, std::string> m_StringValues;

    bool m_bValuesChanged = false;
};
