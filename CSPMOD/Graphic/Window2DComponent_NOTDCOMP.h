#ifndef _Window2DComponent_NOTDCOMP_H
#define _Window2DComponent_NOTDCOMP_H


#include"DeviceRelated.h"
class Cat2D;
namespace c2d {
    namespace bind
    {
        class RenderTexture;

    }
    class Window2DComponent_NDCOM:public DeviceRelated
    {
    public:
        Window2DComponent_NDCOM() :/*vcam(this),*/ pC2D(NULL){};
        virtual ~Window2DComponent_NDCOM() {};
        virtual void Init(HWND hwnd, unsigned int bufferW, unsigned int bufferH, Cat2D* pC2D = NULL);
        void SetViewPort(unsigned int viewW, unsigned int viewH);
        void Clear(float r=0.f, float b = 0.f, float g = 0.f, float a = 0.f);
        void Present();
        void SetOffScreenRenderTarget();
        void SetOpacity(float opacity);
        void SetFlip(bool hor,bool ver);
        HANDLE GetTextureHandle();
        Cat2D& GetC2DInstance() { return *pC2D; };

        void LockTexture(uint64_t key);
        void ReleaseTexture(uint64_t key);

        void ResizeBuffer(int newW,int newH);
        void GetViewSize(int* w,int* h);
        cat::Vector2i GetViewSize();
        bool GetResizeFlag();
        bool CopyBuffer(uint8_t* buff,size_t buflen,size_t* outlen, ID3D11Resource* presource);//失败时，可以检查outlen看是否是因为buf长度而失败


        std::shared_ptr<bind::RenderTexture> GetRenderTarget() { return offscreenTexture; };
    protected:
        Cat2D* pC2D;
        std::shared_ptr<bind::RenderTexture> offscreenTexture;//这个纹理是多重采用的
        //std::unique_ptr<bind::Viewport> viewport;



        Microsoft::WRL::ComPtr <IDXGISwapChain1> pSwapChain;
        Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
        Microsoft::WRL::ComPtr <ID3D11RenderTargetView> pRenderTarget;
        Microsoft::WRL::ComPtr<IDXGIKeyedMutex>pDXGIKeyedMutex;


        Microsoft::WRL::ComPtr<IDCompositionTarget> pComposTarget;
        Microsoft::WRL::ComPtr<IDCompositionVisual> pDCompVisual;
        Microsoft::WRL::ComPtr<IDCompositionEffectGroup> pDCompEffectGroup;

    private:

        //VirtualCamera vcam;

        bool ResizedFlag = false;
        bool isResized=false;
        int targetSizeW;
        int targetSizeH;
    };
}
#endif