


Overview of animate data program
=========
The animate\_data program, animates the data from the main simulations.

WARNING
---------
WARNING, this program is TERRIBLE! on many levels: The program is written in Processing4, a simplified version of Java aimed at beginner developers, Processing4 offers fast and portable coding of 2D graphical application but has poor very performance (compared to C++); In making this program, I have not been very strict about following good OOP design practices, I am just trying to get the display working.
The program is NOT SAFE, It may crash due to numerous uncaught exceptions, if the input files do not follow the correct format.


This is an INTENTIONAL design choice, handling these exceptions are not worth the time it would take to implement, since the inputs should already be safe if it comes from the main program. After all, the keyframe file has been produced by the main program, and the city file need to already have been loaded by the C++ program.


Running the program
--------
To start the program, download the latest version of the Processing4 IDE either from the official website, or your favorite Linux package manager.

Open the file `animate_data\animate_data.pde` in the Processing4 IDE and press the run button (or run the command `processing-java --sketch=/home/.../animate_data  --run` in a Linux commandline, here `...` is the path to the folder with the program in it, you need to pass the folder with the  pde files as the argument, not the pde files themself)

The program will try to read `city.json` and `keyframes.json` from the executing directory (Which is the directory with the pde files, if you use the IDE or the directory your terminal is in if executing from the commandline).


Using the program
--------
This program reads and displays the city, allowing the user to pan around and zoom in with the mouse.

The city is loaded from the same JSON file with nodes and roads as used by the main program (The name of the file to load is hard-coded) (See design\_documents\\city\_data.json.md).

The animation can be played with the spacebar (or by clicking the "play" bar at the bottom of the screen), the animations are loaded from a keyframe.json file, specifically designed for that purpose. This file is descriped in design\_documents\\keyframes.json.md

