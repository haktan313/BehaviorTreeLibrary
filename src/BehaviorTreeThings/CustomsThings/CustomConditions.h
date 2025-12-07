#pragma once
#include "imgui.h"
#include "Nodes.h"

struct IsPlayerInRangeParameters : ParamsForCondition
{
    float Range = 100.0f;
    void DrawImGui() override
    {
        ImGui::InputFloat("Range", &Range);
    }
};
class IsPlayerInRangeCondition : public HCondition
{
public:
    IsPlayerInRangeCondition(const std::string& name, const IsPlayerInRangeParameters& params = IsPlayerInRangeParameters{})
        : HCondition(name, params), m_Range(params.Range) {}

    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    float m_Range;
};

struct IsPlayerAttackingParameters : ParamsForCondition
{
    bool bIsPlayerAttacking = true;
    void DrawImGui() override
    {
        ImGui::Checkbox("Is Player Attacking", &bIsPlayerAttacking);
    }
};
class IsPlayerAttackingCondition : public HCondition
{
public:
    IsPlayerAttackingCondition(const std::string& name, const IsPlayerAttackingParameters& params = IsPlayerAttackingParameters{})
        : HCondition(name, params), m_bIsPlayerAttacking(params.bIsPlayerAttacking) {}
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    bool m_bIsPlayerAttacking;
};

struct IsPlayerInSightParameters : ParamsForCondition
{
    float SightDistance = 150.0f;
    float FieldOfView = 90.0f;
    void DrawImGui() override
    {
        ImGui::InputFloat("Sight Distance", &SightDistance);
        ImGui::InputFloat("Field Of View", &FieldOfView);
    }
};
class IsPlayerInSightCondition : public HCondition
{
public:
    IsPlayerInSightCondition(const std::string& name, const IsPlayerInSightParameters& params = IsPlayerInSightParameters{})
        : HCondition(name, params), m_SightDistance(params.SightDistance), m_FieldOfView(params.FieldOfView) {}
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    float m_SightDistance;
    float m_FieldOfView;
};

struct HasAmmoParameters : ParamsForCondition
{
    int MinimumAmmo = 1;
    void DrawImGui() override
    {
        ImGui::InputInt("Minimum Ammo", &MinimumAmmo);
    }
};
class HasAmmoCondition : public HCondition
{
public:
    HasAmmoCondition(const std::string& name, const HasAmmoParameters& params = HasAmmoParameters{})
        : HCondition(name, params), m_MinimumAmmo(params.MinimumAmmo) {}
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    int m_MinimumAmmo;
};