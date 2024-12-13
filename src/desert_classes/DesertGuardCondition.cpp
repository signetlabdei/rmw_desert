#include "DesertGuardCondition.h"

DesertGuardCondition::DesertGuardCondition()
      : _has_triggered(false)
{
}

void DesertGuardCondition::trigger()
{
  _has_triggered = true;
}

bool DesertGuardCondition::has_triggered()
{
  return _has_triggered;
}

bool DesertGuardCondition::get_has_triggered()
{
  return _has_triggered.exchange(false);
}
