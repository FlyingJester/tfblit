#include "JestMath.h"

int imin(int a, int b){
  if(a<b) return a; return b;
}

int imax(int a, int b){
  if(a>b) return a; return b;
}

float fdistance(float x1, float y1, float x2, float y2){
    float dx = x1-x2;
    float dy = y1-y2;
    return sqrt((dx*dx)+(dy*dy));
}

