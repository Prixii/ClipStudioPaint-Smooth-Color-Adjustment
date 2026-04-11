#include "TriglavPlugInSDK/TriglavPlugInSDK.h"
#include<assert.h>
#include <iostream>
#include <vector>
#include<thread>
#include <chrono>


#include"../Common/FilterPlugIn/FilterPluginHost_Export.h"
#include"pch.h"



//	プラグインメイン
// plugin main？
void TRIGLAV_PLUGIN_API TriglavPluginCall(TriglavPlugInInt* result, TriglavPlugInPtr* data, TriglavPlugInInt selector, TriglavPlugInServer* pluginServer, TriglavPlugInPtr reserved)
{

    *result = kTriglavPlugInCallResultFailed;


    typedef void (*ReplaceCall)(TriglavPlugInInt * result, TriglavPlugInPtr * data, TriglavPlugInInt selector, TriglavPlugInServer * pluginServer, TriglavPlugInPtr reserved);
    static ReplaceCall replaceCall = nullptr;
    if (!replaceCall)
    {

        auto handle = GetModuleHandle(L"CSPMOD.dll");
        if (handle)
        {


            SetUpPluginInfoCall setupPluginInfoCall = (SetUpPluginInfoCall)GetProcAddress(handle, "SetUpPluginInfo");
            if (setupPluginInfoCall)
            {
                CSPFilterPlugin_Interface pluginInterface = {};
                setupPluginInfoCall("ReplaceColor", &pluginInterface);
                replaceCall = pluginInterface.TriglavPluginCall;
            }
        }
    }
    if (replaceCall)
    {
        return replaceCall(result,data,selector,pluginServer, reserved);
    }
    return;


}

