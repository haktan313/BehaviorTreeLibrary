#include "CustomDecorators.h"

void CooldownDecorator::OnStart()
{
    HDecorator::OnStart();
}

NodeStatus CooldownDecorator::Update()
{
    return HDecorator::Update();
}

void CooldownDecorator::OnFinished()
{
    HDecorator::OnFinished();
}

void CooldownDecorator::OnAbort()
{
    HDecorator::OnAbort();
}

void TimeLimitDecorator::OnStart()
{
    HDecorator::OnStart();
}

NodeStatus TimeLimitDecorator::Update()
{
    return HDecorator::Update();
}

void TimeLimitDecorator::OnFinished()
{
    HDecorator::OnFinished();
}

void TimeLimitDecorator::OnAbort()
{
    HDecorator::OnAbort();
}
