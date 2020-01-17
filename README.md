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

In main.cpp line 37, set **enable_output = true;** and in the line 41-43, set the output directories. Recompile and run the program. The uv, normal, and pixel data(the rendered scene) will be outputed to the directories. The runnning program will display a black screen.

The pixel data is stored in .png format. uv and normal are stored in .npz format. 


### Change resolution
The default resolution of the output training data is 512x384 to achieve a balance between performance and image quality. You may change the resolution in main.cpp line 33-36.

### Black screen

If you want to see the realtime rendered scene, in main.cpp line 37, set **enable_output = false;**.Recompile and run the program. The rendered scene shall be outputed to the screen.



You may wonder why outputing the data will lead to the black screen (the only difference is to set enable_output = true/false). When not outputing data, the pixel data is directly written into screen. When outputing the data, the pixel data, uv, normals are written into video memories and the screen itself displays nothing. 


### Object models

The repository includes two models - basketball and vase as shown in the folder. You can directly use these models.

If you want to load your own model, the library included in the repository supports .obj format.


## Rendered scene example
![basketball](https://github.com/A-Dying-Pig/OpenGL_NeuralTexture/blob/master/example/basketball.gif )   

![vase](https://github.com/A-Dying-Pig/OpenGL_NeuralTexture/blob/master/example/vase.gif )   



