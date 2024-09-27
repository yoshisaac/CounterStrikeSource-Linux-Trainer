# What is this?
This is a program intended to automate tasks such as movement, and display information to us that wasn't originally intended to be shown. In a nutshell, this is a video game hack/cheat.

# How does it work?
It externally (as a seperate program from the video game) reads and writes memory using system calls that the Linux kernel graciously gives us. Those system calls include functions like `processes_vm_readv()` and `process_vm_writev()`. 
  
To display a window and draw information inside of it, the program uses the X11 protocol. If you are using Wayland, then there is a chance Xwayland will not handle it correctly. There may also be other issues your window manager or desktop environment can cause, such as overriding placement of windows, or preventing programs from having transparent backgrounds.

# Does my distro or DE work with this?
Most likely. Any problems you will have are primarily going to be the desktop environment's fault, or your distro will not package the required dependencies.  
Here is a list of desktops and distributions that have been tested.  
  
Distributions:  
- Arch Linux  
- Linux Mint  
  
Desktops:  
- Cinnamon (X11 only)  
- KDE Plasma (Wayland only)  
- Gnome (X11, Wayland untested)  
- Xfce4 (X11, DE does not support Wayland yet)  
- Dwm (X11, WM does not support Wayland)  

# How to compile

### Cloning Repository
Clone the repo using `git`, and change your directory into the root of the project.  

```console
$ git clone https://github.com/yoshisaac/CounterStrikeSource-Linux-Trainer.git  
$ cd CounterStrikeSource-Linux-Trainer/
```
  
### Installing dependencies
Install the necessary packages.  
  
Debian/Ubuntu/LinuxMint:
```console
$ sudo ./scripts/debian.sh
```
This script will automagically add the 32bit architecture and install the packages.  
It is magic, because I don't understand how it works!  

ArchLinux:  
```console
$ sudo pacman -S base-devel xorg-fonts-misc lib32-gtk3
```
If you some how haven't already, you will need to [enable the `multilib` repository for `pacman`](https://wiki.archlinux.org/title/Official_repositories#multilib).  
On Arch, once you install `xorg-fonts-misc` you need to restart your computer.  

### Compiling
Build the program from source.  

```console
$ make
...
```

# How to use
After compilation, there will be a program created in the root directory of the project called `cs-source-hack`.

To run the hack, go to the root directory of the project and type `sudo ./cs-source-hack` in your terminal. What this does is it runs the software as `root`, which is required for reading and writing memory on an external level.  

# Features
- Bunny hop (automatically jump after hitting the ground)  
- Visual  
  * External Crosshair  
  * RCS Crosshair  
- Player ESP  
  * Box  
  * Skeleton
  * Name  
  * Health  
  * Armor
  * Snap Lines  
- Aimbot (Hold left ALT)  

![Screenshot of ESP](https://r2.e-z.host/bb3dfc85-7f7f-4dcb-8b0b-3a4af0aa57e4/6oscyb9iqkr4r3gl9k.png)
![Screenshot of dormant ESP](https://r2.e-z.host/bb3dfc85-7f7f-4dcb-8b0b-3a4af0aa57e4/hm7b14y2ehbcuy0or8.png)
![Screenshot of menu](https://r2.e-z.host/bb3dfc85-7f7f-4dcb-8b0b-3a4af0aa57e4/mxaejoj82j8hj92j4f.png)

# Is this finished?
Perhaps you could say it is. It does everything a cheat would do.  
Only minor improvements and bug fixes exist for what the hack currently has, but more advanced features like silent-aimbot or auto-strafing would be neat.  

# TODO
Ordered by importance  
- Toggleable network traffic  
- Roll the skeleton ESP into a loop  
- Sync ESP frame rate to game frame rate  
- Silent-aimbot  
- Strafe optimizer  
