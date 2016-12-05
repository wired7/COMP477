What is it?
------------
The goal of this project was to create an application which simulates the movement and interaction of objects with fluids. 
The simulation of the fluid was done by using a computational method called Smoothed-Particle Hydrodynamics (SPH). 
We used C++ and the modern OpenGL library (4.0+) with GLSL shader language to develop our project. 

As for the computational method for the simulation of fluids, we did not use any external libraries to build our particle system. 
We used SPH as our reference.

Authors
-------
Andres Gonzalez-27753675,
Dexter Ho-Yiu Kwok-27709110,
Jonathan De Marco-26941249
David Katchouni-26461891

Libraries Used
--------------
Freetype (https://www.freetype.org/) for text rendering 
GLFW (http://www.glfw.org/) for window creation and input handling
GLM for math functions (http://glm.g-truc.net/0.9.8/index.html)
Simple OpenGL Image Library (http://www.lonesock.net/soil.html) for reading from an image 
GLEW (http://glew.sourceforge.net/) for OpenGL extensions

INSTALLATION
------------

Depending on the version of Visual Studio you might use to compile the source code,
different lib files will be needed. The two libraries affected by this are GLFW and
Freetype. For GLFW, you must change the include directory to include $(SolutionDir)\Resources\GLFW\lib-vc2015
or $(SolutionDir)\Resources\GLFW\lib-vc2013, depending on whether you are using Visual Studio 2013 or 2015.
For Freetype, the one included in the project is for Visual Studio 2013. If you are using Visual Studio 2015,
we have include a lib file in Resources/Freetype/2015 where you can find the lib file Freetype needs
for Visual Studio 2015. Replace the Freetype lib file in both the Release folder and the one found in
$(SolutionDir)\Resources\Freetype\lib. Make sure you are running Visual Studio in Release 32bit (x86).




