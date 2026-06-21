#pragma once

typedef unsigned char BYTE;

//!
//! @brief  HSL 着色滤镜
//!
//! RGB → HSL → 替换/调整 H, S, L → RGB
//!
class ColorizeFilter
{
public:
    //! 对单个像素执行着色（无遮罩）
    //! @param rR, rG, rB  --- [in/out] RGB 像素值 (0-255)
    //! @param h, s, l     --- [in] 用户设定的 H(0-360), S(0-100), L(-100,100)
    //! @param colorize    --- [in] true=绝对着色(替换H,S), false=增量调整
    inline static void Apply(
        BYTE& rR, BYTE& rG, BYTE& rB,
        int h, int s, int l,
        bool colorize)
    {
        float fH, fS, fL;
        RGBAtoHSLA(rR, rG, rB, fH, fS, fL);

        if (colorize)
        {
            // 着色模式：H 和 S 设为绝对值，L 做增量调整
            fH = h / 360.0f;
            fS = s / 100.0f;
        }
        else
        {
            // 增量模式：H, S 做增量调整
            // H 范围 -180..180 → 映射为 HSL 色相偏移
            float hOffset = (h - 180) / 360.0f;
            fH += hOffset;
            if (fH < 0.0f) fH += 1.0f;
            if (fH >= 1.0f) fH -= 1.0f;

            // S 范围 0..100 → 映射为 -100..0 当 h<180，0..100 当 h≥180 不，直接用 s
            // 增量模式下 s 含义为 -100..100
            float sAdj = (s - 50) / 100.0f;
            fS += sAdj;
            if (fS < 0.0f) fS = 0.0f;
            if (fS > 1.0f) fS = 1.0f;
        }

        // L 始终做增量调整
        float lAdj = l / 100.0f;
        fL += lAdj;
        if (fL < 0.0f) fL = 0.0f;
        if (fL > 1.0f) fL = 1.0f;

        HSLAtoRGBA(fH, fS, fL, rR, rG, rB);
    }

    //! 带遮罩的像素着色
    //! @param mask --- [in] 选区遮罩 (0=不处理, 255=完全处理)
    inline static void ApplyMask(
        BYTE& rR, BYTE& rG, BYTE& rB,
        BYTE mask,
        int h, int s, int l,
        bool colorize)
    {
        BYTE oldR = rR, oldG = rG, oldB = rB;
        Apply(rR, rG, rB, h, s, l, colorize);
        rR = (BYTE)Blend8(rR, oldR, mask);
        rG = (BYTE)Blend8(rG, oldG, mask);
        rB = (BYTE)Blend8(rB, oldB, mask);
    }

private:
    //! RGB(0-255) → HSL(0.0-1.0)
    inline static void RGBAtoHSLA(
        BYTE r, BYTE g, BYTE b,
        float& outH, float& outS, float& outL)
    {
        float rf = r / 255.0f;
        float gf = g / 255.0f;
        float bf = b / 255.0f;

        float maxC = rf > gf ? (rf > bf ? rf : bf) : (gf > bf ? gf : bf);
        float minC = rf < gf ? (rf < bf ? rf : bf) : (gf < bf ? gf : bf);
        float delta = maxC - minC;

        outL = (maxC + minC) / 2.0f;

        if (delta == 0.0f)
        {
            outH = 0.0f;
            outS = 0.0f;
        }
        else
        {
            outS = outL > 0.5f
                ? delta / (2.0f - maxC - minC)
                : delta / (maxC + minC);

            if (maxC == rf)
                outH = (gf - bf) / delta + (gf < bf ? 6.0f : 0.0f);
            else if (maxC == gf)
                outH = (bf - rf) / delta + 2.0f;
            else
                outH = (rf - gf) / delta + 4.0f;

            outH /= 6.0f;
        }
    }

    //! HSL(0.0-1.0) → RGB(0-255)
    inline static void HSLAtoRGBA(
        float h, float s, float l,
        BYTE& outR, BYTE& outG, BYTE& outB)
    {
        if (s == 0.0f)
        {
            BYTE v = (BYTE)(l * 255.0f);
            outR = outG = outB = v;
            return;
        }

        float q = l < 0.5f
            ? l * (1.0f + s)
            : l + s - l * s;
        float p = 2.0f * l - q;

        outR = (BYTE)(Hue2RGB(p, q, h + 1.0f / 3.0f) * 255.0f);
        outG = (BYTE)(Hue2RGB(p, q, h) * 255.0f);
        outB = (BYTE)(Hue2RGB(p, q, h - 1.0f / 3.0f) * 255.0f);
    }

    inline static float Hue2RGB(float p, float q, float t)
    {
        if (t < 0.0f) t += 1.0f;
        if (t > 1.0f) t -= 1.0f;
        if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
        if (t < 1.0f / 2.0f) return q;
        if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
        return p;
    }

    //! Alpha 混合
    inline static int Blend8(const int dst, const int src, const int mask)
    {
        return ((dst - src) * mask / 255) + src;
    }
};
