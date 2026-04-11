#ifndef HSVConvert
#define HSVConvert



float3 RGBtoHSV(float3 colorRGB)
{
    float fMax, fMin;
    float cuR = colorRGB.x;
    float cuG = colorRGB.y;
    float cuB = colorRGB.z;

    fMax = max(max(cuR, cuG), cuB);
    fMin = min(min(cuR, cuG), cuB);
    
    float fD = fMax - fMin;
    
    float fH;
    float fS;
//    float fV;
    
    if (fMax == 0)
        fS = 0;
    else
        fS = fD / fMax;
    
    if (fS == 0)
        fH = 0;
    else
    {
        if (fMax == cuR)
        {
            fH = (cuG - cuB) / fD; //s不为0的时候 D一定不为0
            if (fH < 0)
                fH += 6;
        }
        else if (fMax == cuG)
        {
            fH = 2 + (cuB - cuR) / fD;
        }
        else
        {
            fH = 4 + (cuR - cuG) / fD;
        }
    }
    
    return float3(fH, fS, fMax);
}



float3 HSVFilter(float3 hsvOrig, float3 hsvParam)
{
    float hmax = 6;
    float vmax = 1;
    float smax = 1;
    
    //h:0~6 //s -1~1 //v -1~1
    float hsv_origH = hsvOrig.x;
    float hsv_origS = hsvOrig.y;
    float hsv_origV = hsvOrig.z;
    
    float hsv_paramH = hsvParam.x;
    float hsv_paramS = hsvParam.y;
    float hsv_paramV = hsvParam.z;
    
    float hsv_resultH = hsv_origH;
    float hsv_resultS = hsv_origS;
    float hsv_resultV = hsv_origV;
    
    
    
  
    hsv_resultH += hsv_paramH;
    if (hsv_resultH > hmax)
        hsv_resultH -= hmax;
    
    if (hsv_paramV > 0)
    {
        //在原始明度值和最大明度值之间线形插值
        hsv_resultV += (vmax - hsv_resultV) * hsv_paramV;
        //没看出这里的必要性，是在最大值之间按比例插值，应该不会导致超出最大值才对
//        hsv_resultV=max(hsv_resultV,vmax);
        
        //饱和度在0之间插值，
        //为什么csp这里区别对待饱和度
        //虽然越亮饱和越低确实没错
        hsv_resultS -= hsv_resultS * hsv_paramV;
//        if
    }
    else
    {
        //也是到0之间线形插值
        hsv_resultV -= (0 - hsv_paramV) * hsv_resultV;
    }
    
    if (hsv_paramS != 0 && hsv_resultS > 0 && hsv_resultV > 0)
    {
        if (hsv_paramS > 0)
        {
            float _temS = (smax - hsv_resultS) * hsv_paramS;
            float _temV1 = hsv_resultV * _temS;
            float _temV2 = hsv_resultV;
            
            hsv_resultV += _temV1;
            if (hsv_resultV > vmax)
            {
                hsv_resultV = vmax;
                hsv_resultS += _temS * (hsv_resultV - _temV2) / _temV1;
            }
            else
            {
                hsv_resultS += _temS;
            }
        }
        else
        {
            float _temS2 = hsv_resultS;
            hsv_resultS -= hsv_resultS * (0 - hsv_paramS);
            if (hsv_resultS < 0)
                hsv_resultS = 0;
            
//            hsv_resultV-=(_temS2-hsv_resultS)*hsv_resultV*0.25f;
            hsv_resultV -= (_temS2 - hsv_resultS) * hsv_resultV;
            if (hsv_resultV < 0)
                hsv_resultV = 0;
        }
    }
    
    float3 result;
    result.x = hsv_resultH;
    result.y = hsv_resultS;
    result.z = hsv_resultV;
    return result;
}



float GetP1(float cuV, float cuS, float cuF)
{
    return cuV * (1 - cuS);
}
float GetP2(float cuV, float cuS, float cuF)
{
    return cuV * (1 - cuS * cuF);
}
float GetP3(float cuV, float cuS, float cuF)
{
    return cuV * (1 - cuS * (1 - cuF));
}




//h0~6 s0~1 v0~1
float3 HSVtoRGB(float3 hsv)
{
    float H = hsv.x;
    float S = hsv.y;
    float V = hsv.z;
    
    
    float cuF = H - floor(H);
    
    

    
    float3 result;
    if (H < 1)
    {
        result.x = V;
        result.y = GetP3(V, S, cuF);
        result.z = GetP1(V, S, cuF);
    }
    else if (H < 2)
    {
        result.x = GetP2(V, S, cuF);
        result.y = V;
        result.z = GetP1(V, S, cuF);
    }
    else if (H < 3)
    {
        result.x = GetP1(V, S, cuF);
        result.y = V;
        result.z = GetP3(V, S, cuF);
    }
    else if (H < 4)
    {
        result.x = GetP1(V, S, cuF);
        result.y = GetP2(V, S, cuF);
        result.z = V;
    }
    else if (H < 5)
    {
        result.x = GetP3(V, S, cuF);
        result.y = GetP1(V, S, cuF);
        result.z = V;
    }
    else
    {
        result.x = V;
        result.y = GetP1(V, S, cuF);
        result.z = GetP2(V, S, cuF);
    }
//    result.x+=M;
//    result.y+=M;
//    result.z+=M;
    result=clamp(result,0,1);
    return result;
}


#endif