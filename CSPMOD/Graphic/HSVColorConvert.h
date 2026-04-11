
#ifndef HSVCOLORCONVERT_h
#define HSVCOLORCONVERT_h



//为了保证一致性,需要使用CSP的HSV算法,不能用其他库提供的HSV算法
//#include <SDL3/SDL_pixels.h>


namespace colorUtil
{


	extern void HSVFilter(struct SDL_Color* colorInOut,int paramH,int paramS,int paramV,int paramStrength);
















}




















#endif