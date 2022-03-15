#include <iostream> //io
#include <map> // map
#include <string> //string funcs
#include <Windows.h>
#include <vector> //Vectors
#include <conio.h> //screen clear because the ansi escape for clearing stopped
const int XSIZE = 20; 
const int YSIZE = 10;
using std::string;
int cameraPos[3] = { 5,5,5 };
int worldPos[3] = { 0,0,0 };
std::vector<int> ObjectOne = { 5,2,5 };
std::map<int, std::vector<int>> pixelList = {
    {0,{5,2,5}},
    {1,{7,2,5}}
}; //Meant to be centered
std::vector<int>* createPixel(int x, int y, int z) {
    const int constSize = pixelList.size();
    std::vector<int> mainVec = { x,z,y };
    return &(pixelList[constSize] = mainVec);
}
int dimensions[2] = { XSIZE, YSIZE };
int cameraDepth = 5; //only view 5 pixels ahead 
char pixel = '!';
string toColor(int R, int G, int B) { return "\x1B[38;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m"; }
string toBG(int R, int G, int B) { return "\x1B[48;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m"; }
void clearBuffer() //https://www.cplusplus.com/articles/4z18T05o/
{
    HANDLE hStdOut; CONSOLE_SCREEN_BUFFER_INFO csbi; DWORD count;
    DWORD cellCount;
    COORD homeCoords = { 0, 0 };

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    //cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    //if (!FillConsoleOutputCharacter(hStdOut,(TCHAR)' ',cellCount,homeCoords,&count)) return;
    //if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count)) { std::cout << toBG(0, 0, 0); return; };

    /* Move the cursor to start */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}
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
std::vector<int> screenSpace(std::vector<int> object) {
    std::vector<int> screenSpaceObj = { 0,0,0 };


    screenSpaceObj[0] = (cameraPos[0] - object[0]) + (int)(dimensions[0] / 2);
    screenSpaceObj[2] = (cameraPos[2] - object[2]) + (int)(dimensions[1] / 2);
    screenSpaceObj[1] = clamp((cameraPos[1] - object[1]), 1, cameraDepth);

    return screenSpaceObj;
}
int renderBasic(bool clear, bool debugger) {
    char dMap[6500];
    int depthBuffer[XSIZE * YSIZE];
    int rowCount = YSIZE;

    // begin using depthmap

    for (int i = 0; i < sizeof depthBuffer; i++) {
        dMap[i] = '0'; //fill depth map with 0
    }


    std::map<int, std::vector<int>> relativepixelList;
    
    for (int i = 0; i <= pixelList.size()-1; i++) {

        std::vector<int> object = pixelList[i];
        //check if closest pixel




        if(debugger)std::cout << object[1];
        //int clampedDepth = clamp((int)camPos[1] - object[1], 1, cameraDepth);
        std::vector<int> screenSpaceObj = screenSpace(object);
        relativepixelList[i] = screenSpaceObj;
        int x = screenSpaceObj[0];
        int depth = screenSpaceObj[1];
        
        int y = screenSpaceObj[2];
        if (y <= YSIZE) {
            int yDepthMap = (dimensions[0] * y);//lets multiply the y value by the screen x size to reach y pos in depth buffer

            //OVERLAP FIX
            if (depth > (dMap[yDepthMap + x] - '0')) {
                //Thats all it took...
                dMap[yDepthMap + x] = std::to_string(depth)[0];

                if (x < 0 || x >= XSIZE) {
                    dMap[yDepthMap + x] = '0';
                }
                if (y < 0 || y >= YSIZE) {
                    dMap[yDepthMap + y] = '0';
                }
            }

        }

    }
    string bufferMap = "";
    //if (clear) {
    //    clearBuffer();
    //}
    //Clearing before the for loop creates a few milliseconds of an empty frame, causing flickering thats more severe than how it currently is
    std::map<int, string> depthMap = {
        {0, toBG(10,10,10) + toColor(10,10,10)},
        {1, toBG(50,50,50) + toColor(50, 50, 50)},
        {2, toBG(100,100,100) + toColor(100, 100, 100)},
        {3, toBG(150,150,150) + toColor(150, 150, 150)},
        {4, toBG(200,200,200) + toColor(200, 200, 200)},
        {5, toBG(250,250,250) + toColor(250, 250, 250)}
    };

    for (int j = 0; j < (XSIZE * YSIZE); j++) {

        string stringify[1];
        stringify[0] = dMap[j];

        if (j % XSIZE == 0 && j != 0) {  //j != 0 stops it from making a newline on line 1
            bufferMap = bufferMap + '\n';
            //std::cout << '\n';
        }
        bufferMap = bufferMap + depthMap[std::stoi(stringify[0])] + pixel;
    }
    
    if (clear)clearBuffer();
    
    
    std::cout << bufferMap;
    std::cout << toBG(0, 0, 0);

    return 1;
}

std::map<string, int> key = {
    {"s",0x53},
    {"w",0x57},
    {"a",0x41},
    {"d",0x44}
};

int main()
{

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return 1;
    HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD center = { csbi.dwSize.X/2, 10 };
    DWORD consoleMode;
    if (GetConsoleMode(outputHandle, &consoleMode))
    {
        SetConsoleMode(outputHandle, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(outputHandle, &cursorInfo);
        cursorInfo.bVisible = false; // hide crappy terminal cursor
        SetConsoleCursorInfo(outputHandle, &cursorInfo);

    }
    SetConsoleCursorPosition(hStdOut, center);
    std::cout << toColor(255,255,255) << "This is a crappy game!\n";
    SetConsoleCursorPosition(hStdOut, { 0, 11 });
    std::cout << "[press enter]\n";
    std::cout << toColor(255, 255, 255) << "|It uses a render engine, and has no actual game engine. It only exists to be mini showcase of what the program does|";
    getchar();
    std::cout << toColor(255, 255, 255) << "|Arrow keys will make the square move in the selected direction|\n" <<
        "|W Key moves the square further from the cam, S moves it closer|\n" <<
        "|A and D keys move it sideways, like the left and right arrow  |\n";
    getchar();


    std::cout << toColor(255, 0, 0) << "Have !FUN! :)\n";
    //https://www.cplusplus.com/articles/4z18T05o/
    
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = { 0, 0 };

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))return 0;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count);

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count)) { std::cout << toBG(0, 0, 0);};

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
    Sleep(600);
    std::vector<int> ssPos = screenSpace({ 7,1,7 });
    createPixel(7, 7, 2);
    createPixel(5, 7, 2);

    //createPixel(ssPos[0]+2, 2, ssPos[2]);

    
    


    std::map<std::string, int> bounds = {
        {"up",10},
        {"down",1},
        {"left",15},
        {"right", -4}
    };
    
    //Keep in mind that this is a render engine with a game demo.
    //It's not a game engine, so bounds checking is implemented in int main(). It's not meant to be scaled, it's a game demo, not an actual game.
    std::vector<int> numbers = screenSpace({ 3,3,3 });
    std::vector<int>* object = createPixel(6,2,6);
    for (;;) {
        using namespace std;
        int* camPos = cameraPos;


        renderBasic(true, false);
        
        if (GetConsoleWindow() == GetForegroundWindow()) {
            if (GetAsyncKeyState(key["s"])) {
                (*object)[1] = (*object)[1] - 1;
            }
            if (GetAsyncKeyState(key["w"])) {
                (*object)[1] = (*object)[1] + 1;
            }
            if (GetAsyncKeyState(key["a"])) {
                if ((*object)[0] < bounds["left"]) {
                    (*object)[0] = (*object)[0] + 1; 
                }
            }
            if (GetAsyncKeyState(key["d"])) {
                if ((*object)[0] > bounds["right"]) {
                    (*object)[0] = (*object)[0] - 1;
                }
            }


            if (GetAsyncKeyState(VK_UP)) {
                if ((*object)[2] < bounds["up"]) {
                    (*object)[2] = (*object)[2] + 1;
                }
            }

            if (GetAsyncKeyState(VK_DOWN)) {
                if ((*object)[2] > bounds["down"]) {
                    (*object)[2] = (*object)[2] - 1;
                }
            }
            if (GetAsyncKeyState(VK_LEFT)) {
                if ((*object)[0] < bounds["left"]) {
                    (*object)[0] = (*object)[0] + 1;
                }
            }
            if (GetAsyncKeyState(VK_RIGHT)) {
                if ((*object)[0] > bounds["right"]) {
                    (*object)[0] = (*object)[0] - 1;
                }
            }
        }

        Sleep(15);

    } 
    //-If you intend on actually using this library i recommend you get the clearBuffer thing to work without the flickering
    // 
    // +NOTE FROM ME!!!
    // 
    // +I was just sitting until it finally hit me: Why clear the buffer when instead, i can move the cursor to the start and overwrite the previous frame with the new one? THis would result in ZERO empty frames
    // +Its much smoother now, and no more flickering
    // +clearBuffer is now just a function to move the mouse to the start of the screen
    // +super DUPER! smart
    //
    //
    return 0;
}

