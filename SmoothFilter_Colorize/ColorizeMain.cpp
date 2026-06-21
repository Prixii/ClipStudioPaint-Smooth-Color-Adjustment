#include "pch.h"
#include "TriglavPlugInSDK/TriglavPlugInSDK.h"
#include "ColorizeFilter.h"
#include <vector>
#include <libloaderapi.h>

typedef unsigned char BYTE;

static const int kItemKeyColorize    = 1;
static const int kItemKeyHue         = 2;
static const int kItemKeySaturation  = 3;
static const int kItemKeyLightness   = 4;

static const int kStringIDFilterCategoryName = 101;
static const int kStringIDFilterName         = 102;
static const int kStringIDItemColorize       = 103;
static const int kStringIDItemHue            = 104;
static const int kStringIDItemSaturation     = 105;
static const int kStringIDItemLightness      = 106;

static const char* uuidOfThisPlugin = "B3C5D7E9-1A2B-4C3D-5E6F-7A8B9C0D1E2F";

struct ColorizeFilterInfo
{
    bool colorize;
    int  hue;
    int  saturation;
    int  lightness;

    TriglavPlugInPropertyService* pPropertyService;
};

// 属性回调
static void TRIGLAV_PLUGIN_CALLBACK TriglavPlugInFilterPropertyCallBack(
    TriglavPlugInInt* result,
    TriglavPlugInPropertyObject propertyObject,
    const TriglavPlugInInt itemKey,
    const TriglavPlugInInt notify,
    TriglavPlugInPtr data)
{
    (*result) = kTriglavPlugInPropertyCallBackResultNoModify;

    ColorizeFilterInfo* pFilterInfo = static_cast<ColorizeFilterInfo*>(data);
    if (pFilterInfo == NULL) return;

    TriglavPlugInPropertyService* pPropertyService = (*pFilterInfo).pPropertyService;
    if (pPropertyService == NULL) return;

    if (notify == kTriglavPlugInPropertyCallBackNotifyValueChanged)
    {
        if (itemKey == kItemKeyColorize)
        {
            TriglavPlugInBool value;
            (*pPropertyService).getBooleanValueProc(&value, propertyObject, itemKey);
            bool bv = (value != kTriglavPlugInBoolFalse);
            if ((*pFilterInfo).colorize != bv)
            {
                (*pFilterInfo).colorize = bv;
                (*result) = kTriglavPlugInPropertyCallBackResultModify;
            }
        }
        else if (itemKey == kItemKeyHue)
        {
            TriglavPlugInInt value;
            (*pPropertyService).getIntegerValueProc(&value, propertyObject, itemKey);
            if ((*pFilterInfo).hue != value)
            {
                (*pFilterInfo).hue = value;
                (*result) = kTriglavPlugInPropertyCallBackResultModify;
            }
        }
        else if (itemKey == kItemKeySaturation)
        {
            TriglavPlugInInt value;
            (*pPropertyService).getIntegerValueProc(&value, propertyObject, itemKey);
            if ((*pFilterInfo).saturation != value)
            {
                (*pFilterInfo).saturation = value;
                (*result) = kTriglavPlugInPropertyCallBackResultModify;
            }
        }
        else if (itemKey == kItemKeyLightness)
        {
            TriglavPlugInInt value;
            (*pPropertyService).getIntegerValueProc(&value, propertyObject, itemKey);
            if ((*pFilterInfo).lightness != value)
            {
                (*pFilterInfo).lightness = value;
                (*result) = kTriglavPlugInPropertyCallBackResultModify;
            }
        }
    }
}

// 插件主入口
void TRIGLAV_PLUGIN_API TriglavPluginCall(
    TriglavPlugInInt* result,
    TriglavPlugInPtr* data,
    TriglavPlugInInt selector,
    TriglavPlugInServer* pluginServer,
    TriglavPlugInPtr reserved)
{
    *result = kTriglavPlugInCallResultFailed;
    if (pluginServer == NULL) return;

    if (selector == kTriglavPlugInSelectorModuleInitialize)
    {
        // 告知 CSPMOD.dll 此插件已加载
        auto handle = GetModuleHandle(L"CSPMOD.dll");
        if (handle)
        {
            typedef void(*SetStrDataFunc)(const char* key, const char* value);
            SetStrDataFunc myFunc = (SetStrDataFunc)GetProcAddress(handle, "CSPMOD_SetStrData");
            if (myFunc)
            {
                myFunc("SmoothFilter_Colorize", uuidOfThisPlugin);
            }
        }

        // 模块初始化
        TriglavPlugInModuleInitializeRecord* pModuleInitializeRecord = (*pluginServer).recordSuite.moduleInitializeRecord;
        TriglavPlugInStringService* pStringService = (*pluginServer).serviceSuite.stringService;
        if (pModuleInitializeRecord != NULL && pStringService != NULL)
        {
            TriglavPlugInInt hostVersion;
            (*pModuleInitializeRecord).getHostVersionProc(&hostVersion, (*pluginServer).hostObject);
            if (hostVersion >= kTriglavPlugInNeedHostVersion)
            {
                TriglavPlugInStringObject moduleID = NULL;
                const char* moduleIDString = uuidOfThisPlugin;
                (*pStringService).createWithAsciiStringProc(&moduleID, moduleIDString,
                    static_cast<TriglavPlugInInt>(::strlen(moduleIDString)));
                (*pModuleInitializeRecord).setModuleIDProc((*pluginServer).hostObject, moduleID);
                (*pModuleInitializeRecord).setModuleKindProc((*pluginServer).hostObject, kTriglavPlugInModuleSwitchKindFilter);
                (*pStringService).releaseProc(moduleID);

                ColorizeFilterInfo* pFilterInfo = new ColorizeFilterInfo;
                *data = pFilterInfo;
                *result = kTriglavPlugInCallResultSuccess;
            }
        }
    }
    else if (selector == kTriglavPlugInSelectorModuleTerminate)
    {
        ColorizeFilterInfo* pFilterInfo = static_cast<ColorizeFilterInfo*>(*data);
        delete pFilterInfo;
        *data = NULL;
        *result = kTriglavPlugInCallResultSuccess;
    }
    else if (selector == kTriglavPlugInSelectorFilterInitialize)
    {
        TriglavPlugInRecordSuite* pRecordSuite = &(*pluginServer).recordSuite;
        TriglavPlugInHostObject hostObject = (*pluginServer).hostObject;
        TriglavPlugInStringService* pStringService = (*pluginServer).serviceSuite.stringService;
        TriglavPlugInPropertyService* pPropertyService = (*pluginServer).serviceSuite.propertyService;
        if (TriglavPlugInGetFilterInitializeRecord(pRecordSuite) != NULL && pStringService != NULL && pPropertyService != NULL)
        {
            // 滤镜分类名与滤镜名
            TriglavPlugInStringObject filterCategoryName = NULL;
            TriglavPlugInStringObject filterName = NULL;
            (*pStringService).createWithAsciiStringProc(&filterCategoryName, "Smooth Colorize", static_cast<TriglavPlugInInt>(::strlen("Smooth Colorize")));
            (*pStringService).createWithAsciiStringProc(&filterName, "Colorize", static_cast<TriglavPlugInInt>(::strlen("Colorize")));

            TriglavPlugInFilterInitializeSetFilterCategoryName(pRecordSuite, hostObject, filterCategoryName, 'c');
            TriglavPlugInFilterInitializeSetFilterName(pRecordSuite, hostObject, filterName, 'h');
            (*pStringService).releaseProc(filterCategoryName);
            (*pStringService).releaseProc(filterName);

            // 预览
            TriglavPlugInFilterInitializeSetCanPreview(pRecordSuite, hostObject, true);

            // 目标图层类型
            TriglavPlugInInt target[] = {
                kTriglavPlugInFilterTargetKindRasterLayerRGBAlpha,
                kTriglavPlugInFilterTargetKindRasterLayerGrayAlpha
            };
            TriglavPlugInFilterInitializeSetTargetKinds(pRecordSuite, hostObject, target, 2);

            // 属性创建
            TriglavPlugInPropertyObject propertyObject;
            (*pPropertyService).createProc(&propertyObject);

            // 着色复选框
            TriglavPlugInStringObject colorizeCaption = NULL;
            (*pStringService).createWithAsciiStringProc(&colorizeCaption, "Colorize", static_cast<TriglavPlugInInt>(::strlen("Colorize")));
            (*pPropertyService).addItemProc(propertyObject, kItemKeyColorize,
                kTriglavPlugInPropertyValueTypeBoolean,
                kTriglavPlugInPropertyValueKindDefault,
                kTriglavPlugInPropertyInputKindDefault,
                colorizeCaption, 'z');
            (*pPropertyService).setBooleanValueProc(propertyObject, kItemKeyColorize, kTriglavPlugInBoolFalse);
            (*pPropertyService).setBooleanDefaultValueProc(propertyObject, kItemKeyColorize, kTriglavPlugInBoolFalse);
            (*pStringService).releaseProc(colorizeCaption);

            // 色相
            TriglavPlugInStringObject hueCaption = NULL;
            (*pStringService).createWithAsciiStringProc(&hueCaption, "Hue", static_cast<TriglavPlugInInt>(::strlen("Hue")));
            (*pPropertyService).addItemProc(propertyObject, kItemKeyHue,
                kTriglavPlugInPropertyValueTypeInteger,
                kTriglavPlugInPropertyValueKindDefault,
                kTriglavPlugInPropertyInputKindDefault,
                hueCaption, 'u');
            (*pPropertyService).setIntegerValueProc(propertyObject, kItemKeyHue, 180);
            (*pPropertyService).setIntegerDefaultValueProc(propertyObject, kItemKeyHue, 180);
            (*pPropertyService).setIntegerMinValueProc(propertyObject, kItemKeyHue, 0);
            (*pPropertyService).setIntegerMaxValueProc(propertyObject, kItemKeyHue, 360);
            (*pStringService).releaseProc(hueCaption);

            // 饱和度
            TriglavPlugInStringObject saturationCaption = NULL;
            (*pStringService).createWithAsciiStringProc(&saturationCaption, "Saturation", static_cast<TriglavPlugInInt>(::strlen("Saturation")));
            (*pPropertyService).addItemProc(propertyObject, kItemKeySaturation,
                kTriglavPlugInPropertyValueTypeInteger,
                kTriglavPlugInPropertyValueKindDefault,
                kTriglavPlugInPropertyInputKindDefault,
                saturationCaption, 't');
            (*pPropertyService).setIntegerValueProc(propertyObject, kItemKeySaturation, 50);
            (*pPropertyService).setIntegerDefaultValueProc(propertyObject, kItemKeySaturation, 50);
            (*pPropertyService).setIntegerMinValueProc(propertyObject, kItemKeySaturation, 0);
            (*pPropertyService).setIntegerMaxValueProc(propertyObject, kItemKeySaturation, 100);
            (*pStringService).releaseProc(saturationCaption);

            // 明度
            TriglavPlugInStringObject lightnessCaption = NULL;
            (*pStringService).createWithAsciiStringProc(&lightnessCaption, "Lightness", static_cast<TriglavPlugInInt>(::strlen("Lightness")));
            (*pPropertyService).addItemProc(propertyObject, kItemKeyLightness,
                kTriglavPlugInPropertyValueTypeInteger,
                kTriglavPlugInPropertyValueKindDefault,
                kTriglavPlugInPropertyInputKindDefault,
                lightnessCaption, 'l');
            (*pPropertyService).setIntegerValueProc(propertyObject, kItemKeyLightness, 0);
            (*pPropertyService).setIntegerDefaultValueProc(propertyObject, kItemKeyLightness, 0);
            (*pPropertyService).setIntegerMinValueProc(propertyObject, kItemKeyLightness, -100);
            (*pPropertyService).setIntegerMaxValueProc(propertyObject, kItemKeyLightness, 100);
            (*pStringService).releaseProc(lightnessCaption);

            // 属性设置
            TriglavPlugInFilterInitializeSetProperty(pRecordSuite, hostObject, propertyObject);
            TriglavPlugInFilterInitializeSetPropertyCallBack(pRecordSuite, hostObject, TriglavPlugInFilterPropertyCallBack, *data);

            // 属性销毁
            (*pPropertyService).releaseProc(propertyObject);

            *result = kTriglavPlugInCallResultSuccess;
        }
    }
    else if (selector == kTriglavPlugInSelectorFilterTerminate)
    {
        *result = kTriglavPlugInCallResultSuccess;
    }
    else if (selector == kTriglavPlugInSelectorFilterRun)
    {
        TriglavPlugInRecordSuite* pRecordSuite = &(*pluginServer).recordSuite;
        TriglavPlugInOffscreenService* pOffscreenService = (*pluginServer).serviceSuite.offscreenService;
        TriglavPlugInPropertyService* pPropertyService = (*pluginServer).serviceSuite.propertyService;
        if (TriglavPlugInGetFilterRunRecord(pRecordSuite) != NULL && pOffscreenService != NULL && pPropertyService != NULL)
        {
            TriglavPlugInPropertyObject propertyObject;
            TriglavPlugInFilterRunGetProperty(pRecordSuite, &propertyObject, (*pluginServer).hostObject);

            TriglavPlugInOffscreenObject sourceOffscreenObject;
            TriglavPlugInFilterRunGetSourceOffscreen(pRecordSuite, &sourceOffscreenObject, (*pluginServer).hostObject);

            TriglavPlugInOffscreenObject destinationOffscreenObject;
            TriglavPlugInFilterRunGetDestinationOffscreen(pRecordSuite, &destinationOffscreenObject, (*pluginServer).hostObject);

            TriglavPlugInRect selectAreaRect;
            TriglavPlugInFilterRunGetSelectAreaRect(pRecordSuite, &selectAreaRect, (*pluginServer).hostObject);

            TriglavPlugInOffscreenObject selectAreaOffscreenObject;
            TriglavPlugInFilterRunGetSelectAreaOffscreen(pRecordSuite, &selectAreaOffscreenObject, (*pluginServer).hostObject);

            TriglavPlugInInt r, g, b;
            (*pOffscreenService).getRGBChannelIndexProc(&r, &g, &b, destinationOffscreenObject);

            TriglavPlugInInt blockRectCount;
            (*pOffscreenService).getBlockRectCountProc(&blockRectCount, destinationOffscreenObject, &selectAreaRect);

            std::vector<TriglavPlugInRect> blockRects;
            blockRects.resize(blockRectCount);
            for (TriglavPlugInInt i = 0; i < blockRectCount; ++i)
            {
                (*pOffscreenService).getBlockRectProc(&blockRects[i], i, destinationOffscreenObject, &selectAreaRect);
            }

            TriglavPlugInFilterRunSetProgressTotal(pRecordSuite, (*pluginServer).hostObject, blockRectCount);

            ColorizeFilterInfo* pFilterInfo = static_cast<ColorizeFilterInfo*>(*data);
            (*pFilterInfo).pPropertyService = pPropertyService;
            (*pFilterInfo).colorize = false;
            (*pFilterInfo).hue = 180;
            (*pFilterInfo).saturation = 50;
            (*pFilterInfo).lightness = 0;

            bool restart = true;

            bool colorize;
            int  hue;
            int  saturation;
            int  lightness;

            TriglavPlugInInt blockIndex = 0;
            while (true)
            {
                if (restart)
                {
                    restart = false;

                    TriglavPlugInInt processResult;
                    TriglavPlugInFilterRunProcess(pRecordSuite, &processResult, (*pluginServer).hostObject, kTriglavPlugInFilterRunProcessStateStart);
                    if (processResult == kTriglavPlugInFilterRunProcessResultExit) { break; }

                    colorize   = (*pFilterInfo).colorize;
                    hue        = (*pFilterInfo).hue;
                    saturation = (*pFilterInfo).saturation;
                    lightness  = (*pFilterInfo).lightness;

                    if (colorize || hue != 180 || saturation != 50 || lightness != 0)
                    {
                        blockIndex = 0;
                    }
                    else
                    {
                        // 无修改
                        blockIndex = blockRectCount;
                        TriglavPlugInFilterRunUpdateDestinationOffscreenRect(pRecordSuite, (*pluginServer).hostObject, &selectAreaRect);
                    }
                }

                if (blockIndex < blockRectCount)
                {
                    TriglavPlugInFilterRunSetProgressDone(pRecordSuite, (*pluginServer).hostObject, blockIndex);

                    TriglavPlugInRect blockRect = blockRects[blockIndex];
                    TriglavPlugInPoint pos;
                    pos.x = blockRect.left;
                    pos.y = blockRect.top;
                    TriglavPlugInRect tempRect;

                    TriglavPlugInPtr dstImageAddress;
                    TriglavPlugInInt dstImageRowBytes;
                    TriglavPlugInInt dstImagePixelBytes;
                    (*pOffscreenService).getBlockImageProc(&dstImageAddress, &dstImageRowBytes, &dstImagePixelBytes, &tempRect, destinationOffscreenObject, &pos);

                    TriglavPlugInPtr dstAlphaAddress;
                    TriglavPlugInInt dstAlphaRowBytes;
                    TriglavPlugInInt dstAlphaPixelBytes;
                    (*pOffscreenService).getBlockAlphaProc(&dstAlphaAddress, &dstAlphaRowBytes, &dstAlphaPixelBytes, &tempRect, destinationOffscreenObject, &pos);

                    if (dstImageAddress != NULL && dstAlphaAddress != NULL)
                    {
                        if (selectAreaOffscreenObject == NULL)
                        {
                            BYTE* pDstImageAddressY = static_cast<BYTE*>(dstImageAddress);
                            BYTE* pDstAlphaAddressY = static_cast<BYTE*>(dstAlphaAddress);
                            for (int y = blockRect.top; y < blockRect.bottom; ++y)
                            {
                                BYTE* pDstImageAddressX = pDstImageAddressY;
                                BYTE* pDstAlphaAddressX = pDstAlphaAddressY;
                                for (int x = blockRect.left; x < blockRect.right; ++x)
                                {
                                    if (*pDstAlphaAddressX > 0)
                                    {
                                        ColorizeFilter::Apply(
                                            pDstImageAddressX[r], pDstImageAddressX[g], pDstImageAddressX[b],
                                            hue, saturation, lightness, colorize);
                                    }
                                    pDstImageAddressX += dstImagePixelBytes;
                                    pDstAlphaAddressX += dstAlphaPixelBytes;
                                }
                                pDstImageAddressY += dstImageRowBytes;
                                pDstAlphaAddressY += dstAlphaRowBytes;
                            }
                        }
                        else
                        {
                            TriglavPlugInPtr selectAddress;
                            TriglavPlugInInt selectRowBytes;
                            TriglavPlugInInt selectPixelBytes;
                            (*pOffscreenService).getBlockSelectAreaProc(&selectAddress, &selectRowBytes, &selectPixelBytes, &tempRect, selectAreaOffscreenObject, &pos);

                            if (selectAddress != NULL)
                            {
                                BYTE* pDstImageAddressY = static_cast<BYTE*>(dstImageAddress);
                                BYTE* pDstAlphaAddressY = static_cast<BYTE*>(dstAlphaAddress);
                                const BYTE* pSelectAddressY = static_cast<const BYTE*>(selectAddress);
                                for (int y = blockRect.top; y < blockRect.bottom; ++y)
                                {
                                    BYTE* pDstImageAddressX = pDstImageAddressY;
                                    BYTE* pDstAlphaAddressX = pDstAlphaAddressY;
                                    const BYTE* pSelectAddressX = pSelectAddressY;
                                    for (int x = blockRect.left; x < blockRect.right; ++x)
                                    {
                                        if (*pDstAlphaAddressX > 0)
                                        {
                                            if (*pSelectAddressX == 255)
                                            {
                                                ColorizeFilter::Apply(
                                                    pDstImageAddressX[r], pDstImageAddressX[g], pDstImageAddressX[b],
                                                    hue, saturation, lightness, colorize);
                                            }
                                            else if (*pSelectAddressX != 0)
                                            {
                                                ColorizeFilter::ApplyMask(
                                                    pDstImageAddressX[r], pDstImageAddressX[g], pDstImageAddressX[b],
                                                    *pSelectAddressX,
                                                    hue, saturation, lightness, colorize);
                                            }
                                        }

                                        pDstImageAddressX += dstImagePixelBytes;
                                        pDstAlphaAddressX += dstAlphaPixelBytes;
                                        pSelectAddressX += selectPixelBytes;
                                    }
                                    pDstImageAddressY += dstImageRowBytes;
                                    pDstAlphaAddressY += dstAlphaRowBytes;
                                    pSelectAddressY += selectRowBytes;
                                }
                            }
                        }
                    }
                    TriglavPlugInFilterRunUpdateDestinationOffscreenRect(pRecordSuite, (*pluginServer).hostObject, &blockRect);
                    ++blockIndex;
                }

                TriglavPlugInInt processResult;
                if (blockIndex < blockRectCount)
                {
                    TriglavPlugInFilterRunProcess(pRecordSuite, &processResult, (*pluginServer).hostObject, kTriglavPlugInFilterRunProcessStateContinue);
                }
                else
                {
                    TriglavPlugInFilterRunSetProgressDone(pRecordSuite, (*pluginServer).hostObject, blockIndex);
                    TriglavPlugInFilterRunProcess(pRecordSuite, &processResult, (*pluginServer).hostObject, kTriglavPlugInFilterRunProcessStateEnd);
                }
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
    }
    return;
}
