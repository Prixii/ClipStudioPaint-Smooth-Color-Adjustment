#include "LchColorControl.h"
#include "duilib/Control/ColorConvert.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/GlobalManager.h"
#include"Graphic/CSPMOD_ColorConvert.h"
#include"SDL3/SDL.h"
#include"CspData.h"
#include "BrushColorBlock.h"

using namespace ui;
    BrushColorBlock::BrushColorBlock(ui::Window* pWindow)
       :Box(pWindow)
    {
        SetMargin({6,0,0,0},true);
        //添加两个Option控件
        auto initFunc = [](ui::OptionBox* mainColor, ui::Window* pWindow)
            {

                float borderSize = 2.5f;

                mainColor->AddItem(new ui::Control(pWindow));
                mainColor->SetBorderRound({ 4,4 });
                mainColor->SetBorderSize({ borderSize,borderSize,borderSize,borderSize }, true);
                mainColor->SetBorderColor(ControlStateType::kControlStateHot, L"#CSPColor_BlueButton");
                mainColor->SetBorderColor(ControlStateType::kControlStatePushed, L"#CSPColor_BlueButton");
                mainColor->SetBorderColor(ControlStateType::kControlStateNormal, L"#CSPColor_Separator");
                mainColor->SetFixedWidth(UiFixedInt::MakeStretch(blockLenPercent), true, false);
                mainColor->SetFixedHeight(UiFixedInt::MakeStretch(blockLenPercent), true, false);
                mainColor->SetGroup(L"_ColorBlock");
                mainColor->SetHorAlignType(ui::HorAlignType::kAlignLeft);
                mainColor->SetVerAlignType(ui::VerAlignType::kAlignTop);
                mainColor->SetBkColor(ui::UiColor(0, 0, 0));
                mainColor->SetMouseChildEnabled(false);
                auto pControl = mainColor->GetItemAt(0);

                int margin = (int)borderSize + 1;
                pControl->SetMargin({ margin ,margin ,margin ,margin },false);
                pControl->SetBorderRound({ 2,2 });
            };


        mainColor =  new ui::OptionBox(pWindow);
        mainColor->SetSelected(true);
        initFunc(mainColor,pWindow);
        subColor =  new ui::OptionBox(pWindow);
        initFunc(subColor,pWindow);


        mainColor->SetHorAlignType(ui::HorAlignType::kAlignLeft);
        mainColor->SetVerAlignType(ui::VerAlignType::kAlignTop);
        subColor->SetHorAlignType(ui::HorAlignType::kAlignRight);
        subColor->SetVerAlignType(ui::VerAlignType::kAlignBottom);


        this->AddItem(subColor);
        this->AddItem(mainColor);

        SetMainColor(1.f,1.f,1.f);
        SetSubColor(0.f,0.f,0.f);

        mainColor->AttachSelect(UiBind(&BrushColorBlock::OnColorBlockSelected, this, std::placeholders::_1));
        subColor->AttachSelect(UiBind(&BrushColorBlock::OnColorBlockSelected, this, std::placeholders::_1));

        //mainColor->setforecolor
    }

    bool BrushColorBlock::SetWorkingColorType(CspColorTable::PaintColorType type)
    {
        if (type == CspColorTable::PaintColorType::Main)
        {
            if (!mainColor->IsSelected())
            {
                mainColor->Selected(true, true);
                return true;
            }
        }
        else
        {
            if (!subColor->IsSelected())
            {
                subColor->Selected(true, true);
                return true;
            }
        }
        return false;

    }

    //void BrushColorBlock::SetCurrentColor(const ui::UiColor& uicolor)
    //{
    //    if (mainColor->IsSelected())
    //    {
    //        SetMainColor(uicolor);
    //    }
    //    else
    //    {
    //        SetSubColor(uicolor);
    //    }
    //}

    //void BrushColorBlock::SetMainColor(const ui::UiColor& uicolor)
    //{
    //    mainColor->GetItemAt(0)->SetBkColor(uicolor);
    //}

    void BrushColorBlock::SetCurrentColor(float r, float g, float b)
    {
        if (mainColor->IsSelected())
        {
            SetMainColor(r,g,b);
        }
        else
        {
            SetSubColor(r, g, b);
        }
    }

    bool BrushColorBlock::SetMainColor(float r, float g, float b)
    {
        mainColor->GetItemAt(0)->SetBkColor(
        ui::UiColor(
            static_cast<uint8_t>(std::round(r * UINT8_MAX)),
            static_cast<uint8_t>(std::round(g * UINT8_MAX)),
            static_cast<uint8_t>(std::round(b * UINT8_MAX))
        ));
        mainR = r;
        mainG = g;
        mainB = b;
        return mainColor->IsSelected();
    }

    bool BrushColorBlock::SetSubColor(float r, float g, float b)
    {
        subColor->GetItemAt(0)->SetBkColor(
            ui::UiColor(
                static_cast<uint8_t>(std::round(r * UINT8_MAX)),
                static_cast<uint8_t>(std::round(g * UINT8_MAX)),
                static_cast<uint8_t>(std::round(b * UINT8_MAX))
            ));
        subR = r;
        subG = g;
        subB = b;
        return subColor->IsSelected();
    }

    //void BrushColorBlock::SetSubColor(const ui::UiColor& uicolor)
    //{
    //    subColor-> GetItemAt(0)->SetBkColor(uicolor);
    //}

    bool BrushColorBlock::OnColorBlockSelected(const ui::EventArgs& msg)
    {
        ui::OptionBox* psender =(ui::OptionBox*) msg.GetSender();
        if (psender == mainColor)
        {
            mainColor->SetBorderColor(ControlStateType::kControlStateNormal, L"#CSPColor_BlueButton");
            subColor->SetBorderColor(ControlStateType::kControlStateNormal, L"#CSPColor_Separator");
            this->SetItemIndex(mainColor,1);

            CspColorTable::SetPaintColorType(CspColorTable::PaintColorType::Main);

        }
        else if (psender == subColor)
        {

            subColor->SetBorderColor(ControlStateType::kControlStateNormal, L"#CSPColor_BlueButton");
            mainColor->SetBorderColor(ControlStateType::kControlStateNormal, L"#CSPColor_Separator");
            this->SetItemIndex(mainColor, 0);

            CspColorTable::SetPaintColorType(CspColorTable::PaintColorType::Sub);
        }
        return true;
    }
