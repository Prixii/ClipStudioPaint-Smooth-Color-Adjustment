#pragma once
#ifndef ReplaceColor_H
#define ReplaceColor_H


#include "../Common/FilterPlugIn/TriglavPlugInSDK/TriglavPlugInType.h"
#include "../Common/FilterPlugIn/TriglavPlugInSDK/TriglavPlugInServer.h"
#include "../Common/FilterPlugIn/TriglavPlugInSDK/TriglavPlugInService.h"

#include "Graphic/Texture.h"

#include<SDL3/SDL.h>




class ReplaceColor
{
public:
	static ReplaceColor& GetIns() {
		if (!pthis)pthis = new ReplaceColor;
		return *pthis;
	}

	void Init(TriglavPlugInServer* runPluginServer);//运行时获取插件服务，和初始化获取的服务好像是不同的



	//FORM中处理？
	//void OnParamChanged(TriglavPlugInPoint point1,TriglavPlugInBool bPreview);

	void ShutDown();

	static void PluginModuleCleanUp();

	//UI参数
	void Render(int H,int S,int V, int strength, int tolerance, bool preview);
	void RenderSeleceArea();
	//void SetSkip(bool bSkip1Frame);//某些情况需要逻辑上跳过1帧


	enum PickType
	{
		PickType_Normal,
		PickType_Add,
		PickType_Sub,
	};
	void StartPick(PickType pickType);
	SDL_Color SetPickingColor(double canvasX,double canvasY);
	void SetPickingColor(SDL_Color* color);
	void PickSuccess();
	void PickCancel();



	const TriglavPlugInRect& GetSelectAreaRect() { return selectAreaRect; };

	//void OnCursorMoveInCanvas(double x, double y);

private:
	enum PickState
	{
		PickState_Picking,
		PickState_Success,
		PickState_Cancel,
	}pickState= PickState_Success;



	PickType pickType = PickType_Normal;
	bool isPicking = false;
	SDL_Color curPickingColor;
	int toleranceLast=-1;
	TriglavPlugInPtr srcBitmapData;






	static ReplaceColor* pthis;

	bool inited = false;
	bool needRenderMask = false;
	TriglavPlugInServer* runPluginServer;
	TriglavPlugInBitmapObject renderBitmap = NULL;
	TriglavPlugInBitmapObject sourceBitmap = NULL;
	TriglavPlugInRect selectAreaRect;


	Texture originTexture;
	Texture selectAreaTexture;


	RenderTexture maskTextureOrig;
	RenderTexture maskTextureActive;

	RenderTexture renderTexture;

};




#endif
