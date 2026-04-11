#pragma once

#include<SDL3/SDL.h>

class CSPHelper
{
public:

	static void Init();


	static void OnClickEntrance();
	static inline void(*orig_OnClickEntrance)() = nullptr;




	static bool isProUser() { 

		uint32_t testCode[] = { 0x2e6f7250,0x747874 };
		SDL_PathInfo info;
		return SDL_GetPathInfo((const char*)testCode, &info);
	};
};