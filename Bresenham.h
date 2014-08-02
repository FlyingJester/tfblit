#pragma once
#include "JestMath.h"

#define ALG_PI  3.14159265358979323846f
#define ALG_PI2 6.28318530717958647692f

#define DIFF(A, B) ((A>B)?\
          (A-B) :\
          (B-A) )

struct Point *CalcLine(const struct Point /*in*/ *aLine, int * /*out*/ oSteps);
