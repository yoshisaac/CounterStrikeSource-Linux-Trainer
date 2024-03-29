# What is this?
This is a program that is intended to automate tasks such as movement, and display information to us that wasn't originally intended to be shown. In a nutshell, this is a video game hack/cheat.

# How does it work?
It externally (as a seperate program from the video game) reads and writes memory using system calls that the Linux kernel graciously gives us. Those system calls include functions like `processes_vm_readv()` and `process_vm_writev()`. 
  
As a way to render information inside a window to the screen, it uses OpenGL, the Open Graphics Library. To create windows, it also uses the X11 protocol. That can be a problem if you are using a display server/client such as Wayland or its variants, so make sure your desktop environment/floating window manager is set to a Xorg session. There may also be other issues your window manager or desktop environment can cause, such as overriding placement of windows.

# How to compile
  
### 1.
Install the necessary packages.  
  
Debian/Ubuntu/LinuxMint:  
`sudo apt install libglu1-mesa-dev freeglut3-dev mesa-common-dev libx11-dev gcc g++`  
If these are found to be incorrect, then contact me.  
  
### 2.
Go to the root directory of the project in your terminal (where main.cpp is located), and run:  
`g++ main.cpp hacks/bhop.cpp hacks/playerInfo.cpp client/client.cpp -o hack -L/usr/X11/lib -lX11 -lXfixes -lGL -lGLU -lstdc++ -Wno-int-to-pointer-cast`  
  
This is very archaic, but hopefully in the future I will use make, cmake, or something similar.

# How to use
After compilation, there will be a program created in the root directory of the project called "`hack`"  
To run the software, go to the root directory of the project and type `sudo ./hack` in your terminal. What this does is it runs the software as `root`, which is required for reading and writing memory on an external level.  

The only distribution I have tested (and currently use) is Linux Mint 21.3

# Features:
- Bunny hop (automatically jump after hitting the ground)  
- Player ESP

# Is this finished?
No, it is very incomplete, and will change a lot over time. The program is inefficient, and commented out code is all over the place. Features such as ESP are very incomplete but work on a crude level.

# TODO:
Ordered by importance  
- Render differently or ignore dormant (non networked) players
- More visual features (boxes around players, display player health, name, etc)
- Fix ESP flickering in fullscreen
- Automatic aiming  
- Fix the X window from blocking all mouse clicks  
