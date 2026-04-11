
#include<iostream>
#include"ReplaceColor.h"
#include"ReplaceColorContext.h"
#include"Graphic/D3D11Graphic.h"
#include<vector>
#include<chrono>


ReplaceColor* ReplaceColor::pthis = NULL;
void ReplaceColor::Init(TriglavPlugInServer* _runPluginServer)
{
    if (NULL == _runPluginServer)return;

    runPluginServer = _runPluginServer;

    //创建gpu资源

      //获取源离屏对象
    TriglavPlugInRecordSuite* pRecordSuite = &(*runPluginServer).recordSuite;
    TriglavPlugInOffscreenService* pOffscreenService = (*runPluginServer).serviceSuite.offscreenService;
    //    pWorkingPropertyService    = (*pluginServer).serviceSuite.propertyService;
    TriglavPlugInBitmapService* pBitmapService = (*runPluginServer).serviceSuite.bitmapService;




    TriglavPlugInHostObject hostObject = (*runPluginServer).hostObject;






    if (TriglavPlugInGetFilterRunRecord(pRecordSuite) == NULL || pOffscreenService == NULL /*&& pWorkingPropertyService != NULL */)
        return;


    //获取到源离屏对象
    TriglavPlugInOffscreenObject sourceOffscreen, selectAreaOffscreen;
    TriglavPlugInFilterRunGetSourceOffscreen(pRecordSuite, &sourceOffscreen, hostObject);

    //先确认是否有选区
    TriglavPlugInFilterRunGetSelectAreaOffscreen(pRecordSuite, &selectAreaOffscreen, hostObject);
    TriglavPlugInRect targetRect;
    if (selectAreaOffscreen == NULL)
    {
        pOffscreenService->getRectProc(&targetRect, sourceOffscreen);
    }
    else
    {
        TriglavPlugInFilterRunGetSelectAreaRect(pRecordSuite, &targetRect, hostObject);
    }


    //根据选区创建矩形
    //TriglavPlugInRect selectAreaRect;
    if (selectAreaOffscreen == NULL)
    {
        selectAreaRect.left = 0;
        selectAreaRect.top = 0;
        TriglavPlugInFilterRunGetCanvasWidth(pRecordSuite, &selectAreaRect.right, hostObject);
        TriglavPlugInFilterRunGetCanvasHeight(pRecordSuite, &selectAreaRect.bottom, hostObject);
    }
    else
    {
        TriglavPlugInFilterRunGetSelectAreaRect(pRecordSuite, &selectAreaRect, hostObject);
    }



    //构建图像
//从原始图像获取位图

    sourceBitmap = NULL;
    pBitmapService->createProc(&sourceBitmap, selectAreaRect.right - selectAreaRect.left, selectAreaRect.bottom - selectAreaRect.top, 4, kTriglavPlugInBitmapScanlineHorizontalLeftTop);
    TriglavPlugInPoint pointZero;
    pointZero.x = 0;
    pointZero.y = 0;
    TriglavPlugInPoint bitmapLeftTop;
    bitmapLeftTop.x = selectAreaRect.left;
    bitmapLeftTop.y = selectAreaRect.top;

    pOffscreenService->getBitmapProc(sourceBitmap, &pointZero, sourceOffscreen, &bitmapLeftTop,
        selectAreaRect.right - selectAreaRect.left,
        selectAreaRect.bottom - selectAreaRect.top,
        kTriglavPlugInOffscreenCopyModeNormal
    );





    //创建纹理
    srcBitmapData;
    pBitmapService->getAddressProc(&srcBitmapData, sourceBitmap, &pointZero);
    TriglavPlugInInt depth = 0;
    pBitmapService->getDepthProc(&depth, sourceBitmap);
    originTexture.Resize(selectAreaRect.right - selectAreaRect.left, selectAreaRect.bottom - selectAreaRect.top, srcBitmapData);


    //maskTextureOrig.Resize(originTexture.GetSizeW(), originTexture.GetSizeH());
    //maskTextureActive.Resize(originTexture.GetSizeW(), originTexture.GetSizeH());

    maskTextureOrig.Resize(originTexture.GetSizeW(), originTexture.GetSizeH());
    maskTextureActive.Resize(originTexture.GetSizeW(), originTexture.GetSizeH());

    renderTexture.Resize(originTexture.GetSizeW(), originTexture.GetSizeH());



    //选区
    if (selectAreaOffscreen != NULL)
    {

        TriglavPlugInBitmapObject selectAreaBitmap = NULL;
        pBitmapService->createProc(&selectAreaBitmap, selectAreaRect.right - selectAreaRect.left, selectAreaRect.bottom - selectAreaRect.top, 4, kTriglavPlugInBitmapScanlineHorizontalLeftTop);


        pOffscreenService->getBitmapProc(selectAreaBitmap, &pointZero, selectAreaOffscreen, &bitmapLeftTop,
            selectAreaRect.right - selectAreaRect.left,
            selectAreaRect.bottom - selectAreaRect.top,
            //                                         kTriglavPlugInOffscreenCopyModeAlpha
            kTriglavPlugInOffscreenCopyModeNormal
        );
        TriglavPlugInPtr selectAreaBitmapData;
        pBitmapService->getAddressProc(&selectAreaBitmapData, selectAreaBitmap, &pointZero);
        TriglavPlugInInt rowBytes;
        pBitmapService->getRowBytesProc(&rowBytes, selectAreaBitmap);
        TriglavPlugInInt depth, colorBytes;
        pBitmapService->getDepthProc(&depth, selectAreaBitmap);
        pBitmapService->getPixelBytesProc(&colorBytes, selectAreaBitmap);

        selectAreaTexture.Resize(originTexture.GetSizeW(), originTexture.GetSizeH(), selectAreaBitmapData, rowBytes);

        pBitmapService->releaseProc(selectAreaBitmap);
    }
    else
    {
        //        selectAreaTexture.Resize(4,1);//纯白
        //        uint32_t white=0xffffffff;
        //        selectAreaTexture.UpdateData(0, 0, 4, 1, &white);
                //位图模式下下方数据是1像素大小而不是4像素大小

        uint32_t white = 0xffffffff;
        selectAreaTexture.Resize(1, 1, &white);//纯白
    }



    //图像已经加载进gpu纹理了，释放原始纹理
    // ReplaceColor需要原始图像进行取色，不释放
    //pBitmapService->releaseProc(sourceBitmap);


    //创建渲染目标位图
    runPluginServer->serviceSuite.bitmapService->createProc(&renderBitmap, originTexture.GetSizeW(), originTexture.GetSizeH(), 4, kTriglavPlugInBitmapScanlineHorizontalLeftTop);












    //初始化参数
    pickType = PickType_Normal;
    isPicking = false;




    inited = true;
}


void ReplaceColor::ShutDown()
{
    inited = false;
    if (runPluginServer && renderBitmap)
    {
        runPluginServer->serviceSuite.bitmapService->releaseProc(renderBitmap);
    }
    if (runPluginServer && sourceBitmap)
    {
        runPluginServer->serviceSuite.bitmapService->releaseProc(sourceBitmap);
    }
    renderBitmap = NULL;
    sourceBitmap = NULL;
    runPluginServer = NULL;

    if (pthis)delete pthis;
    pthis = NULL;
}



void ReplaceColor::PluginModuleCleanUp()
{
    //D3D11Graphic::ShutDown();
    ReplaceColorContext::ShutDown();



}

//void ReplaceColor::OnParamChanged(TriglavPlugInPoint point1, TriglavPlugInBool bPreview)
//{
//    //检查预览
//
//    //参数改变时，进行绘制
//    //if (!renderBitmap)
//    //{
//    //    if (runPluginServer)
//    //        runPluginServer->serviceSuite.bitmapService->createProc(&renderBitmap, originTexture.GetSizeW(), originTexture.GetSizeH(), 4, kTriglavPlugInBitmapScanlineHorizontalLeftTop);
//
//    //    if (!renderBitmap)
//    //        return;
//    //}
//
//
//    auto start = std::chrono::system_clock::now();
//    Render(static_cast<float>(point1.x), bPreview);
//    auto end = std::chrono::system_clock::now();
//
//    lastCostTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//}


//
//void ReplaceColor::SetSkip(bool bSkip1Frame)
//{
//    needSkip = bSkip1Frame;
//}


//void ReplaceColor::OnCursorMoveInCanvas(double x, double y)
//{
//    std::cout << "OnCursorMoveInCanvas:" << x << "," << y << std::endl;
//}


void ReplaceColor::Render(int H, int S, int V, int strength, int tolerance, bool preview)
{
    //输入的参数为UI风格，需要转为具体浮点数
    
    if (H < 0)H += 360;
    float fH = H / 60.f;//H -180~180转换为0~6

    float fS=S/100.f;
    float fV=V/100.f;

    if (!preview)
    {
        fH = 0;
        fS = 0;
        fV = 0;
    }


    float fStrength = strength / 100.f;


    float fTolerance = tolerance / 200.f;

    if (toleranceLast != tolerance)
    {
        toleranceLast = tolerance;
        needRenderMask = true;
    }



    //    blurRadius=200;

    if (!renderBitmap)
    {
        if (runPluginServer)
            runPluginServer->serviceSuite.bitmapService->createProc(&renderBitmap, originTexture.GetSizeW(), originTexture.GetSizeH(), 4, kTriglavPlugInBitmapScanlineHorizontalLeftTop);

        if (!renderBitmap)
            return;
    }



    if (!preview)
    {
        fH = 0;
        fS = 0;
        fV = 0;
        fStrength = 0;
    }


    uint8_t* data = NULL;
    TriglavPlugInPoint zero;
    zero.x = 0;
    zero.y = 0;
    runPluginServer->serviceSuite.bitmapService->getAddressProc((void**)&data, renderBitmap, &zero);
    if (!data)
        return;


    TriglavPlugInInt rowBytesCount;
    runPluginServer->serviceSuite.bitmapService->getRowBytesProc(&rowBytesCount, renderBitmap);










    if (needRenderMask)
    {

        if (pickState == PickState_Cancel)
        {
            maskTextureOrig.CopyToTexture(maskTextureActive);
        }
        else
        {


            ReplaceColorContext::GetIns().GenerateMaskTexture(&maskTextureActive, &originTexture, &maskTextureOrig,
                curPickingColor.r / 255.f, curPickingColor.g / 255.f, curPickingColor.b / 255.f, fTolerance, pickType);
            needRenderMask = false;


            if (pickState == PickState_Success)
            {
                maskTextureActive.CopyToTexture(maskTextureOrig);
            }
        }
    }





    ReplaceColorContext::GetIns().BindAndDraw(&renderTexture, &originTexture,&selectAreaTexture, &maskTextureActive, fH, fS, fV, fStrength);




    renderTexture.CopyToMem(data, true, rowBytesCount);

    //应用位图
//获取目标离屏纹理
    TriglavPlugInRecordSuite* pRecordSuite = &(*runPluginServer).recordSuite;
    TriglavPlugInOffscreenService* pOffscreenService = (*runPluginServer).serviceSuite.offscreenService;
    TriglavPlugInHostObject hostObject = (*runPluginServer).hostObject;
    TriglavPlugInOffscreenObject destOffscreen;
    TriglavPlugInFilterRunGetDestinationOffscreen(pRecordSuite, &destOffscreen, hostObject);

    TriglavPlugInPoint targetPos;

    targetPos.x = selectAreaRect.left;
    targetPos.y = selectAreaRect.top;

    runPluginServer->serviceSuite.offscreenService->setBitmapProc
    (destOffscreen, &targetPos, renderBitmap, &zero, originTexture.GetSizeW(), originTexture.GetSizeH(), kTriglavPlugInOffscreenCopyModeNormal);

    TriglavPlugInFilterRunUpdateDestinationOffscreenRect(pRecordSuite, hostObject, &selectAreaRect);

}

void ReplaceColor::RenderSeleceArea()
{
    ReplaceColorContext::GetIns().RenderSelectAreaPreviewTexture(&originTexture,&selectAreaTexture,&maskTextureActive);


}







void ReplaceColor::StartPick(PickType _pickType)
{
    pickType = _pickType;
    isPicking = true;
    pickState = PickState_Picking;

}

SDL_Color ReplaceColor::SetPickingColor(double canvasX, double canvasY)
{

    //从画布取色

    selectAreaRect;

    int colorPosX = static_cast<int>(canvasX) - selectAreaRect.left;
    int colorPosY = static_cast<int>(canvasY) - selectAreaRect.top;


    if (!srcBitmapData || colorPosX >= selectAreaRect.right - selectAreaRect.left|| colorPosX< selectAreaRect.left
        || colorPosY >= selectAreaRect.bottom - selectAreaRect.top|| colorPosY< selectAreaRect.top)
    {
        //超出范围外 取色失败
        return curPickingColor;
    }


    //pBitmapService->getAddressProc(&bitmapData, sourceBitmap, &pointZero);


    uint8_t* pColor=(uint8_t*) srcBitmapData;


    pColor += (colorPosY * (selectAreaRect.right - selectAreaRect.left) + colorPosX) * 4;

    //BGRA
    curPickingColor.r = pColor[2];
    curPickingColor.g = pColor[1];
    curPickingColor.b = pColor[0];

    needRenderMask = true;



    //预览颜色变化




    return curPickingColor;
}

void ReplaceColor::SetPickingColor(SDL_Color* color)
{
    curPickingColor = *color;


    needRenderMask = true;
}

void ReplaceColor::PickSuccess()
{
    //应用颜色变化
    std::cout << "PickSuccess" << std::endl;
    pickState = PickState_Success;
    needRenderMask = true;
    //maskTextureActive.CopyToTexture(maskTextureOrig);
}

void ReplaceColor::PickCancel()
{
    //取消颜色变化
    std::cout << "PickCanceled" << std::endl;
    pickState = PickState_Cancel;
    needRenderMask = true;

    //maskTextureOrig.CopyToTexture(maskTextureActive);

}
