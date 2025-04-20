#pragma once
#include "Defines.h"
#include "MyMath.h"
#include "Shader.h"
#include "fire_02.h"
#include "tiles_12.h"
#include "XTime.h"

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
void drawPixel(int x, int y, unsigned int color) 
{
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;

    int index = y * SCREEN_WIDTH + x;
    SCREEN_ARRAY[index] = MyAlphaBlend(SCREEN_ARRAY[index], color);
}

// Draw line using float interpolation
void DrawLine(int x1, int y1, int x2, int y2, unsigned int color)
{
    float dx = static_cast<float>(x2 - x1);
    float dy = static_cast<float>(y2 - y1);

    int steps = static_cast<int>(max(std::abs(x2 - x1), std::abs(y2 - y1)));
    if (steps == 0) 
    {
        drawPixel(x1, y1, color);
        return;
    }

    for (int i = 0; i <= steps; ++i)
    {
        float t = static_cast<float>(i) / steps;
        float x = (1.0f - t) * x1 + t * x2;
        float y = (1.0f - t) * y1 + t * y2;

        drawPixel(static_cast<int>(std::round(x)), static_cast<int>(std::round(y)), color);
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
            drawPixel(worldX + (x - startX), worldY + (y - startY), color);
        }
    }
}

void clearScreen() 
{
    for (int i = 0; i < NUM_PIXELS; ++i)
        SCREEN_ARRAY[i] = 0xFF000000; 
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
    DrawLine(356, 356, 250, 400, 0xffff0000);
    DrawLine(144, 144, 250, 100, 0xffff0000);
    DrawLine(400, 250, 356, 356, 0xffff0000);
    DrawLine(100, 250, 144, 144, 0xffff0000);
    DrawLine(250, 400, 144, 356, 0xffff0000);
    DrawLine(250, 100, 356, 144, 0xffff0000);
    DrawLine(144, 356, 100, 250, 0xffff0000);
    DrawLine(356, 144, 400, 250, 0xffff0000);

    DrawLine(250, 100, 250, 400, 0xff00ff00);
    DrawLine(356, 356, 144, 144, 0xff0000ff);
    DrawLine(144, 356, 356, 144, 0xff00ffff);
    DrawLine(400, 250, 100, 250, 0xffffff00);
}

void LabThreeStuff(XTime& timer, Float4* gridVerts, int gridVertCount, Float4* cubeVerts, int cubeVertCount)
{
    timer.Signal();
    float elapsed = static_cast<float>(timer.TotalTime());

    //  === World Matrix for Grid ===
    Matrix4x4 worldGrid = CreateIdentityMatrix();

    // === World Matrix for Cube ===
    Matrix4x4 rotationY = CreateRotationYMatrix(elapsed);
    Matrix4x4 translation = CreateTranslationMatrix(0.0f, 0.25f, 0.0f);
    Matrix4x4 worldCube;
    MatrixMatrixMultiply(worldCube, rotationY, translation);

    // === View Matrix ===
    Matrix4x4 viewRotation = CreateRotationXMatrix(18.0f * DEG2RAD);
    Matrix4x4 viewTranslation = CreateTranslationMatrix(0.0f, 0.0f, -1.0f);
    Matrix4x4 viewCombined;
    MatrixMatrixMultiply(viewCombined, viewTranslation, viewRotation); 
    Matrix4x4 view = InverseRigidBodyMatrix(viewCombined);

    // === Projection Matrix ===
    Matrix4x4 proj = CreatePerspectiveMatrix(
        90.0f * DEG2RAD,
        (float)SCREEN_WIDTH / SCREEN_HEIGHT,
        0.1f,
        10.0f
    );

    // === DRAW GRID ===
    for (int i = 0; i < gridVertCount; i += 2) 
    {
        Float4 a = MatrixVertexMultiply(gridVerts[i], worldGrid);
        Float4 b = MatrixVertexMultiply(gridVerts[i + 1], worldGrid);

        a = MatrixVertexMultiply(a, view);
        b = MatrixVertexMultiply(b, view);

        a = MatrixVertexMultiply(a, proj);
        b = MatrixVertexMultiply(b, proj);

        a.x /= a.w;
        a.y /= a.w;
        a.z /= a.w;
        a.w = 1.0f;

        b.x /= b.w;
        b.y /= b.w;
        b.z /= b.w;
        b.w = 1.0f;

        ScreenXY p0 = NDCtoScreen(a);
        ScreenXY p1 = NDCtoScreen(b);
        DrawLine(p0.x, p0.y, p1.x, p1.y, 0xFFFFFFFF);
    }


    // === DRAW CUBE ===
    for (int i = 0; i < cubeVertCount; i += 2) 
    {
        Float4 a = MatrixVertexMultiply(cubeVerts[i], worldCube);
        Float4 b = MatrixVertexMultiply(cubeVerts[i + 1], worldCube);

        a = MatrixVertexMultiply(a, view);
        b = MatrixVertexMultiply(b, view);

        a = MatrixVertexMultiply(a, proj);
        b = MatrixVertexMultiply(b, proj);

        a.x /= a.w;
        a.y /= a.w;
        a.z /= a.w;
        a.w = 1.0f;

        b.x /= b.w;
        b.y /= b.w;
        b.z /= b.w;
        b.w = 1.0f;

        ScreenXY p0 = NDCtoScreen(a);
        ScreenXY p1 = NDCtoScreen(b);
        DrawLine(p0.x, p0.y, p1.x, p1.y, 0xFF00FF00); 
    }
}


