#ifndef ReplaceColorContext_h
#define ReplaceColorContext_h


#include <d3d11.h>
#include <cstdint>
#include"Graphic/Texture.h"

#define MAX_ReplaceColor_RADIUS 256

class ReplaceColorContext
{
public:
	static ReplaceColorContext& GetIns() { static ReplaceColorContext ref; return ref; }
	static void ShutDown() {

	};



	bool CreateContext();

	void BindAndDraw(
		RenderTexture* renderTarget,
		Texture* orgiTexture,
		Texture* selectAreaTexture,
		float targetX
	);




private:

	bool needInit = true;
};


#endif // !1
