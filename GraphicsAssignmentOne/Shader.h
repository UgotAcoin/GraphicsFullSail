#pragma once
#include "Defines.h"
#include "MyMath.h"

Matrix4x4 SV_WorldMatrix;
void (*VertexShader)(Float4&);
void (*PixelShader)(unsigned int&);

void VS_World(Float4& v)
{
    v = MatrixVertexMultiply(v, SV_WorldMatrix);
}

void PS_White(unsigned int& color)
{
    color = 0xFFFFFFFF;
}