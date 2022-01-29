# BadEngine
the worst 3d engine you will ever see


# its a buggy pile of garbage that can only show 1 pixel active


# please dont actually use this, i made this as a side project

the code MOVES THE CAMERA, NOT THE PIXEL.

it is NOT INVERTED.

if you want to make it move the pixel so badly, just change the lines that look like
```
 if (GetAsyncKeyState(key["s"])) {
                cameraPos[1] = cameraPos[1] - 1;
            }
```
to use  `pixelOne` instead of cameraPos
