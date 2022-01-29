#include <iostream> //io
#include <map> // map
#include <string> //string funcs
#include <Windows.h> //keystate
#include <chrono> //sleep

using std::string;
#define XSIZE 10
#define YSIZE 10

int cameraPos[3] = {5,5,5};
//Camera is 5 pixels tall and 5 pixels wide
//
int worldPos[3] = { 0,0,0 };


void clearBuffer()
{
    std::cout << "\x1B[2J\x1B[H";
}


//xzy
double pixelOne[3] = { 1,3,3 }; //meant to be centered
int dimensions[2] = { XSIZE, YSIZE };
int cameraDepth = 5; //only view 5 pixels ahead 
char pixel = '=';
string toColor(int R, int G, int B) { return "\x1B[38;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m"; }
string toBG(int R, int G, int B) { return "\x1B[48;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m"; }

std::map<int, string> depthMap = {
        {0, toColor(0,0,0)},
        {1, toColor(50, 50, 50)},
        {2, toColor(100, 100, 100)},
        {3, toColor(150, 150, 150)},
        {4, toColor(200, 200, 200)},
        {5, toColor(250, 250, 250)}
};




int clamp(int val, int min, int max) {  //used for limiting values to a range of min and max
    if (val <= min) { return min; } // if value is smaller than min, return min
    if (val > max) { return 0; } //if value is larger than max return 0, because it'll be behind cam
    return val; //if it hasnt done any returning then just return value
}


int hide(int val, int min, int max) {  //used for limiting values to a range of min and max
    if (val <= min) { return 0; } // if value is smaller than min, return min
    if (val >= max) { return 0; } //if value is larger than max return 0, because it'll be behind cam
    return val; //if it hasnt done any returning then just return value
}

int renderBasic() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // hide cursor
    SetConsoleCursorInfo(out, &cursorInfo);
    int* camPos = cameraPos;
    double* object = pixelOne;
    int calculateCamOffset[3];//Calculate how far off the camera is from world center
    for (int current = 0; current < 3; current++) {
        calculateCamOffset[current] = camPos[current] - worldPos[current];

    }
    //calculateCamOffset[0] += dimensions[0];
    //calculateCamOffset[2] += dimensions[1];

    int rawDepth = camPos[1] - object[1];
    int clampedDepth = clamp(rawDepth, 1, 5);

    std::map<int, string> depthMap = {
        {0, toBG(0,0,0) + toColor(0,0,0)},
        {1, toBG(50,50,50) + toColor(50, 50, 50)},
        {2, toBG(100,100,100) + toColor(100, 100, 100)},
        {3, toBG(150,150,150) + toColor(150, 150, 150)},
        {4, toBG(200,200,200) + toColor(200, 200, 200)},
        {5, toBG(250,250,250) + toColor(250, 250, 250)}
    };
    //std::cout << "\b\b" << depthMap[clampedDepth] << pixel;
    // 
    char dMap[6500];
    int depthBuffer[XSIZE*YSIZE];
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

    int x = screenSpaceObj[0];
    int depth = screenSpaceObj[1];
    int y = screenSpaceObj[2];
    if (y <= cameraPos[2]) {
        int yDepthMap = (dimensions[0] * y);//lets multiply the y value by the screen x size to reach y pos in depth buffer
        dMap[yDepthMap + x] = std::to_string(depth)[0];
        if (x<0 || x>=XSIZE) {
            dMap[yDepthMap + x] = '0';
        }
        if (y<0 || y>=YSIZE) {
            dMap[yDepthMap + y] = '0';
        }

    }

    
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
    string bufferMap = "";
    for (int j = 0; j < (XSIZE * YSIZE); j++) {
        if (j % XSIZE == 0) {
            bufferMap=bufferMap+'\n';
            std::cout << '\n';
        }
        bufferMap = bufferMap + dMap[j];
        string stringify[1];
        stringify[0] = dMap[j];
        std::cout << depthMap[std::stoi(stringify[0])] << pixel;
    }
    std::cout << toBG(0, 0, 0);
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
    {"w",0x57},
    {"a",0x41},
    {"d",0x44}
};

int main()
{
    for (;;) {
        using namespace std;
        renderBasic();

        if (GetConsoleWindow() == GetForegroundWindow()) {
            if (GetAsyncKeyState(key["s"])) {
                cameraPos[1] = cameraPos[1] - 1;
            }
            if (GetAsyncKeyState(key["w"])) {
                cameraPos[1] = cameraPos[1] + 1;
            }

            if (GetAsyncKeyState(key["a"])) {
                cameraPos[0] = cameraPos[0] + 1;
            }
            if (GetAsyncKeyState(key["d"])) {
                cameraPos[0] = cameraPos[0] - 1;
            }


            if (GetAsyncKeyState(VK_UP)) {
                cameraPos[2] = cameraPos[2] + 1;
            }

            if (GetAsyncKeyState(VK_DOWN)) {
                cameraPos[2] = cameraPos[2] - 1;
            }
        }

        Sleep(50);
        
        clearBuffer();
    }
   


}
