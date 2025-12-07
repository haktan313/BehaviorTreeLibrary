#pragma once
#include "imgui.h"
#include "Nodes.h"


struct MoveToParameters : Params
{
    bool bToPlayer = true;
    float StopDistance = 10.0f;
    void DrawImGui() override
    {
        ImGui::Checkbox("Move To Player", &bToPlayer);
        ImGui::InputFloat("Stop Distance", &StopDistance);
    }
};
class MoveToAction : public HActionNode
{
public:
    MoveToAction(const std::string& name, const MoveToParameters& params = MoveToParameters{})
        : HActionNode(name, params), m_bToPlayer(params.bToPlayer), m_StopDistance(params.StopDistance) {}
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    bool m_bToPlayer;
    float m_StopDistance;
};

//Melee Enemy Actions
struct MeleeEnemyAttackActionParameters : Params
{
    int AttackPower = 10;
    void DrawImGui() override
    {
        ImGui::InputInt("Attack Power", &AttackPower);
    }
};
class MeleeEnemyAttackAction : public HActionNode
{
public:
    MeleeEnemyAttackAction(const std::string& name, const MeleeEnemyAttackActionParameters& params = MeleeEnemyAttackActionParameters{})
        : HActionNode(name, params), m_AttackPower(params.AttackPower) {}

    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    int m_AttackPower;
};

struct MeleeEnemyDodgeActionParameters : Params
{
    float DodgeDistance = 5.0f;
    void DrawImGui() override
    {
        ImGui::InputFloat("Dodge Distance", &DodgeDistance);
    }
};
class MeleeEnemyDodgeAction : public HActionNode
{
public:
    MeleeEnemyDodgeAction(const std::string& name, const MeleeEnemyDodgeActionParameters& params = MeleeEnemyDodgeActionParameters{})
        : HActionNode(name, params), m_DodgeDistance(params.DodgeDistance) {}
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    float m_DodgeDistance;
};


//Ranged Enemy Actions
struct RangedEnemyShootActionParameters : Params
{
    int AmmoAmount = 5;
    float ShootInterval = 1.0f;
    int Damage = 15;
    void DrawImGui() override
    {
        ImGui::InputInt("Ammo Amount", &AmmoAmount);
        ImGui::InputFloat("Shoot Interval", &ShootInterval);
        ImGui::InputInt("Damage", &Damage);
    }
};
class RangedEnemyShootAction : public HActionNode
{
public:
    RangedEnemyShootAction(const std::string& name, const RangedEnemyShootActionParameters& params = RangedEnemyShootActionParameters{})
        : HActionNode(name, params), m_AmmoAmount(params.AmmoAmount), m_ShootInterval(params.ShootInterval), m_Damage(params.Damage) {}

    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    int m_AmmoAmount;
    float m_ShootInterval;
    int m_Damage;
};
struct ReloadPistolActionParameters : Params
{
    int AmmoToReload = 5;
    float ReloadTime = 2.0f;
    void DrawImGui() override
    {
        ImGui::InputInt("Ammo To Reload", &AmmoToReload);
        ImGui::InputFloat("Reload Time", &ReloadTime);
    }
};
class ReloadPistolAction : public HActionNode
{
public:
    ReloadPistolAction(const std::string& name, const ReloadPistolActionParameters& params = ReloadPistolActionParameters{})
        : HActionNode(name, params), m_AmmoToReload(params.AmmoToReload), m_ReloadTime(params.ReloadTime) {}
    void OnStart() override;
    NodeStatus Update() override;
    void OnFinished() override;
    void OnAbort() override;
private:
    int m_AmmoToReload;
    float m_ReloadTime;
};