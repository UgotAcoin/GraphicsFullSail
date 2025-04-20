#include "RasterSurface.h"
#include "Rasterization_Functions.h"
#include "VertexData.h" 
#include "XTime.h"
#include <ctime>

const int numOfTrees = 10;
Tree treeList[numOfTrees];
bool treesInitialized = false;
int fireX = 0, fireY = 0;
double fireTimeAccumulator = 0.0;
const double fireFrameDuration = 1.0 / 30.0;

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    RS_Initialize("Joshua Tomasso", SCREEN_WIDTH, SCREEN_HEIGHT);

    XTime timer;
    timer.Restart();

    InitializeGridVerts();
    InitializeCubeVerts();

    do 
    {
        clearScreen(); // Move outside loop for LabOneStuff();

        //LabOneStuff(timer, treesInitialized, treeList, 10, fireX, fireY, fireTimeAccumulator, fireFrameDuration);
        //LabTwoStuff();
        LabThreeStuff(timer, gridVerts, 44, cubeVerts, 24);

    } while (RS_Update(SCREEN_ARRAY, NUM_PIXELS));

    RS_Shutdown();
    return 0;
}
