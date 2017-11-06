#include "VideoPCH.h"
#undef M_PI
#define _USE_MATH_DEFINES
#include <math.h>
#include <utils/Utils.h>
#include "PotentiometerModel.h"

PotentiometerModel::PotentiometerModel()
{
  utils::zero<PotentiometerModelData>(*this);
}

PotentiometerModel::PotentiometerModel( const PotentiometerModel& src ) :
  PotentiometerModelData(src)
{
}

void PotentiometerModel::update( float deltaTime )
{
  // opór
  float frictionMoment = pushed ? clickFriction : airFriction;

  // prędkość bez uwzględnienia oporu
  angularVelocity = angularVelocity + (forcedMoment / momentOfInteria) * deltaTime;
  // delta wynikająca z oporu (przeciwna do prędkości)
  float frictionVelocity = -1.0f * utils::sign(angularVelocity) * ( frictionMoment / momentOfInteria ) * deltaTime;

  if ( fabs(angularVelocity) < fabs(frictionVelocity) ) {
    // ciało jest w spoczynku (nie udało się pokonać tarcia)
    angularVelocity = 0.0;
  } else {
    // odejmujemy wartość wynikającą z oporu
    angularVelocity += frictionVelocity;
  }

  // delta kątowa
  delta = 0.5f * angularVelocity * deltaTime;
  // zerujemy wymuszony moment obrotowy
  forcedMoment = 0.0f;

  // nowa wartość
  float prevValue = value;
  value += delta;

  // czy dotarliśmy do granicy?
  if ( value < minValue || value > maxValue ) {
    // zerujemy fizykę, bo jesteśmy na końcu
    value = utils::clamp(value, minValue, maxValue);
    delta = value - prevValue;
    angularVelocity = 0.0f;
  }

  // aktualizacja
  notify();
}
