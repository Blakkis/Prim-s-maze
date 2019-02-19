#pragma once


template <typename T>
struct W_Vec4
{
    T x, y, z, w;
};


inline W_Vec4<uint8_t> uint32ToVec4(uint32_t color)
{
    W_Vec4<uint8_t> rgba;
    rgba.x = (color >> 24) & 0xff;
    rgba.y = (color >> 16) & 0xff;
    rgba.z = (color >> 8 ) & 0xff;
    rgba.w =  color        & 0xff;
    return rgba;
}
