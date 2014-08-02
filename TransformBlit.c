#include "TransformBlit.h"
#include "Bresenham.h"
#include <assert.h>

int TransformBlit(SDL_Surface *src, SDL_Surface *dst, struct Point crd[4]){

    struct Point crd_zero_one[2];
    struct Point crd_three_two[2];
    crd_zero_one[0].x = crd[0].x;
    crd_zero_one[0].y = crd[0].y;
    crd_zero_one[1].x = crd[1].x;
    crd_zero_one[1].y = crd[1].y;
    crd_three_two[0].x = crd[3].x;
    crd_three_two[0].y = crd[3].y;
    crd_three_two[1].x = crd[2].x;
    crd_three_two[1].y = crd[2].y;

    printf("[tfblit] CRD: %i, %i, %i, %i\n", crd[0].x, crd[0].y, crd[1].x,
           crd[1].y);
    printf("[tfblit] CRD: %i, %i, %i, %i\n", crd[2].x, crd[2].y, crd[3].x,
           crd[3].y);
    int size_zero_one, size_three_two;

    struct Point *zero_one = CalcLine(crd_zero_one, &size_zero_one);
    struct Point *three_two = CalcLine(crd_three_two, &size_three_two);

    printf("[tfblit] zero_one: %i, %i, %i, %i\n", zero_one[0].x, zero_one[0].y,
           zero_one[1].x, zero_one[1].y);
    printf("[tfblit] three_two: %i, %i, %i, %i\n", three_two[0].x,
           three_two[0].y, three_two[1].x, three_two[1].y);
    float at = 0;
    float f_run = size_three_two;
    float step = size_zero_one/size_three_two;
    int size_line, iat, i = 0;

    struct Point lLine[2];

    SDL_LockSurface(dst);
    SDL_LockSurface(src);

    while((at < f_run) && (i < size_zero_one)){
      iat = at;

      lLine[0].x = zero_one[i].x;
      lLine[0].y = zero_one[i].y;
      lLine[1].x = three_two[iat].x;
      lLine[1].y = three_two[iat].y;


      struct Point *lPoints = CalcLine(lLine, &size_line);
    printf("[tfblit] lPoints: %i, %i, %i, %i\n", lPoints[0].x, lPoints[0].y,
                                                 lPoints[1].x, lPoints[1].y);

      if(size_line==0)
        continue;

      float U = fmin(at/f_run, 1.0f);
      float V = 0.0f;

      for(int i = 0; i<size_line; i++){
          V = i;
          V/= (float)size_line;
          V = fmin(V, 1.0f);
          uint32_t *pix = &(((uint32_t *)dst->pixels)[
              lPoints[i].x + (lPoints[i].y * dst->w) ]);

          int lx = imin(U*((float)src->w), src->w);
          int ly = imin(V*((float)src->h), src->h);

          *pix = ((uint32_t *)src->pixels)[lx + (ly * src->w)];
      }


      free(lPoints);

      i++;
      at+=step;
    }

    free(zero_one);
    free(three_two);

    SDL_UnlockSurface(dst);
    SDL_UnlockSurface(src);

    return 0;
}
