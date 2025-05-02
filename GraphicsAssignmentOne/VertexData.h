#pragma once
#include "Defines.h"

#include <cstdlib>
#include <cmath>

#ifndef GRID_CUBE_VERTS_DEFINED
#define GRID_CUBE_VERTS_DEFINED

Float4 gridVerts[44];
Float4WithColor cubeVertsUV[36];
Star starField[3000];
Float4WithColor stoneVerts[1457];
unsigned int stoneIndices[2532];
constexpr int GROUND_SEGMENTS = 64;
Float4WithColor groundVerts[GROUND_SEGMENTS + 2];

#endif // GRID_CUBE_VERTS_DEFINED


void InitializeGridVerts()
{
    int index = 0;
    for (int i = 0; i <= 10; ++i)
    {
        float t = -0.5f + i * (1.0f / 10.0f);
        gridVerts[index++] = { -0.5f, 0.0f, t, 1.0f };
        gridVerts[index++] = { 0.5f, 0.0f, t, 1.0f };
        gridVerts[index++] = { t, 0.0f, -0.5f, 1.0f };
        gridVerts[index++] = { t, 0.0f,  0.5f, 1.0f };
    }
}

void InitializeCubeVerts()
{
    int i = 0;
    float s = 0.25f;

    // Front
    cubeVertsUV[i++] = { { -s, -s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s, -s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 0.0f, 0.0f };

    // Back
    cubeVertsUV[i++] = { {  s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { { -s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { {  s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { { -s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 0.0f, 0.0f };

    // Left
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { { -s, -s, -s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s,  s,  s, 1.0f }, 0, 0.0f, 0.0f };

    // Right
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s,  s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { {  s,  s, -s, 1.0f }, 0, 0.0f, 0.0f };

    // Top
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s, -s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s,  s, -s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s,  s,  s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s,  s,  s, 1.0f }, 0, 0.0f, 0.0f };

    // Bottom
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s,  s, 1.0f }, 0, 1.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s, -s,  s, 1.0f }, 0, 0.0f, 1.0f };
    cubeVertsUV[i++] = { {  s, -s, -s, 1.0f }, 0, 1.0f, 0.0f };
    cubeVertsUV[i++] = { { -s, -s, -s, 1.0f }, 0, 0.0f, 0.0f };
}

float RandomRange(float min, float max)
{
    return min + ((rand() / (float)RAND_MAX) * (max - min));
}

void InitializeStars()
{
    for (int i = 0; i < 3000; ++i)
    {
        float x = RandomRange(-1.0f, 1.0f);
        float y = RandomRange(-1.0f, 1.0f);
        float z = RandomRange(-1.0f, 1.0f);
        float len = std::sqrt(x * x + y * y + z * z);
        if (len == 0.0f) len = 1.0f;

        x /= len;
        y /= len;
        z /= len;

        // Move stars to 5–20 units away from the camera (within visible range)
        float dist = RandomRange(5.0f, 9.5f); // ensure they're inside the 10.0 far plane
        starField[i].position = { x * dist, y * dist, z * dist, 1.0f };
    }
}

void InitializeStonehengeData()
{
    for (int i = 0; i < 1457; ++i)
    {
        const OBJ_VERT& vert = StoneHenge_data[i];

        Float4WithColor v;
        v.pos.x = vert.pos[0] * 0.1f; // scale position
        v.pos.y = vert.pos[1] * 0.1f;
        v.pos.z = vert.pos[2] * 0.1f;
        v.pos.w = 1.0f;

        v.u = vert.uvw[0]; // don't scale UVs
        v.v = vert.uvw[1];

        v.color = 0; // placeholder, we will use texture

        stoneVerts[i] = v;
    }

    for (int i = 0; i < 2532; ++i)
    {
        stoneIndices[i] = StoneHenge_indicies[i];
    }
}


