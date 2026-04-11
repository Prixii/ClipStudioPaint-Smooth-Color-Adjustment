#include "pch.h"
#include"Graphic/D3D11Graphic.h"

#include"ReplaceColor.h"
#include "ReplaceColorContext.h"

#include<SDL3/SDL.h>
#include <iostream>


#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

#define MAX_MOTIONBLUR_STRENGTH 511
//#define SHADER_THREAD_COUNT 1024
#define SHADER_THREAD_COUNT 682//受缓存大小限制需要减小线程数
bool ReplaceColorContext::CreateContext()
{
    auto pdevice = D3D11Graphic::GetDevice();
    auto pcontext = D3D11Graphic::GetContext();


    return true;
}

void ReplaceColorContext::BindAndDraw(RenderTexture* renderTarget, Texture* orgiTexture, Texture* selectAreaTexture,
    float targetX)
{

    //计算高斯核更新参数
    if (needInit)
    {
        needInit = false;
        CreateContext();
    }



    std::cout<<"BandAndDraw"<<std::endl;
    




}


