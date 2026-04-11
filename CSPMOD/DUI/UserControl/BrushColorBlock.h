#ifndef BrushColorBlock_H_
#define BrushColorBlock_H_

#include "DUI/DuiCommon.h"
#include "duilib/Render/IRender.h"
#include"CspData.h"
#include"Graphic/Texture.h"

    /**显示当前主色和子色的方块*/



    class BrushColorBlock : public ui::Box
    {


        typedef  ui::Control BaseClass;
    public:

        static inline constexpr uint32_t blockLenPercent = 60;



        explicit BrushColorBlock(ui::Window* pWindow);

        /** 获取控件类型
        */
        virtual DString GetType() const override { return L"BrushColorBlock"; };





        bool SetWorkingColorType(CspColorTable::PaintColorType type);

        //void SetCurrentColor(const ui::UiColor& uicolor);
        void SetCurrentColor(float r, float g, float b);
        //void SetMainColor(const ui::UiColor& uicolor);
        bool SetMainColor(float r,float g,float b);//返回true表示当前控件也是mainColor，外部可能需要刷新
        //void SetMainColor(struct Color96* col96);
        //void SetSubColor(const ui::UiColor& uicolor);
        bool SetSubColor(float r, float g, float b);
        //void SetSubColor(struct Color96* col96);


        void GetCurrentColor(float* r, float* g, float* b) 
        {
            if (mainColor && mainColor->IsSelected())
            {
                return GetMainColor(r,g,b);
            }
            return GetSubColor(r, g, b);
        };
        void GetMainColor(float* r, float* g, float* b) { *r = mainR; *g= mainG; *b = mainB; };
        void GetSubColor(float* r,float* g,float* b) { *r = subR; *g = subG; *b = subB; };
    private:
        ui::OptionBox* mainColor;//使用Box是为了实现边框绘制
        ui::OptionBox* subColor;

        float mainR;
        float mainG;
        float mainB;
        float subR;
        float subG;
        float subB;

        bool OnColorBlockSelected(const ui::EventArgs& msg);
    };


#endif //UI_CONTROL_COLOR_CONTROL_H_