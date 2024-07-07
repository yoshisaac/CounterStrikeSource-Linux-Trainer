# What is this?
This is a program intended to automate tasks such as movement, and display information to us that wasn't originally intended to be shown. In a nutshell, this is a video game hack/cheat.

# How does it work?
It externally (as a seperate program from the video game) reads and writes memory using system calls that the Linux kernel graciously gives us. Those system calls include functions like `processes_vm_readv()` and `process_vm_writev()`. 
  
To display a window and draw information inside of it, the program uses the X11 protocol. If you are using Wayland, then there is a chance Xwayland will not handle it correctly. There may also be other issues your window manager or desktop environment can cause, such as overriding placement of windows, or preventing programs from having transparent backgrounds.

# Does my distro or DE work with this?
Most likely. Any problems you will have are primarily going to be the desktop environment's fault, or your distro will not package the required dependencies. Here is a list of desktops and distributions that are known to work.  
  
Distributions:  
- Arch Linux  
- Linux Mint  
  
Desktops:  
- Cinnamon (X11 only)  
- KDE Plasma (Wayland only)  
- Gnome (X11, Wayland untested)  
- Xfce4 (X11, does not support Wayland yet)  
- Dwm (X11, does not support Wayland)  

# How to compile
  
### Installing dependencies
Install the necessary packages.  
  
Debian/Ubuntu/LinuxMint:

```console
$ sudo apt install mesa-common-dev libx11-dev libxfixes-dev libxcomposite-dev gcc g++ make libgtk-3-dev:i386
```
ArchLinux:  

```console
$ sudo pacman -S base-devel xorg-fonts-misc lib32-gtk3
```

If these are found to be incorrect, then contact me.  
  
### Compiling
Go to the root directory of the project in your terminal (where main.cpp is located), and run:  

```console
$ make
...
```

# How to use
After compilation, there will be a program created in the root directory of the project called `cs-source-hack`.

To run the software, go to the root directory of the project and type `sudo ./cs-source-hack` in your terminal. What this does is it runs the software as `root`, which is required for reading and writing memory on an external level.  

# Features
- Bunny hop (automatically jump after hitting the ground)  
- Player ESP  
  * Box  
  * Name  
  * Health  
- Aimbot (Hold left ALT)  

![Screenshot of ESP](https://r2.e-z.host/bb3dfc85-7f7f-4dcb-8b0b-3a4af0aa57e4/kdw5ujpgc78pjxtv2b.png)
![Screenshot of dormant ESP](https://r2.e-z.host/bb3dfc85-7f7f-4dcb-8b0b-3a4af0aa57e4/hm7b14y2ehbcuy0or8.png)
![Screenshot of menu](https://r2.e-z.host/bb3dfc85-7f7f-4dcb-8b0b-3a4af0aa57e4/rcsdv5y34xbbw7a1p6.png)

# Is this finished?
No, it is very incomplete, and will change a lot over time. The program is inefficient, and commented out code is all over the place. Features such as ESP are very incomplete but work on a crude level.

# TODO
Ordered by importance  
- Automatically aim at hit box  
- More visual features (bones, proper head dot, dynamic health bar)  
- Sync ESP frame rate to game frame rate  
