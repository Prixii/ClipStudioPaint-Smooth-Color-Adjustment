#pragma once
#include "../Common/FilterPlugIn/TriglavPlugInSDK/TriglavPlugInType.h"
#include "../Common/FilterPlugIn/TriglavPlugInSDK/TriglavPlugInServer.h"
#include "../Common/FilterPlugIn/TriglavPlugInSDK/TriglavPlugInService.h"

//extern void  ReplaceColor_TriglavPlugInFilterPropertyCallBack(TriglavPlugInInt* result, TriglavPlugInPropertyObject propertyObject, const TriglavPlugInInt itemKey, const TriglavPlugInInt notify, TriglavPlugInPtr data)
extern void  ReplaceColor_TriglavPluginCall(TriglavPlugInInt* result, TriglavPlugInPtr* data, TriglavPlugInInt selector, TriglavPlugInServer* pluginServer, TriglavPlugInPtr reserved);