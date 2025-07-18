How to run the defult simulation:
To run the project you can use the you can just open the main.exe file by dubble clicking or right clicking on the file and select oppen (you might have to trust the file for windows to open it).

How to modify the simulation:
If you want to modify the valuse you need c++ 14 or higher and raylib instaled. Then oppen the main.code-workspace to oppen the project in VSCode.

The main values you would want to change are in line 17 "balls[i] = new Ball(randomWidthtOnScreen, randomHeightOnScreen, 10, 10, 0.47f, 0.62f);".
first value is the x position.
second value is the y position.
third value is the radius.
forth value is the mass.
fifht value is the drag coefficient(air resistance).
sixth value it the Elasticity(coefficient of elasticity).

Then when done with your changes save the file with control + s or go to File > Save in the upper Left.
to have the changes show in the simulation you need to compile the project so press F5 or go the Run > Start Debugging.
