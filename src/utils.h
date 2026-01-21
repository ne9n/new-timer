#ifndef UTILS_H
#define UTILS_H

// Clamp integer value to [lo, hi]
int clampInt(int v, int lo, int hi);

// Compute signed smallest-angle difference in degrees in range [-180,180]
int signedAngleDelta(int cur, int prev);

#endif // UTILS_H
