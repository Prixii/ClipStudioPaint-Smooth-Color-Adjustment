#pragma once
#ifndef ReplaceColorForm_H
#define ReplaceColorForm_H


#include"Graphic/RenderSubWindow.h"
#include"DUI/DuiCommon.h"


class ReplaceColorForm : public ui::WindowImplBase
{
	static inline ReplaceColorForm* pthis = nullptr;
public:
	static void ShowForm();
	static ReplaceColorForm* GetCurrentForm() { return pthis; };



	ReplaceColorForm();
	virtual ~ReplaceColorForm();

	/**
	 * 一下三个接口是必须要覆写的接口，父类会调用这三个接口来构建窗口
	 * GetSkinFolder		接口设置你要绘制的窗口皮肤资源路径
	 * GetSkinFile			接口设置你要绘制的窗口的 xml 描述文件
	 * GetWindowClassName	接口设置窗口唯一的类名称
	 */
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;



	/**
	 * 收到 WM_CREATE 消息时该函数会被调用，通常做一些控件初始化的操作
	 */
	virtual void OnInitWindow() override;
	virtual void OnCloseWindow()override;


	//监听回车键 ESC键
	virtual LRESULT OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;
	virtual LRESULT OnKeyUpMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

	//监听Capture丢失的信息
	virtual LRESULT OnCaptureChangedMsg(const ui::NativeMsg& nativeMsg, bool& bHandled) override;





	//监听鼠标移动，移动出滤镜窗口是setcapture
	virtual LRESULT OnMouseMoveMsg(const ui::UiPoint& pt, uint32_t modifierKey, bool bFromNC, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

	virtual LRESULT OnMouseLButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;
	virtual LRESULT OnMouseLButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

	//监听子窗口控件大小变动
	bool OnChildWindowControlPosSizeChanged(const ui::EventArgs& args);
	ui::UiRect CalcChildWindowPos(ui::Control* pChildHost);


public:
	static void FocusCheck();

	static void RenderLoop();





	bool OnButtonClick(const ui::EventArgs& args);
	bool OnEyedropperOptionSelect(const ui::EventArgs& args);
	bool OnSliderValueChange(const ui::EventArgs& args);
	bool OnSliderValueTextChange(const ui::EventArgs& args);


	//void OnCursorMoveInCanvas(double x, double y);
	bool GetCurrentPosInCanvas(double* x, double* y);

	bool IsMouseOnPaintWindow();




	void __SetCapture();
	void __ReleaseCapture();
	bool isDataFilled = false;



private:

	void SetSipCursor();
	void DoPickColor();







	bool isCloseWithOK = false;


	bool needDrawCanvas = false;
	bool needDrawSelectArea=false;
	bool LButtonDown = false;





	bool isColorPicking = false;
	inline static ui::UiColor lastPickColor;
	ui::UiColor pickingColor;






	//UI参数
	int colorTolerance = 50;
	int H=0;
	int S=0;
	int V=0;
	int strength=100;
	bool bPreview = true;



	enum SipCursorType 
	{
		SipCursorType_Normal,
		SipCursorType_Add,
		SipCursorType_Sub
	};
	SipCursorType currentCursorType = SipCursorType_Normal;
	SipCursorType workingCursorType = SipCursorType_Normal;
	






	uintptr_t originFilterDlg=0;


	//CONTROLS
	ui::Option* option_dropper_normal =nullptr;
	ui::Option* option_dropper_add =nullptr;
	ui::Option* option_dropper_sub =nullptr;


	ui::Control* colorBlock_target=nullptr;
	ui::Control* colorBlock_result=nullptr;


	ui::Slider* slider_tolerance = nullptr;
	ui::Slider* slider_hue = nullptr;
	ui::Slider* slider_saturation = nullptr;
	ui::Slider* slider_value = nullptr;
	ui::Slider* slider_strength = nullptr;

	ui::RichEdit* edit_tolerance = nullptr;
	ui::RichEdit* edit_hue = nullptr;
	ui::RichEdit* edit_saturation = nullptr;
	ui::RichEdit* edit_value = nullptr;
	ui::RichEdit* edit_strength = nullptr;

	RenderSubWindow renderSubwWindow;

};





#endif
