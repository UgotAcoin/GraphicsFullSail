#pragma once
#include "Shader.h"
#include "StoneHenge_Texture.h"
#include "StoneHenge.h"
#include "greendragon.h"
#include "fire_02.h"
#include "tiles_12.h"
#include "XTime.h"

#include "VertexData.h" 
#include <cstdio>


unsigned int MyAlphaBlend(unsigned int DestinationColor, unsigned int SourceColor)
{
    unsigned int destA = (DestinationColor >> 24) & 0xFF;
    unsigned int destR = (DestinationColor >> 16) & 0xFF;
    unsigned int destG = (DestinationColor >> 8) & 0xFF;
    unsigned int destB = (DestinationColor >> 0) & 0xFF;

    unsigned int srcA = (SourceColor >> 24) & 0xFF;
    unsigned int srcR = (SourceColor >> 16) & 0xFF;
    unsigned int srcG = (SourceColor >> 8) & 0xFF;
    unsigned int srcB = (SourceColor >> 0) & 0xFF;

    float alpha = srcA / 255.0f;

    unsigned int outA = static_cast<unsigned int>(destA * (1.0f - alpha) + srcA * alpha);
    unsigned int outR = static_cast<unsigned int>(destR * (1.0f - alpha) + srcR * alpha);
    unsigned int outG = static_cast<unsigned int>(destG * (1.0f - alpha) + srcG * alpha);
    unsigned int outB = static_cast<unsigned int>(destB * (1.0f - alpha) + srcB * alpha);

    return (outA << 24) | (outR << 16) | (outG << 8) | outB;
}

// Pixel draw
void drawPixel(int x, int y, float z, unsigned int color)
{
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;

    int index = convertDimCoords(x, y);

    // Z-buffer test
    if (z >= DEPTH_BUFFER[index])
        return;

    DEPTH_BUFFER[index] = z;

    if ((color >> 24) == 0xFF) // Alpha is 255 (fully opaque)
    {
        SCREEN_ARRAY[index] = color;
    }
    else
    {
        SCREEN_ARRAY[index] = MyAlphaBlend(SCREEN_ARRAY[index], color);
    }
}

// Draw line using float interpolation
void DrawLine(const Float4WithColor& start, const Float4WithColor& end)
{
    Float4WithColor vStart = start;
    Float4WithColor vEnd = end;

    if (VertexShader)
    {
        VertexShader(vStart.pos);
        VertexShader(vEnd.pos);
    }

    ScreenXY screenStart = NDCtoScreen(vStart.pos);
    ScreenXY screenEnd = NDCtoScreen(vEnd.pos);

    int dx = std::abs(screenEnd.x - screenStart.x);
    int dy = std::abs(screenEnd.y - screenStart.y);
    int steps = max(dx, dy);
    if (steps == 0)
    {
        drawPixel(screenStart.x, screenStart.y, 0.999f, vStart.color);
        return;
    }

    float xStep = (screenEnd.x - screenStart.x) / float(steps);
    float yStep = (screenEnd.y - screenStart.y) / float(steps);

    float x = float(screenStart.x);
    float y = float(screenStart.y);

    for (int i = 0; i <= steps; ++i)
    {
        drawPixel(int(round(x)), int(round(y)), 0.999f, vStart.color);
        x += xStep;
        y += yStep;
    }
}

int RandomNumber100to400()
{
    return rand() % 301 + 100;
}

void blockImageTransfer(int startX, int startY, int endX, int endY, int worldX, int worldY, const unsigned int* array, int arrayWidth)
{
    for (int y = startY; y < endY; ++y)
    {
        for (int x = startX; x < endX; ++x)
        {
            int srcIndex = y * arrayWidth + x;

            if (worldX + (x - startX) < 0 || worldX + (x - startX) >= SCREEN_WIDTH ||
                worldY + (y - startY) < 0 || worldY + (y - startY) >= SCREEN_HEIGHT)
                continue;

            unsigned int color = C2C(array[srcIndex]);
            drawPixel(worldX + (x - startX), worldY + (y - startY), 0.005f, color);
        }
    }
}

void clearScreen()
{
    for (int i = 0; i < NUM_PIXELS; ++i)
    {
        SCREEN_ARRAY[i] = 0xFF000033;
        DEPTH_BUFFER[i] = FLT_MAX;
    }
}

float EdgeFunction(const Float4& a, const Float4& b, const Float4& c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}


bool IsTopLeft(const Float4& a, const Float4& b)
{
    return (a.y == b.y && a.x > b.x) || (a.y > b.y);
}

unsigned int SampleTexture(float u, float v)
{
    u = Clamp(u, 0.0f, 1.0f);
    v = Clamp(v, 0.0f, 1.0f);

    int x = static_cast<int>(u * (greendragon_width - 1));
    int y = static_cast<int>(v * (greendragon_height - 1));
    int index = y * greendragon_width + x;

    return C2C(greendragon_pixels[index]);
}

unsigned int SampleStonehengeTexture(float u, float v)
{
    u = Clamp(u, 0.0f, 1.0f);
    v = Clamp(v, 0.0f, 1.0f);

    int x = static_cast<int>(u * (StoneHenge_width - 1));
    int y = static_cast<int>(v * (StoneHenge_height - 1));
    int index = y * StoneHenge_width + x;

    return C2C(StoneHenge_pixels[index]);
}

unsigned int (*ActiveTextureSampler)(float, float) = SampleStonehengeTexture; // default


void DrawFilledTriangle(const Float4WithColor& v0, const Float4WithColor& v1, const Float4WithColor& v2)
{
    Float4WithColor p0 = v0;
    Float4WithColor p1 = v1;
    Float4WithColor p2 = v2;

    if (VertexShader)
    {
        VertexShader(p0.pos);
        VertexShader(p1.pos);
        VertexShader(p2.pos);
    }

    ScreenXY s0 = NDCtoScreen(p0.pos);
    ScreenXY s1 = NDCtoScreen(p1.pos);
    ScreenXY s2 = NDCtoScreen(p2.pos);

    Float4 f0 = { static_cast<float>(s0.x), static_cast<float>(s0.y), p0.pos.z / p0.pos.w, 1.0f };
    Float4 f1 = { static_cast<float>(s1.x), static_cast<float>(s1.y), p1.pos.z / p1.pos.w, 1.0f };
    Float4 f2 = { static_cast<float>(s2.x), static_cast<float>(s2.y), p2.pos.z / p2.pos.w, 1.0f };

    int minX = MyMinimum(s0.x, s1.x, s2.x);
    int maxX = MyMaximum(s0.x, s1.x, s2.x);
    int minY = MyMinimum(s0.y, s1.y, s2.y);
    int maxY = MyMaximum(s0.y, s1.y, s2.y);

    float area = EdgeFunction(f0, f1, f2);
    if (area == 0.0f) return;

    bool e0TopLeft = IsTopLeft(f1, f2);
    bool e1TopLeft = IsTopLeft(f2, f0);
    bool e2TopLeft = IsTopLeft(f0, f1);

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            Float4 p = { static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, 0, 1 };
            float w0 = EdgeFunction(f1, f2, p);
            float w1 = EdgeFunction(f2, f0, p);
            float w2 = EdgeFunction(f0, f1, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                float invArea = 1.0f / area;
                w0 *= invArea;
                w1 *= invArea;
                w2 *= invArea;

                float z = w0 * f0.z + w1 * f1.z + w2 * f2.z;

                // Perspective-correct UVs
                float w_recip0 = 1.0f / p0.pos.w;
                float w_recip1 = 1.0f / p1.pos.w;
                float w_recip2 = 1.0f / p2.pos.w;

                float u = (w0 * p0.u * w_recip0 + w1 * p1.u * w_recip1 + w2 * p2.u * w_recip2);
                float v = (w0 * p0.v * w_recip0 + w1 * p1.v * w_recip1 + w2 * p2.v * w_recip2);
                float wSum = (w0 * w_recip0 + w1 * w_recip1 + w2 * w_recip2);

                u /= wSum;
                v /= wSum;


                // === Clamp UVs (use Clamp() if C++17 not available) ===
                u = Clamp(u, 0.0f, 1.0f);
                v = Clamp(v, 0.0f, 1.0f);

                // === Sample Texture ===
                unsigned int texColor = ActiveTextureSampler(u, v);

                // Optional: run pixel shader
                if (PixelShader)
                    PixelShader(texColor);

                // Final draw
                drawPixel(x, y, z, texColor);
            }
        }
    }
}

void DrawStarField(Matrix4x4 view, Matrix4x4 proj)
{
    for (int i = 0; i < 3000; ++i)
    {
        Float4 p = starField[i].position;

        // Transform to view and projection space
        p = MatrixVertexMultiply(p, view);
        p = MatrixVertexMultiply(p, proj);

        // Perspective divide
        if (p.w == 0.0f) continue;
        p.x /= p.w;
        p.y /= p.w;
        p.z /= p.w;

        // Clip space check
        if (p.x < -1 || p.x > 1 || p.y < -1 || p.y > 1 || p.z < 0 || p.z > 1)
            continue;

        // Convert to screen space
        ScreenXY screen = NDCtoScreen(p);
        drawPixel(screen.x, screen.y, 0.001f, 0xFFFFFFFF); // test: force near
    }
}

void DrawStonehengeModel()
{
    for (int i = 0; i < 2532; i += 3)
    {
        const Float4WithColor& v0 = stoneVerts[stoneIndices[i + 0]];
        const Float4WithColor& v1 = stoneVerts[stoneIndices[i + 1]];
        const Float4WithColor& v2 = stoneVerts[stoneIndices[i + 2]];

        DrawFilledTriangle(v0, v1, v2);
    }
}

// === DrawGround ===
void DrawGround()
{
    const int segments = 64;
    const float radius = 3.0f;

    Float4WithColor rawCenter = { {0, -0.8f, 0, 1}, 0xFFFFFFFF, 0.5f, 0.5f };



    for (int i = 0; i < segments; ++i)
    {
        float angle0 = (2.0f * 3.14159f * i) / segments;
        float angle1 = (2.0f * 3.14159f * (i + 1)) / segments;

        Float4WithColor rawV0 = {
            { radius * cosf(angle0), -0.8f, radius * sinf(angle0), 1 },
            0xFFFFFFFF,
            0.5f + 0.5f * cosf(angle0),
            0.5f + 0.5f * sinf(angle0)
        };


        Float4WithColor rawV1 = {
            { radius * cosf(angle1), -0.8f, radius * sinf(angle1), 1 },
            0xFFFFFFFF,
            0.5f + 0.5f * cosf(angle1),
            0.5f + 0.5f * sinf(angle1)
        };


        Float4WithColor center = rawCenter;
        Float4WithColor v0 = rawV0;
        Float4WithColor v1 = rawV1;

        if (VertexShader)
        {
            VertexShader(center.pos);
            VertexShader(v0.pos);
            VertexShader(v1.pos);
        }

        if (std::isfinite(center.pos.x) && std::isfinite(v0.pos.x) && std::isfinite(v1.pos.x))
        {
            DrawFilledTriangle(center, v0, v1);
        }
    }
}

void LabOneStuff(XTime& timer, bool& treesInitialized, Tree* treeList, int numOfTrees, int& fireX, int& fireY, double& fireTimeAccumulator, const double fireFrameDuration)
{
    timer.Signal();
    fireTimeAccumulator += timer.Delta();

    if (fireTimeAccumulator >= fireFrameDuration)
    {
        for (int y = 0; y < SCREEN_HEIGHT; y += 30)
        {
            for (int x = 0; x < SCREEN_WIDTH; x += 30)
            {
                blockImageTransfer(290, 130, 320, 160, x, y, tiles_12_pixels, tiles_12_width);
            }
        }

        if (!treesInitialized)
        {
            for (int i = 0; i < numOfTrees; i++)
            {
                treeList[i].x = RandomNumber100to400();
                treeList[i].y = RandomNumber100to400();
            }
            treesInitialized = true;
        }

        for (int i = 0; i < numOfTrees; i++)
        {
            blockImageTransfer(320, 15, 385, 95, treeList[i].x, treeList[i].y, tiles_12_pixels, tiles_12_width);
        }

        blockImageTransfer(
            fireX, fireY,
            fireX + 128, fireY + 128,
            SCREEN_WIDTH / 2 - 64,
            SCREEN_HEIGHT / 2 - 64,
            fire_02_pixels,
            fire_02_width
        );

        fireTimeAccumulator -= fireFrameDuration;
        fireX += 128;
        if (fireX >= fire_02_width)
        {
            fireX = 0;
            fireY += 128;
            if (fireY >= fire_02_height)
                fireY = 0;
        }
    }
}

void LabTwoStuff()
{
    auto ScreenToNDC = [](int x, int y) -> Float4 {
        Float4 f;
        f.x = (2.0f * x / SCREEN_WIDTH) - 1.0f;
        f.y = 1.0f - (2.0f * y / SCREEN_HEIGHT); // invert Y
        f.z = 0.0f;
        f.w = 1.0f;
        return f;
        };

    auto MakeVertex = [&](int x, int y, unsigned int color) -> Float4WithColor {
        return { ScreenToNDC(x, y), color };
        };

    // Red star
    DrawLine(MakeVertex(356, 356, 0xffff0000), MakeVertex(250, 400, 0xffff0000));
    DrawLine(MakeVertex(144, 144, 0xffff0000), MakeVertex(250, 100, 0xffff0000));
    DrawLine(MakeVertex(400, 250, 0xffff0000), MakeVertex(356, 356, 0xffff0000));
    DrawLine(MakeVertex(100, 250, 0xffff0000), MakeVertex(144, 144, 0xffff0000));
    DrawLine(MakeVertex(250, 400, 0xffff0000), MakeVertex(144, 356, 0xffff0000));
    DrawLine(MakeVertex(250, 100, 0xffff0000), MakeVertex(356, 144, 0xffff0000));
    DrawLine(MakeVertex(144, 356, 0xffff0000), MakeVertex(100, 250, 0xffff0000));
    DrawLine(MakeVertex(356, 144, 0xffff0000), MakeVertex(400, 250, 0xffff0000));

    // Extra lines
    DrawLine(MakeVertex(250, 100, 0xff00ff00), MakeVertex(250, 400, 0xff00ff00)); // Green vertical
    DrawLine(MakeVertex(356, 356, 0xff0000ff), MakeVertex(144, 144, 0xff0000ff)); // Blue diagonal
    DrawLine(MakeVertex(144, 356, 0xff00ffff), MakeVertex(356, 144, 0xff00ffff)); // Cyan diagonal
    DrawLine(MakeVertex(400, 250, 0xffffff00), MakeVertex(100, 250, 0xffffff00)); // Yellow horizontal
}

void LabThreeStuff(XTime& timer, Float4* gridVerts, int gridVertCount, Float4WithColor* cubeVertsUV, int cubeVertCount)
{
    timer.Signal();
    float elapsed = static_cast<float>(timer.TotalTime());

    Matrix4x4 rotationY = CreateRotationYMatrix(elapsed);
    Matrix4x4 translation = CreateTranslationMatrix(0.0f, 0.25f, 0.0f);
    Matrix4x4 worldCube;
    MatrixMatrixMultiply(worldCube, rotationY, translation);

    Matrix4x4 viewRotation = CreateRotationXMatrix(18.0f * DEG2RAD);
    Matrix4x4 viewTranslation = CreateTranslationMatrix(0.0f, 0.25f, -1.0f);
    Matrix4x4 viewCombined;
    MatrixMatrixMultiply(viewCombined, viewRotation, viewTranslation);
    Matrix4x4 view = InverseRigidBodyMatrix(viewCombined);

    Matrix4x4 proj = CreatePerspectiveMatrix(90.0f * DEG2RAD, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 10.0f);

    VertexShader = VS_World;
    SV_ViewMatrix = view;
    SV_ProjectionMatrix = proj;

    // Draw grid
    SV_WorldMatrix = CreateIdentityMatrix();
    for (int i = 0; i < gridVertCount; i += 2)
    {
        Float4WithColor v0 = { gridVerts[i], 0xFFFFFFFF };
        Float4WithColor v1 = { gridVerts[i + 1], 0xFFFFFFFF };
        DrawLine(v0, v1);
    }

    // Draw cube
    SV_WorldMatrix = worldCube;
    for (int i = 0; i < cubeVertCount; i += 3)
    {
        DrawFilledTriangle(cubeVertsUV[i], cubeVertsUV[i + 1], cubeVertsUV[i + 2]);
    }

    PixelShader = nullptr;
}

void LabFourStuff(XTime& timer)
{
    static bool starsInitialized = false;
    static bool modelInitialized = false;
    static bool groundInitialized = false;

    if (!starsInitialized)
    {
        InitializeStars();
        starsInitialized = true;
    }

    if (!modelInitialized)
    {
        InitializeStonehengeData();
        modelInitialized = true;
    }

    timer.Signal();

    // === Fixed Camera Looking at Origin ===
    Matrix4x4 viewRotation = CreateRotationXMatrix(18.0f * DEG2RAD); // Slight downward tilt
    Matrix4x4 viewTranslation = CreateTranslationMatrix(0.0f, 0.25f, -3.5f); // Pull back to fit model

    Matrix4x4 viewCombined;
    MatrixMatrixMultiply(viewCombined, viewRotation, viewTranslation);
    Matrix4x4 view = InverseRigidBodyMatrix(viewCombined);

    Matrix4x4 proj = CreatePerspectiveMatrix(
        90.0f * DEG2RAD,
        static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT,
        0.1f,
        100.0f
    );

    VertexShader = VS_World;
    PixelShader = nullptr;

    SV_ViewMatrix = view;
    SV_ProjectionMatrix = proj;

    // === Draw Background Starfield ===
    SV_WorldMatrix = CreateIdentityMatrix();
    DrawStarField(view, proj);

    // === Draw Ground ===
    SV_WorldMatrix = CreateTranslationMatrix(0.0f, 0.0f, -0.9f);
    DrawGround();

    // === Draw Stonehenge ===
    Matrix4x4 stoneTranslation = CreateTranslationMatrix(0.0f, -0.8f, -0.9f);
    Matrix4x4 scale = CreateScaleMatrix(0.9f, 0.9f, 0.9f);
    Matrix4x4 worldCombined;
    MatrixMatrixMultiply(worldCombined, scale, stoneTranslation); // Scale * Translate
    SV_WorldMatrix = worldCombined;

    DrawStonehengeModel();

}
