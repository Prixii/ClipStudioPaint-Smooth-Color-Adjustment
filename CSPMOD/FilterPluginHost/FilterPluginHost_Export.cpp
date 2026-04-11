#include<iostream>
#include"../Common/FilterPlugIn/FilterPluginHost_Export.h"
#include"ReplaceColor/ReplaceColorPlugin.h"


extern "C" __declspec(dllexport) bool SetUpPluginInfo(const char* pluginName, CSPFilterPlugin_Interface* interFace)
{ 
	if (strcmp(pluginName, "ReplaceColor") == 0)
	{
		//interFace->TriglavPlugInFilterPropertyCallBack = ReplaceColor_TriglavPlugInFilterPropertyCallBack;
		interFace->TriglavPluginCall = ReplaceColor_TriglavPluginCall;

		return true;
	}


	return false;
}

