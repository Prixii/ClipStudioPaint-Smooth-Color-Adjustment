#include "../Common/FilterPlugIn/TriglavPlugInSDK/TriglavPlugInType.h"
#include "../Common/FilterPlugIn/TriglavPlugInSDK/TriglavPlugInServer.h"
#include "../Common/FilterPlugIn/TriglavPlugInSDK/TriglavPlugInService.h"




typedef bool (*SetUpPluginInfoCall)(const char* pluginName, struct CSPFilterPlugin_Interface* interFace);


struct CSPFilterPlugin_Interface
{
	//void (*TriglavPlugInFilterPropertyCallBack)(TriglavPlugInInt* result, TriglavPlugInPropertyObject propertyObject, const TriglavPlugInInt itemKey, const TriglavPlugInInt notify, TriglavPlugInPtr data) = nullptr;
	void (*TriglavPluginCall)(TriglavPlugInInt* result, TriglavPlugInPtr* data, TriglavPlugInInt selector, TriglavPlugInServer* pluginServer, TriglavPlugInPtr reserved) = nullptr;

};