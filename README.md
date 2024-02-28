# What is this?
This is a program that is intended to automate tasks such as movement, and display information to us that wasn't originally intended to be shown. In a nutshell, this is a video game hack/cheat.

# How does it work?
It externally (as a seperate program from the video game) reads and writes memory using system calls that the Linux kernel graciously gives us. Those system calls include functions like `processes_vm_readv()` and `process_vm_writev()`. 
  
As a way to render information inside a window to the screen, it uses OpenGL, the Open Graphics Library. To create windows, it also uses the X11 protocol. That can be a problem if you are using a display server/client such as Wayland or its variants, so make sure your desktop environment/floating window manager is set to a Xorg session. There may also be other issues your window manager or desktop environment can cause, such as over riding placement of windows.

# How to compile
Go to the root directory of the project in your terminal (where main.cpp is located), and run:  
`g++ main.cpp hacks/bhop.cpp hacks/playerInfo.cpp client/client.cpp -o hack -L/usr/X11/lib -lX11 -lXfixes -lGL -lGLU -lstdc++ -Wno-int-to-pointer-cast`  
  
This is very archaic, but hopefully in the future I will use make, cmake, or something similar.

# How to use
After compilation, there will be a program created in the root directory of the project called "`hack`"  
To run the software, go to the root directory of the project and type `sudo ./hack` in your terminal. What this does is it runs the software as `root`, which is required for reading and writing memory on an external level.

# Features:
- Bunny hop (automatically jump after hitting the ground)  
- Player ESP

# Is this finished?
No, it is very incomplete, and will change a lot over time. The program is inefficient, and commented out code is all over the place. Features such as ESP are very incomplete but work on a crude level.

# TODO:
Ordered by importance  
- Fix the X window from blocking all mouse clicks
- More visual features  
- Ignore dormant (non networked) players  
- Render differently or ignore players of the same team  
- Automatic aiming  
