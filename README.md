# BadEngine
It looks like 2D, and you'd be right!\
3D stuff use a system with triangles and vertices\
The only thing that makes this "3D" is that it uses a 3 Dimensional coordinate system

What **ISNT** 3D, is that in 3D, you're meant to see **Height**, **Width** and **Thickness**\
In here, you can only see the Height and Width


Think of this as a basic screen output thing that uses a 3D coordinate system rather than an actual render engine

To create a pixel, use:\
`std::vector<int>* myPixel = createPixel(x,depth,y);`\
(With `myPixel` being whatever you want to name it and x,depth,y being the... well, the x, depth and y)\
to move that pixel around, use\
`(*myPixel)[0 for x, 1 for depth, 2 for y]=(*mypixel)[the same index as before]+[step]`\
(+= / -= doesnt work properly with pointers, for some reason)\
If you want to know the basics of the coordinate system, refer to `int main` in the GetAsyncKeyState


# TODO
Pixels with lower depth should be hidden when there is another pixel on top of it thats closer to the camera\
Due to how I added support for more than one pixel, it only places the pixel based on its order in the map of vectors, not in the order of the vector depth
