# 2048
A copy of 2048 game using SDL2

## Linux

Assuming developer versions of SDL2 (>= 2.0.12) and SDL2_ttf (>= 2.0.15) are installed:

```
mkdir build
cd build
cmake ..
make
./2048
```

## Windows x64

Assuming Visual Studio 2015 is installed: (via MinGW64)

```
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015 Win64"
```

After this a ".sln" will be generated that can be opened with Visual Studio 2015.
Right click on 2048 in project explorer and select as startup project.
You can now run the demo from Visual Studio.

## Mac OS

Not tested