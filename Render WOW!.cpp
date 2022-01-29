#include <iostream> //io
#include <map> //depth map
#include <string> //string funcs
#include <algorithm> //for clamping
#include <Windows.h> //keystate
#include <chrono> //sleep
#include <cmath> // round()

#define XSIZE 5
#define YSIZE 5

int cameraPos[3] = {5,5,5};
int worldPos[3] = { 0,0,0 };
double pixelOne[3] = { 2,3,2 };
int dimensions[2] = { 5,5 };
int cameraDepth = 5; //only view 5 pixels ahead 
char pixel = (char)254u;
using std::string;

int clamp(int val, int min, int max) {  //used for limiting values to a range of min and max
    if (val <= min) { return min; } // if value is smaller than min, return min
    if (val > max) { return 0; } //if value is larger than max return 0, because it'll be behind cam
    return val; //if it hasnt done any returning then just return value
}

string toColor(int R, int G, int B) {return "\x1B[38;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m";}

string toBG(int R, int G, int B) {return "\x1B[48;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m";}

int renderBasic() {
    int* camPos = cameraPos;
    double* object = pixelOne;
    int calculateCamOffset[3];//Calculate how far off the camera is from world center
    for (int current = 0; current < 3; current++) {
        calculateCamOffset[current] = camPos[current] - worldPos[current];
    }

    int rawDepth = camPos[1] - object[1];
    int clampedDepth = clamp(rawDepth, 1, 5);

    std::map<int, string> depthMap = {
        {0, toColor(0,0,0)},
        {1, toColor(50, 50, 50)},
        {2, toColor(100, 100, 100)},
        {3, toColor(150, 150, 150)},
        {4, toColor(200, 200, 200)},
        {5, toColor(250, 250, 250)}
    };
    //std::cout << "\b\b" << depthMap[clampedDepth] << pixel;
    // 
    char dMap[6500];
    int depthBuffer[XSIZE*YSIZE];
    std::cout << sizeof depthBuffer<<std::endl;
    int rowCount = YSIZE;

    // begin using depthmap
   
    for (int i = 0; i < sizeof depthBuffer; i++) {
        dMap[i] = '0'; //fill depth map with 0
    }
    
    int screenSpaceObj[3];
    for (int i = 0; i < 3; i++) {
        screenSpaceObj[i] = camPos[i] - object[i];
    }


    screenSpaceObj[1] = clamp(screenSpaceObj[1], 1, 5);

    int x = screenSpaceObj[0]-1;
    int depth = screenSpaceObj[1];
    int y = screenSpaceObj[2];

    int yDepthMap = (dimensions[0] * (y - 1)-1);//lets multiply the y value by the screen x size to reach y pos in depth buffer

    /*
    Okay, what in Gods green Earth have I written?
    here's what's happening:

    depthBuffer is a massive list of 0s
    a small part of it : 000000000000000

    lets split it up into bits of 5

    00000 00000 00000

    the x size is 5 in this case

    if our y position is 3, then:

    3x5=15

    0000000000000000000000000000

    the 3rd byte ends at position 15 of that string, meaning the end of the y3 section is at position 15

    if we use the formula i made:
    5x(3-1)
    then we get
    5x2
    so we end up with 10

    lets mark position 10 of the string with 1
    00000 00000 10000
    -- Computers count from 0, so [10] is at 11
    the formula gives the start of the 3rd Y coordinate in the depth buffer, so we just add X to get the position of X and Y in the depth buffer
    */
    
    dMap[yDepthMap + x] = std::to_string(depth)[0];
    if (x<0 || x>XSIZE) {
        dMap[yDepthMap + x] = '0';
    }
    if (y<0 || y>YSIZE) {
        dMap[yDepthMap + y] = '0';
    }

    for (int j = 0; j < (XSIZE * YSIZE); j++) {
        if (j % 5 == 0) {
            std::cout << '\n';
        }
        std::cout << dMap[j];
    }
    
    return 1;
}

//20,20,20 slightly shows
//50,50,50 gray 5depth
//100,100,100 visible enough, light gray 4depth
//150, 150, 150 darker white 3depth
//200,200,200 visibly darker than 250 2depth
//250,250,250 1depth

std::map<string, int> key = {
    {"s",0x53},
    {"w",0x57}
};

int main()
{
    /*
    for (;;) {
        using namespace std;
        renderBasic();
        if (GetAsyncKeyState(key["s"])) {
            cameraPos[1] = cameraPos[1]-1; 
        }
        if (GetAsyncKeyState(key["w"])) {
            cameraPos[1] = cameraPos[1] + 1;
        }
        Sleep(100);
    }
    */
    // the commented region here is for when i get it working

    renderBasic();

}
