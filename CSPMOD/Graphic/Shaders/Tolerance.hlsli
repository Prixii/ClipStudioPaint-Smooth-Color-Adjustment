
#include"HSVConvert.hlsli"

//先简单使用距离的方式，后续转为使用Lab色差
//float CalcPixWeight(float3 color1, float3 color2, float tolerance)
//{
//    if (tolerance > distance(color1, color2) / 1.732051f)
//    {
//        return 1.f;
//    }
//    else
//    {
//        return 0.f;
//    }
//    return 1.f;
//}


//简单计算RGB距离,tolerance参数为半透明选区范围，线性
//会出现明显的高亮条带
//float CalcPixWeight(float3 color1, float3 color2, float tolerance)
//{
//    float d = distance(color1, color2) / 1.732051f;
//    if (tolerance > d)
//    {
//        return (tolerance - d) / tolerance;
//    }
//    else
//    {
//        return 0.f;
//    }
//    return 1.f;
//}

//简单计算RGB距离,tolerance参数为半透明选区范围，高斯函数
//容差高的时候范围巨大，容差低的时候边缘巨硬
//float CalcPixWeight(float3 color1, float3 color2, float tolerance)
//{
//    float d = distance(color1, color2) / 1.732051f;//sqrt3
//    return 1 / (2 * 3.1415927f * tolerance) * exp(-d * d / (2 * tolerance));
//    //return 0.5f*cos(3.1415927f*d/ tolerance)+0.5f;
//}

//简单计算RGB距离,tolerance参数为半透明选区范围，线性
//会出现明显的高亮条带
//float CalcPixWeight(float3 color1, float3 color2, float tolerance)
//{
//    float d = distance(color1, color2) / 1.732051f;
//    if (tolerance > d*d)
//    {
//        return (tolerance - d*d) / tolerance;
//    }
//    else
//    {
//        return 0.f;
//    }
//    return 1.f;
//}


//简单计算RGB距离,tolerance参数为半透明选区范围，正弦
//截断功能欠缺，容差稍大会导致选区过大
//float CalcPixWeight(float3 color1, float3 color2, float tolerance)
//{
//    float d = distance(color1, color2) / 1.732051f; //sqrt3
//    //return 1 / (2 * 3.1415927f * tolerance) * exp(-d * d / (2 * tolerance));
//    tolerance += 0.01F;
//    if (d > 2 * tolerance)
//        return 0.f;
//     return 0.5f * cos(3.1415927f * 0.5f * d / tolerance) + 0.5f;
//}



//LAB D65

float GetTValue(float t)
{
    if (t>0.008856)
    {
        return pow(t,1.f/3.f);
    }
    
    return 7.787037 * t + 0.137931;
    
}

float3 RGB2Lab(float3 rgb)
{
    float3 xyz;
    xyz.x = 0.412453 * rgb.r + 0.357580f * rgb.g + 0.180423 * rgb.b;
    xyz.y = 0.212671 * rgb.r + 0.715160 * rgb.g + 0.072169 * rgb.b;
    xyz.z = 0.019334 * rgb.r + 0.119193 * rgb.g + 0.950227 * rgb.b;
    
    xyz.x /= 0.95047;
    //xyz.y /= 1;
    xyz.z /= 1.08883;
    
    //T
    xyz.x = GetTValue(xyz.x);
    xyz.y = GetTValue(xyz.y);
    xyz.z = GetTValue(xyz.z);
    
    float3 Lab;
    Lab.x = 116 * xyz.y - 16;//L
    Lab.y = 500 * (xyz.x-xyz.y);//a
    Lab.b = 200 * (xyz.y-xyz.z);//b
    
    return Lab;
}

#define DEG2RAD 0.0174532925
#define PI 3.14159265358
float DistanceLab(float3 lab1, float3 lab2)
{
    //deltaE00
    
     // 1. 提取分量
    float L1 = lab1.x, a1 = lab1.y, b1 = lab1.z;
    float L2 = lab2.x, a2 = lab2.y, b2 = lab2.z;
    
    
    // 2. 计算平均值和初步差值
    float L_bar = 0.5 * (L1 + L2);
    float C1 = sqrt(a1 * a1 + b1 * b1);
    float C2 = sqrt(a2 * a2 + b2 * b2);
    float C_bar = 0.5 * (C1 + C2);
    
    // 3. 计算 G 因子 (彩度加权)
    float C_bar_pow7 = pow(C_bar, 7.0);
    float G = 0.5 * (1.0 - sqrt(C_bar_pow7 / (C_bar_pow7 + 25.0 * 25.0 * 25.0 * 25.0 * 25.0 * 25.0 * 25.0))); // 25^7

     // 4. 计算修正后的 a'
    float a1_prime = a1 * (1.0 + G);
    float a2_prime = a2 * (1.0 + G);
    
        // 5. 重新计算彩度 C' 和 色相 h'
    float C1_prime = sqrt(a1_prime * a1_prime + b1 * b1);
    float C2_prime = sqrt(a2_prime * a2_prime + b2 * b2);
    float C_bar_prime = 0.5 * (C1_prime + C2_prime);
    
        // 计算 h' (注意象限处理，使用 atan2)
    float h1_prime = degrees(atan2(b1, a1_prime));
    if (h1_prime < 0.0)
        h1_prime += 360.0;
    
    float h2_prime = degrees(atan2(b2, a2_prime));
    if (h2_prime < 0.0)
        h2_prime += 360.0;

    // 6. 计算差值 Delta
    float dL_prime = L2 - L1;
    float dC_prime = C2_prime - C1_prime;
    
    float dh_prime;
    float dH_prime;
    
    if (abs(h2_prime - h1_prime) <= 180.0)
    {
        dh_prime = h2_prime - h1_prime;
    }
    else
    {
        if (h2_prime <= h1_prime)
            dh_prime = h2_prime - h1_prime + 360.0;
        else
            dh_prime = h2_prime - h1_prime - 360.0;
    }
    
    dH_prime = 2.0 * sqrt(C1_prime * C2_prime) * sin(0.5 * dh_prime * DEG2RAD);

    // 7. 计算平均值 H'
    float H_bar_prime;
    if (abs(h2_prime - h1_prime) <= 180.0)
        H_bar_prime = 0.5 * (h1_prime + h2_prime);
    else
    {
        if (h1_prime + h2_prime < 360.0)
            H_bar_prime = 0.5 * (h1_prime + h2_prime + 360.0);
        else
            H_bar_prime = 0.5 * (h1_prime + h2_prime - 360.0);
    }

    // 8. 计算权重函数 T, S_L, S_C, S_H
    float H_bar_rad = H_bar_prime * DEG2RAD;
    float T = 1.0
            - 0.17 * cos(H_bar_rad - 30.0 * DEG2RAD)
            + 0.24 * cos(2.0 * H_bar_rad)
            + 0.32 * cos(3.0 * H_bar_rad + 6.0 * DEG2RAD)
            - 0.20 * cos(4.0 * H_bar_rad - 63.0 * DEG2RAD);

    float dTheta = 30.0 * exp(-pow((H_bar_prime - 275.0) / 25.0, 2.0));
    float R_C = 2.0 * sqrt(pow(C_bar_prime, 7.0) / (pow(C_bar_prime, 7.0) + 25.0 * 25.0 * 25.0 * 25.0 * 25.0 * 25.0 * 25.0));
    float S_L = 1.0 + (0.015 * pow(L_bar - 50.0, 2.0)) / sqrt(20.0 + pow(L_bar - 50.0, 2.0));
    float S_C = 1.0 + 0.045 * C_bar_prime;
    float S_H = 1.0 + 0.015 * C_bar_prime * T;
    float R_T = -sin(2.0 * dTheta * DEG2RAD) * R_C;

    // 9. 最终计算 (假设 kL=kC=kH=1)
    float term1 = dL_prime / S_L;
    float term2 = dC_prime / S_C;
    float term3 = dH_prime / S_H;
    
    float deltaE = sqrt(term1 * term1 + term2 * term2 + term3 * term3 + R_T * term2 * term3);

    return deltaE;
}

float CalcPixWeight(float3 color1, float3 color2, float tolerance)
{
    float t = tolerance * 200*3+0.001;
    float d = DistanceLab(RGB2Lab(color1), RGB2Lab(color2));
    d = d * d;
    if (t > d)
    {
        return (t - d) / t;
    }
    else
    {
        return 0.f;
    }
    return 1.f;
}




float _LinearToGamma_(float c)
{
    if (c <= 0.0031308f)
    {
        return 12.92f * c;
    }
    else
    {
        return 1.055f * pow(c, 1.0f / 2.4f) - 0.055f;
    }
}
float3 LinearToGamma(float3 c)
{
    return float3(
    _LinearToGamma_(c.r),
    _LinearToGamma_(c.g),
    _LinearToGamma_(c.b));

}

float _GammaToLinear_(float c)
{
    if (c <= 0.04045f)
    {
        //return c / 12.92f;
        return c * 0.077399381;
    }
    else
    {
        return pow((c + 0.055f) / 1.055f, 2.4f);
    }
}
float3 GammaToLinear(float3 c)
{
    return float3(
    _GammaToLinear_(c.r),
    _GammaToLinear_(c.g),
    _GammaToLinear_(c.b));

}




//0~100 0~100 0~360
float3 LCH_to_sRGB(float3 LCH)
{
    //float h_rad = LCH.z * DEG2RAD;
    float h_rad = LCH.z ;
    float lab_a = LCH.y * cos(h_rad)*1.28f;//a实际是-128~128,软件实现中把C也设定为了100
    float lab_b = LCH.y * sin(h_rad)*1.28f;
    
    //参考白点 0.95047  1 1.08883
    const float Xn = 0.95047f;
    const float Yn = 1.00000f;
    const float Zn = 1.08883f;
    
    
    float fy = (LCH.x + 16.0f) / 116.0f;
    float fx = lab_a / 500.0f + fy;
    float fz = fy - lab_b / 200.0f;
    
    float x = Xn * ((fx * fx * fx > 0.008856f) ? (fx * fx * fx) : ((fx - 16.0f / 116.0f) / 7.787f));
    float y = Yn * ((fy * fy * fy > 0.008856f) ? (fy * fy * fy) : ((fy - 16.0f / 116.0f) / 7.787f));
    float z = Zn * ((fz * fz * fz > 0.008856f) ? (fz * fz * fz) : ((fz - 16.0f / 116.0f) / 7.787f));
    
    float3 resultCol;
    resultCol.r = x * 3.2404542f - y * 1.5371385f - z * 0.4985314f;
    resultCol.g = x * -0.9692660f + y * 1.8760108f + z * 0.0415560f;
    resultCol.b = x * 0.0556434f - y * 0.2040259f + z * 1.0572252f;
    return clamp(LinearToGamma(resultCol),0,1);
}



//因为这个函数目前只用来计算色相，这里移除一些内容
float3 sRGB_to_LCH(float3 srgb)
{
    float3 rgb = GammaToLinear(srgb);
    
    float x = 0.412453f * rgb.r + 0.357580f * rgb.g + 0.180423f * rgb.b;
    float y = 0.212671f * rgb.r + 0.715160f * rgb.g + 0.072169f * rgb.b;
    float z = 0.019334f * rgb.r + 0.119193f * rgb.g + 0.950227f * rgb.b;
    
    //参考白点 0.95047  1 1.08883
    x /= 0.95047f;
	//y /= 1;
    z /= 1.08883f;
    
    x = GetTValue(x);
    y = GetTValue(y);
    z = GetTValue(z);
    
    float3 lch;
    //lch.x = clamp(((116 * y - 16)), 0.f, 100.f);
    float lab_a = 500 * (x - y);
    float lab_b = 200 * (y - z);
    //lch.y = clamp(sqrt(lab_a * lab_a + lab_b * lab_b), 0.f, 100.f);
    lch.z = atan2(lab_b, lab_a);
    if (lch.z < 0)
        lch.z = lch.z+ 
    2 * 3.14159265359f;
    return lch;

}




float RGBH2LCHH(float h)
{
    float hInRgb = h * 0.95493f; //0~2pi 转为0~6;
    float3 rgb=HSVtoRGB(float3(hInRgb, 1.f, 0.5f));
    //float L, C, H;
    return sRGB_to_LCH(rgb).z;
}


#define SAMPLER_FROM_TABLE(h_rad,light) L_MaxC_Table.Sample(TableSampler, float2(h_rad*0.1591549431f,light*0.01f))


float DistancePointToLine2D(float2 P, float2 A, float2 B)
{
    float2 AB = B - A;
    float2 AP = P - A;
    
    // 2D 叉乘公式：AB.x * AP.y - AB.y * AP.x
    // 这计算的是平行四边形的面积
    float cross = AB.x * AP.y - AB.y * AP.x;
    
    // 距离 = 面积 / 底边长度
    float lenAB = length(AB);
    
    // 防止除以 0 (如果 A 和 B 重合)
    if (lenAB < 1e-6)
        return length(AP);
    
    return abs(cross) / lenAB;
}