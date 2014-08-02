#include "TransformBlit.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define Frmask 0x000000FF
#define Fgmask 0x0000FF00
#define Fbmask 0x00FF0000
#define Famask 0xFF000000

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask
#define IMAGE_DEPTH 32

#define NUM_VERTICES 4

struct Point Vertices[NUM_VERTICES] = {
  {100, 103},
  {402, 102},
  {404, 402},
  {106, 405}
};

SDL_atomic_t Die, VertexLock;

#define GRABLOCK(X)\
  while(SDL_AtomicCAS(&X, 0, 1)==SDL_FALSE) SDL_Delay(0);\
  assert(SDL_AtomicGet(&VertexLock)==1);

#define LOSELOCK(Y)\
          assert(SDL_AtomicGet(&Y)==1);\
          assert(SDL_AtomicCAS(&Y, 1, 0)==SDL_TRUE);

#define SETVERTEXMOUSE(Z)\
          assert(Z<NUM_VERTICES);\
          GRABLOCK(VertexLock);\
          SDL_GetMouseState(&(Vertices[Z].x), &(Vertices[Z].y));\
          LOSELOCK(VertexLock)

#define CASE_CHECKKEY_SETVERTEX(A)\
  case SDLK_##A:\
  case SDLK_KP_##A:\
  SETVERTEXMOUSE((A-1));\
  return 0;

int EventFilter(void * _unused, SDL_Event *event){
if(event->type==SDL_QUIT){
        SDL_AtomicSet(&Die, 1);
    }

    if(event->type==SDL_KEYDOWN){
        //printf("[SDL_GL] Info: Keydown Event.\n");
        int key = event->key.keysym.sym;
        switch(key){
        case SDLK_ESCAPE:
          SDL_AtomicSet(&Die, 1);
          return 0;

        CASE_CHECKKEY_SETVERTEX(1);
        CASE_CHECKKEY_SETVERTEX(2);
        CASE_CHECKKEY_SETVERTEX(3);
        CASE_CHECKKEY_SETVERTEX(4);
        }
    }
    return 1;
}

int main(int argc, char *argv[]){

    SDL_Window  *lWindow = NULL;
    SDL_Surface *lScreen = NULL;
    SDL_Surface *lTestImage = NULL;

    SDL_AtomicSet(&Die, 0);
    SDL_AtomicSet(&VertexLock, 0);

    if(SDL_Init(0x00))
      goto fail;

    if(SDL_VideoInit(NULL))
      goto fail_w;

    lWindow = SDL_CreateWindow("Transform BLit Demo",
                               SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                               800,600,0);
    if(!lWindow)
      goto fail_s;

    lScreen = SDL_GetWindowSurface(lWindow);
    if(!lScreen)
      goto fail_s;

    SDL_SetEventFilter(EventFilter, NULL);

    if(argc>1){
        lTestImage = SDL_LoadBMP(argv[1]);
        if(!lTestImage){
            fprintf(stderr, "[tfblit] Error: Could not load image %s (Must be"
                    "a 24-bit BMP image).\n"
                    "[tfblit] Error: %s\n", argv[1], SDL_GetError());
        }
    }
    if(!lTestImage){
        lTestImage = SDL_LoadBMP("test.bmp");
    }
    if(!lTestImage)
      goto fail;

    {

      SDL_Surface *lS = SDL_CreateRGBSurface(0, lTestImage->w, lTestImage->h,
                                             IMAGE_DEPTH, CHANNEL_MASKS);
      SDL_Rect lRect = {0, 0, lTestImage->w, lTestImage->h};
      SDL_LowerBlit(lTestImage, &lRect, lS, &lRect);

      SDL_FreeSurface(lTestImage);
      lTestImage = lS;

    }

    SDL_Surface *lBuffer = SDL_CreateRGBSurface(0, 800, 600, IMAGE_DEPTH,
                                                CHANNEL_MASKS);
    SDL_Rect lRect = {0, 0, 800, 600};
    while(SDL_AtomicGet(&Die)==0){
      int a = SDL_GetTicks();

      GRABLOCK(VertexLock);
      //SDL_BlitSurface(lTestImage, NULL, lBuffer, NULL);
      TransformBlit(lTestImage, lBuffer, Vertices);
      LOSELOCK(VertexLock);

      SDL_LowerBlit(lBuffer, &lRect, lScreen, &lRect);

      SDL_Delay(imin(imax(0, 60-(SDL_GetTicks()-a)), 60));
      SDL_UpdateWindowSurface(lWindow);
      SDL_PumpEvents();
    }


    fputs("[tfblit] Success.\n", stdout);
    SDL_FreeSurface(lTestImage);
    SDL_VideoQuit();
    SDL_Quit();
    return EXIT_SUCCESS;

fail:
fail_w:
    SDL_Quit();
fail_s:
    SDL_VideoQuit();
    fprintf(stderr, "[tfblit] Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;



}
