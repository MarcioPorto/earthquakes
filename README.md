# Earthquakes Homework

Charles Park & Marcio Porto

COMP 394 - Interactive Graphics - Homework 1 - Earthquakes

## Overview

Using C++ and G3D, we implemented a three-dimensional Earth, which is textured with a high resolution image taken by NASA. Then we used an earthquake dataset that includes 13,540 different earthquakes to display them on top of the Earth in their corresponding place. The colors of the representation of the earthquakes reflect their magnitude.

Here is a more detailed description of what we did for each step:

1 - Convert spherical to Cartesian coordinates

We used the following formula to return the correspondent x, y and z positions given a longitude and latitude as input:

x position = cos(lon)*cos(lat-pi/2)
y position = sin(lat-pi/2)
z position = -sin(lon)*cos(lat-pi/2)

Together, they form our variable called location:

location = Vector3(x position, y position, z position)  

2 - Tessellate a sphere

We used a nested for loop to tessellate a unit sphere. The first for loop was in charge of the latitude, where it iterated from 0 to pi. The second for loop was in charge of the longitude, where it iterated from 0 to 2pi. Each iteration is divided by the magnitude of the stacks and slices. This would translate into a more precise sphere by calculating more points.

3. Texture-map a sphere

We created an algorithm that figures out the ordering of the CPU indices. This algorithm would also account for the correct ordering of the indices, utilizing the right hand rule or having counterclockwise orientation. This algorithm was very similar to the nested for loop used for tessellating a sphere, however each index is paired with two neighboring indices. In order to determine which neighboring indices, we kept track of a counter. If the counter is even, each index is paired with an index above it and an index to the right of it. If the counter is odd, each index is paired with an index diagonal to the upper-left and an index above it. All of these little instances made the process of texturing much harder than tessellating. Debugging was also extremely hard because of the inability to see graphical mistakes.

4. Display earthquakes

In order to display the earthquakes appropriately, we divided them in three categories based on their magnitude. If the magnitude was between 0 and 6.5, we considered it a low or medium-low magnitude and represented it on the Earth with the color yellow, If it was between 6.5 and 7.5 we considered it to be of medium magnitude, represented by the color orange and anything  higher than that was of medium-high or high magnitude, represented by the color red. The sizes of the earthquakes on the Earth also depend on their magnitude. Red earthquakes have the highest size, followed by orange and yellow.  

5. Rotate world 3x3 matrix

We implemented rotation based on the motion (clicking and dragging) of the mouse. For example, if you click and drag the Earth down, you would move the Earth so that you visualize the upper portion of the position you were seeing before. We also made it so that at the beginning of our simulation, the north pole is at the top of the window.
