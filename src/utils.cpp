#include "utils.h"

int clampInt(int v, int lo, int hi)
{
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

int signedAngleDelta(int cur, int prev)
{
  int d = cur - prev;
  if (d > 180) d -= 360;
  else if (d < -180) d += 360;
  return d;
}
