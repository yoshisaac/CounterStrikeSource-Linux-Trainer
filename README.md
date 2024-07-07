# What is this?
This is a program that is intended to automate tasks such as movement, and display information to us that wasn't originally intended to be shown. In a nutshell, this is a video game hack/cheat.

# How does it work?
It externally (as a seperate program from the video game) reads and writes memory using system calls that the Linux kernel graciously gives us. Those system calls include functions like `processes_vm_readv()` and `process_vm_writev()`. 
  
To display a window and draw information inside of it, the program uses the X11 protocol. That can be a problem if you are using a display server/client such as Wayland or its variants, so make sure your desktop environment/floating window manager is set to a Xorg session. There may also be other issues your window manager or desktop environment can cause, such as overriding placement of windows.

# How to compile
  
### Installing dependencies
Install the necessary packages.  
  
Debian/Ubuntu/LinuxMint:

```console
$ sudo apt install mesa-common-dev libx11-dev libxfixes-dev libxcomposite-dev gcc g++ make libgtk-3-dev:i386
```
ArchLinux:  

```console
$ sudo pacman -S base-devel xorg-fonts-misc
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

The only distribution I have tested (and currently use) is Linux Mint 21.3

# Features
- Bunny hop (automatically jump after hitting the ground)  
- Player ESP  
- Aim lock (Hold left ALT)  

![Screenshot of ESP](https://cdn.discordapp.com/attachments/1091542098471293092/1243730595641692261/image.png?ex=665289e1&is=66513861&hm=98050a2f1e201ca2c671744ff66b57f4a99b6fde275bad420e1b16f5dc6cf78c&)

# Is this finished?
No, it is very incomplete, and will change a lot over time. The program is inefficient, and commented out code is all over the place. Features such as ESP are very incomplete but work on a crude level.

# TODO
Ordered by importance  
- Automatically aim at hit box  
- More visual features (bones, proper head dot, dynamic health bar)  
- Sync ESP frame rate to game frame rate  
