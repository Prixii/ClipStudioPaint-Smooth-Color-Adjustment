
#include"DUI/DuiCommon.h"
#include"ReplaceColor.h"
#include"ReplaceColorPlugin.h"
#include"ReplaceColorForm.h"
#include"CSPMOD.h"
#include<iostream>
#include <CspData.h>
#include <AddressTable.h>
struct	ReplaceColorFilterInfo
{
    TriglavPlugInPoint  point1;

    TriglavPlugInBool bPreview;

    TriglavPlugInPropertyService* pService1;
    TriglavPlugInPropertyService2* pPropertyService;

    TriglavPlugInRecordSuite* rsut;
    TriglavPlugInHostObject hobj;

};


static    const int kItemPoint1 = 1;
static    const int kItemPreview = 2;


static const char* uuidOfThisPlugin = "02A80AE9-8F36-4BC5-8280-167CDA2F58C4";



//48 8B C4
//48 89 58 08
//48 89 70 20
//55
//57
//41 56
//48 8D 68 A1
//48 81 EC 00 01 00 00
//0F 29 70 D8
//0F 29 78 C8
//48 8B 05 6F E7 6C 02
//48 33 C4
//48 89 45 17
//49 8B F0
//4C 8B F2
//48 8B F9
//48 89 55 9F
//F2 41 0F 10 30
//F2 41 0F 10 78 08
//48 8B 02
//48 89 44 24 38
//48 8B 4A 08
//48 89 4D 87
//BB 01 00 00 00
//48 85 C9


//这个函数数据可以保存直接用，因为只会得到焦点画布窗口中的信息
//可以使用函数调用时与指针实际的坐标差来计算画布窗口坐标
// pPosInCanvasWinodow
//HOOK以获取信息
static bool hookEnable = false;

static uintptr_t lastObject=0;
static double lastPosInCanvasWindowX;
static double lastPosInCanvasWindowY;
static double lastPosInBaseWindowX;
static double lastPosInBaseWindowY;
static POINT lastCursorPos;
static HWND curCanvasBaseWindow = 0;
static HWND curCanvasWindow = 0;


static int64_t(*orig_PosConvert)(uintptr_t pPosInWinodow, uintptr_t pObject, uintptr_t pPosInCanvasWinodow/*, uintptr_t uk*/) = NULL;
static int64_t Hook_PosConvert(uintptr_t pPosInWinodow, uintptr_t pObject, uintptr_t pPosInCanvasWinodow/*,  uintptr_t uk*/)
{





    if (!hookEnable) return orig_PosConvert(pPosInWinodow, pObject, pPosInCanvasWinodow/*,uk*/);









    lastObject = *(uintptr_t*)pObject;
    lastPosInCanvasWindowX = ((double*)pPosInCanvasWinodow)[0];
    lastPosInCanvasWindowY = ((double*)pPosInCanvasWinodow)[1];
    lastPosInBaseWindowX = ((double*)pPosInCanvasWinodow)[2];
    lastPosInBaseWindowY = ((double*)pPosInCanvasWinodow)[3];
    ::GetCursorPos(&lastCursorPos);
    

    //查询pObject对应的子窗口
    if (!curCanvasWindow)
    {
        HWND curParentWindow = ::WindowFromPoint(lastCursorPos);
        if (curParentWindow)
        {
            HWND curChildWindow = ::ChildWindowFromPoint(curParentWindow, {
                static_cast<int>(lastPosInBaseWindowX),static_cast<int>(lastPosInBaseWindowY) });

            if (curChildWindow != curParentWindow)
            {
                //使用坐标进行验证
                POINT childLT = {};
                POINT parentLT = {};
                ::ClientToScreen(curChildWindow,&childLT);
                ::ClientToScreen(curParentWindow,&parentLT);

                //RECT childRect = {};
                //RECT parentRect = {};
                //::GetWindowRect(curChildWindow, &childRect);
                //::GetWindowRect(curParentWindow, &parentRect);
                int ltoffX = static_cast<int>(lastPosInBaseWindowX - lastPosInCanvasWindowX);
                int ltoffY = static_cast<int>(lastPosInBaseWindowY - lastPosInCanvasWindowY);
                //测试发现客户区与工作区坐标一致, 怎么计算这里都会差1  手动补上
                if (childLT.x - parentLT.x == ltoffX-1
                    &&
                    childLT.y - parentLT.y == ltoffY-1
                    )
                {
                    curCanvasBaseWindow = curParentWindow;
                    curCanvasWindow = curChildWindow;
                    auto form = ReplaceColorForm::GetCurrentForm();
                    if (form)form->isDataFilled = true;

                }


            }
        }
    }








    



    return orig_PosConvert(pPosInWinodow, pObject, pPosInCanvasWinodow/*,uk*/);





    auto result = orig_PosConvert(pPosInWinodow, pObject, pPosInCanvasWinodow/*, uk*/);

    
    //ReplaceColorForm::GetCurrentForm()->OnCursorMoveInCanvas(pPointInWindow[0], pPointInWindow[1]);



    return result;


}


bool ReplaceColorForm::GetCurrentPosInCanvas(double* x, double* y)
{
    if (!isDataFilled)return false;


    POINT cursorPos={};
    ::GetCursorPos(&cursorPos);
    double posInCanvasWinodow[10] = { 
        cursorPos.x- lastCursorPos.x+ lastPosInCanvasWindowX,
        cursorPos.y- lastCursorPos.y+ lastPosInCanvasWindowY,
        0,
        0,
    };

    double outResult[8] = {};
    uintptr_t _lastObject[2] = { lastObject ,0 };
    orig_PosConvert((uintptr_t)outResult, (uintptr_t)_lastObject, (uintptr_t)posInCanvasWinodow);
    *x = outResult[0];
    *y = outResult[1];
    return true;

}


bool ReplaceColorForm::IsMouseOnPaintWindow()
{
    if (!curCanvasWindow)return false;
    POINT currenCursorPos = {};
    ::GetCursorPos(&currenCursorPos);
    if(curCanvasBaseWindow!= ::WindowFromPoint(currenCursorPos))return false;

    ::ScreenToClient(curCanvasBaseWindow ,&currenCursorPos);
    if (curCanvasWindow != ::ChildWindowFromPoint(curCanvasBaseWindow, currenCursorPos))
    {
        return false;
    }
    return true;
}

























static void InstallPosConvertHook()
{
    static bool isHooked = false;
    if (isHooked)return;
    auto pWindowPosToCanvasPosFunc= AddressTable::GetAddress("CSPCustomFilter_WindowPosToCanvasPos_Func");
    if(pWindowPosToCanvasPosFunc)
        CSPMOD::Hook(pWindowPosToCanvasPosFunc, Hook_PosConvert, (void**)&orig_PosConvert,CSPMOD::_HOOKREG_R11);
    isHooked = true;

};












static void  ReplaceColor_TriglavPlugInFilterPropertyCallBack(TriglavPlugInInt* result, TriglavPlugInPropertyObject propertyObject, const TriglavPlugInInt itemKey, const TriglavPlugInInt notify, TriglavPlugInPtr data)
{
    //使用自建UI窗口，不需要编写回调，但不设置回调函数会导致崩溃
    (*result) = kTriglavPlugInPropertyCallBackResultNoModify;
    return;

}










void  ReplaceColor_TriglavPluginCall(TriglavPlugInInt* result, TriglavPlugInPtr* data, TriglavPlugInInt selector, TriglavPlugInServer* pluginServer, TriglavPlugInPtr reserved)

{
    *result = kTriglavPlugInCallResultFailed;
    if (pluginServer != NULL)
    {
        if (selector == kTriglavPlugInSelectorModuleInitialize)
        {


            InstallPosConvertHook();


            //告知CSPMOD.DLL此插件已加载
            //auto handle = GetModuleHandle(L"CSPMOD.dll");
            //if (handle)
            //{
            //    typedef void(*SetStrDataFunc)(const char* key, const char* value);

            //    SetStrDataFunc myFunc = (SetStrDataFunc)GetProcAddress(handle, "CSPMOD_SetStrData");
            //    if (myFunc)
            //    {
            //        myFunc("SmoothFilter_ReplaceColor", uuidOfThisPlugin);
            //    }
            //}





            //模块初始化
            //	プラグインの初期化
            TriglavPlugInModuleInitializeRecord* pModuleInitializeRecord = (*pluginServer).recordSuite.moduleInitializeRecord;
            TriglavPlugInStringService* pStringService = (*pluginServer).serviceSuite.stringService;
            if (pModuleInitializeRecord != NULL && pStringService != NULL)
            {

                TriglavPlugInInt	hostVersion;
                (*pModuleInitializeRecord).getHostVersionProc(&hostVersion, (*pluginServer).hostObject);

                if (hostVersion >= kTriglavPlugInNeedHostVersion)
                {
                    //如果csp版本受支持
                    TriglavPlugInStringObject	moduleID = NULL;
                    const char* moduleIDString = uuidOfThisPlugin;//防止重复加载的uuid

                    //调用后在csp中创建模块实例并返回模块id
                    (*pStringService).createWithAsciiStringProc(&moduleID, moduleIDString, static_cast<TriglavPlugInInt>(::strlen(moduleIDString)));

                    (*pModuleInitializeRecord).setModuleIDProc((*pluginServer).hostObject, moduleID);
                    (*pModuleInitializeRecord).setModuleKindProc((*pluginServer).hostObject, kTriglavPlugInModuleSwitchKindFilter);//设置模块种类，得看看文档

                    //为什么这里有release
                    (*pStringService).releaseProc(moduleID);

                    ReplaceColorFilterInfo* pFilterInfo = new ReplaceColorFilterInfo;
                    *data = pFilterInfo;
                    *result = kTriglavPlugInCallResultSuccess;
                }
            }
        }
        else if (selector == kTriglavPlugInSelectorModuleTerminate)
        {
            //模块终止时的行为
            //	プラグインの終了処理
            ReplaceColorFilterInfo* pFilterInfo = static_cast<ReplaceColorFilterInfo*>(*data);
            delete pFilterInfo;
            *data = NULL;
            *result = kTriglavPlugInCallResultSuccess;

            //似乎让动态库卸载时自动释放com指针会出现死锁的问题.
            ReplaceColor::PluginModuleCleanUp();


        }
        else if (selector == kTriglavPlugInSelectorFilterInitialize)
        {




            //	フィルタの初期化
            //滤镜初始化

            TriglavPlugInRecordSuite* pRecordSuite = &(*pluginServer).recordSuite;
            TriglavPlugInHostObject					hostObject = (*pluginServer).hostObject;
            TriglavPlugInStringService* pStringService = (*pluginServer).serviceSuite.stringService;
            TriglavPlugInPropertyService* pPropertyService = (*pluginServer).serviceSuite.propertyService;

            //接口不能为空
            if (TriglavPlugInGetFilterInitializeRecord(pRecordSuite) != NULL && pStringService != NULL && pPropertyService != NULL)
            {
                //	フィルタカテゴリ名とフィルタ名の設定
                //滤镜组名和滤镜名
                TriglavPlugInStringObject	filterCategoryName = NULL;
                TriglavPlugInStringObject	filterName = NULL;

                std::wstring str=ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_FILTERPLUGIN_CATEGORYNAME");
                (*pStringService).createWithUnicodeStringProc(&filterCategoryName, (unsigned short*)str.data(),static_cast<TriglavPlugInInt> (str.size()));


                str=ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_FILTERPLUGIN_REPLACECOLOR");
                (*pStringService).createWithUnicodeStringProc(&filterName, (unsigned short*)str.data(), static_cast<TriglavPlugInInt> (str.size()));




                TriglavPlugInFilterInitializeSetFilterCategoryName(pRecordSuite, hostObject, filterCategoryName, 'c');
                TriglavPlugInFilterInitializeSetFilterName(pRecordSuite, hostObject, filterName, 'r');
                (*pStringService).releaseProc(filterCategoryName);
                (*pStringService).releaseProc(filterName);

                //	プレビュー
                //preview。 煞笔片假名
                //给插件设置是否可以预览
                //禁用CSP预览能有效提高流畅性，太神秘了
                TriglavPlugInFilterInitializeSetCanPreview(pRecordSuite, hostObject, false);




                //	ターゲット 目标
                //看起来可以设置很多目标，这里只设置了彩色位图的rgba;
                //源码中看起来可用的只能rgba了。看不懂这里设置灰度图之类的意义
                TriglavPlugInInt	target[] = { kTriglavPlugInFilterTargetKindRasterLayerRGBAlpha };
                TriglavPlugInFilterInitializeSetTargetKinds(pRecordSuite, hostObject, target, 1);





                //	プロパティの作成
                //生成可以控制的属性 Property    煞笔片假名
                TriglavPlugInPropertyObject	propertyObject;
                (*pPropertyService).createProc(&propertyObject);



                TriglavPlugInPropertyService2* pService2 = (*pluginServer).serviceSuite.propertyService2;
                //    测试属性
                TriglavPlugInStringObject    strPreview = NULL;



                str = ui::GlobalManager::Instance().Lang().GetStringViaID(L"STRID_FILTERPLUGIN_PREVIEW");
                (*pStringService).createWithUnicodeStringProc(&strPreview, (unsigned short*)str.data(), static_cast<TriglavPlugInInt> (str.size()));





                //虽然自绘窗口已经不需要按键了，但是为了防止预期外的结果，这里仍然创建一个按钮来吸引焦点
                //（比如按到回车导致被隐藏的原始滤镜对话框关闭了而自绘对话框未关闭，导致出现崩溃）

                //预览
                (*pPropertyService).addItemProc(propertyObject, kItemPreview, kTriglavPlugInPropertyValueTypeBoolean, kTriglavPlugInPropertyValueKindDefault, kTriglavPlugInPropertyInputKindDefault, strPreview, 'p');
                (*pPropertyService).setBooleanValueProc(propertyObject, kItemPreview, true);
                (*pPropertyService).setBooleanDefaultValueProc(propertyObject, kItemPreview, true);

                (*pStringService).releaseProc(strPreview);





                //	プロパティの設定
                //property的设置
               TriglavPlugInFilterInitializeSetProperty(pRecordSuite, hostObject, propertyObject);
                TriglavPlugInFilterInitializeSetPropertyCallBack(pRecordSuite, hostObject, ReplaceColor_TriglavPlugInFilterPropertyCallBack, *data);

                //	プロパティの破棄
                //释放propertyobj 应该是减少引用计数吧。。这东西应该不会这么快就从内存里移除
                (*pPropertyService).releaseProc(propertyObject);

                *result = kTriglavPlugInCallResultSuccess;
            }
        }
        else if (selector == kTriglavPlugInSelectorFilterTerminate)
        {
            //	フィルタの終了処理
            //滤镜终止时的处理
            *result = kTriglavPlugInCallResultSuccess;
        }
        else if (selector == kTriglavPlugInSelectorFilterRun)
        {


            hookEnable = true;







            //	フィルタの実行
            //滤镜的执行

            auto pWorkingRecordSuite = &(*pluginServer).recordSuite;
            auto pWorkingOffscreenService = (*pluginServer).serviceSuite.offscreenService;
            auto pWorkingPropertyService = (*pluginServer).serviceSuite.propertyService;
            auto workingHostObject = (*pluginServer).hostObject;


            auto& customFilter = ReplaceColor::GetIns();


            if (TriglavPlugInGetFilterRunRecord(pWorkingRecordSuite) != NULL && pWorkingOffscreenService != NULL && pWorkingPropertyService != NULL)
            {
                customFilter.Init(pluginServer);




                //设置进度
                //还需要设置进度，ai生图是吧


                //初始化
                ReplaceColorFilterInfo* pFilterInfo = static_cast<ReplaceColorFilterInfo*>(*data);
                (*pFilterInfo).pService1 = (*pluginServer).serviceSuite.propertyService;
                (*pFilterInfo).pPropertyService = (*pluginServer).serviceSuite.propertyService2;
                //(*pluginServer).serviceSuite.
                (*pFilterInfo).rsut = &(*pluginServer).recordSuite;
                (*pFilterInfo).hobj = (*pluginServer).hostObject;
                pFilterInfo->bPreview = true;


                pFilterInfo->point1 = { 0,0 };
                //                (*pFilterInfo).blurRadius			= 6;
                //                customFilter.Render(pFilterInfo->blurRadius);
                //                customFilter.Render(200);
                                //？？restart干嘛 复位？
                bool restart = true;
                bool firstRun = true;

                std::cout << "loop entrypoint" << std::endl;



                ReplaceColorForm::ShowForm();


                while (true)
                {

                    //正常处理消息即可，画面绘制在参数改变的回调里，会比较流畅。。


                    //                    customFilter.SetSkip(false);
                    //                    customFilter.Render(pFilterInfo->point1.x-pFilterInfo->point2.x,pFilterInfo->point1.y-pFilterInfo->point2.y);
                    //customFilter.SetSkip(true);
                    //处理的状态
                    TriglavPlugInInt	processResult;
                    TriglavPlugInFilterRunProcess(pWorkingRecordSuite, &processResult, workingHostObject, kTriglavPlugInFilterRunProcessStateEnd);
                    //TriglavPlugInFilterRunProcess(pWorkingRecordSuite, &processResult, workingHostObject, kTriglavPlugInFilterRunProcessStateContinue);


                    if (processResult == kTriglavPlugInFilterRunProcessResultRestart)
                    {
                        restart = true;
                    }
                    else if (processResult == kTriglavPlugInFilterRunProcessResultExit)
                    {
                        break;
                    }
                }
                *result = kTriglavPlugInCallResultSuccess;



            }
            hookEnable = false;
            curCanvasWindow = 0; curCanvasBaseWindow = 0;
            customFilter.ShutDown();



        }
    }
    return;
}