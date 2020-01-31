# OpenGL_NeuralTexture

This repository generates **training data** for NeuralTexture(https://github.com/SSRSGJYD/NeuralTexture) using OpenGL. The scene is rendered with Blinn-Phong Light Model, Gamma Correction and shadows.


## Requirements
OpenGL >= 3.0 

CMake

## How to Use
Compile the program
```
cd OpenGL_NeuralTexture
mkdir build
cd build
cmake ..
make
```
Run the program
```
cp lyr_opengl_task1 ..
cd ..
./lyr_opengl_task1
```


## Some problems that may occur

You may need to modify the codes to have different effects.

### Get the training data

In main.cpp: line 37, set **enable_output = true;** line 41-43, set the output directories. 

Recompile and run the program. The **uv**, **camera extrinsic**, and **pixel data**(the rendered scene / each frame) will be outputed to the directories. The runnning program will display a black screen.

The pixel data (frame) is stored in **.png** format. Uv shaped (height, width,2) is stored in **.npy** format.  Camera extrinsics shaped (3) is stored in **.npy**  format.

### Object models

The repository includes two models - **basketball** and **vase** as shown in the folder. To change the model, in main.cpp line 40, set **object_id = 0** for basketball or **object_id = 1** for vase. Then recompile and run the program.

If you want to load your own model, the library included in the repository supports .obj format. You may need to understand how OpenGL works and modify some codes.


### Change resolution
The default resolution of the output training data is 512x384 to achieve a balance between performance and image quality. You may change the resolution in main.cpp line 33-36.

### Black screen

If you want to see the realtime rendered scene, in main.cpp line 37, set **enable_output = false;**.Recompile and run the program. The rendered scene shall be outputed to the screen.

You may wonder why outputing the data will lead to the black screen (the only difference is to set enable_output = true/false). When not outputing data, the pixel data is directly written into screen. When outputing the data, the pixel data, uv, normals etc. are written into video memories and the screen itself displays nothing. 

## Additional Information

This repository only serves to offer training data with OpenGL. It is not intended to be any OpenGL tutorial. Hence, the way to control this program is quite primitive ( nobody likes to go into C code and modify it). Sorry for any related trouble.

If you are not satisfied with the scenes the program produce (such as the **motion of the camera**), you may need to modify more than one line in the program. I recommend that you first understand OpenGL. A good tutorial is https://learnopengl.com/ . Feel free to put up issues and discuss any problems you meet.


## Rendered scene example
![basketball](https://github.com/A-Dying-Pig/OpenGL_NeuralTexture/blob/master/example/basketball.gif )   

![vase](https://github.com/A-Dying-Pig/OpenGL_NeuralTexture/blob/master/example/vase.gif )   



