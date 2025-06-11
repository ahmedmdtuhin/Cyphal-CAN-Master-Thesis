# This is for vendor independent build and flash using Makefile and CMake project.

# STM32 Tutorials 

This directory contains the code to [this video turorial](https://youtu.be/FkqQpBqkSns?si=5zNxzBqs-jDhxcmu). Make sure to watch the tutorial video while trying out the code yourself.

## Dependencies and Installation

You need to install:


|                                Instruction                               |     Description     |
|------------------------------------------------------------------------|-------------------|
|                     sudo apt install build-essential                     |  GCC C/C++ Compiler |
| sudo apt-get install cmake                                               | CMake               |
| sudo apt install stlink-tools                                            | [St-Link Tools](https://github.com/stlink-org/stlink)        |
| sudo apt install tio                                                     | [tio](https://github.com/tio/tio)       |
| https://www.st.com/en/development-tools/stm32cubemx.html#st-get-software | STM32 CubeMX        |
| https://developer.arm.com/downloads/-/gnu-rm                             | STM32 Crosscompiler |
| https://www.st.com/en/development-tools/stm32cubeprog.html               | STM32 CubeProgrammer |








The stable version of CubeMX is `6.10.0` and CubeProgrammer is `2.15.0` as of December 2024.
Make sure to install the correct version for your system. In my case it would be `gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2` cross-compiler, since I use
an `x86-64` architecture to compile for STM32. To now install the cross-compiler, based on [askubuntu](https://askubuntu.com/questions/1243252/how-to-install-arm-none-eabi-gdb-on-ubuntu-20-04-lts-focal-fossa), do:

```
sudo tar xjf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C /usr/share/
```

then 

```
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy
```

to symbolically link the executables to the previously installed software. To check if it works:

```
arm-none-eabi-gcc --version
arm-none-eabi-g++ --version
arm-none-eabi-gdb --version
arm-none-eabi-size --version
```

where `arm-none-eabi-gcc` is the C-Cross-Compiler, `arm-none-eabi-g++` is the C++-Cross-Compiler, `arm-none-eabi-gdb` GDB is the GNU Project Debugger and `arm-none-eabi-size` is 
an utility tool to analyze binary size after compilation. Be aware the commands might be slightly different for your case, i.e. `...gcc-arm-none-eabi-10.3-2021...` might differ!

Now symbolically link the paths to include directories:

```
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/arm-none-eabi /usr/arm-none-eabi
```

such that later on, you can include the `.h`-files in your project, e.g. `/usr/arm-none-eabi/include` and `/usr/arm-none-eabi/include/c++/10.3.1`.


## Make-c - Manual Compilation

To compile `main.c` and `Test.c` manually to Object-files, do

```
cd $yourPath/manual-c
gcc -c main.c
gcc -c Test.c
```

which will generate `main.o` and `Test.o` into the directory. To link both object files and generate an exectuable do

```
gcc -o executable main.o Test.o
```

## Make-c - Make File and Compilation

To automate the manual process from above using Makefile, do

```
cd $yourPath/make-c
make
```

which will build all files into `$yourPath/make-c/build`.

## Make-cpp

The above is also true for the Make-cpp. So 

```
cd $yourPath/make-cpp
make
```

## CubeMX and Make

This part refers to [this part of the tutorial](https://youtu.be/FkqQpBqkSns?feature=shared&t=847) in which you generate the necessary Make file using CubeMX. See `$yourPath/test-f407/` and start the installed CubeMX software. Open existing project `$yourPath/test-f407/test-f407.ioc` and follow the tutorial.

## Flashing

To now flash your software, do

```
cd $yourPath/make-cpp
(sudo) st-flash --reset write build/$yoursoftwarename.bin 0x8000000
```

## Help

Contact the [Authors](##authors), but please make sure to make an effort before asking for help.

## Authors

Contributor names and contact info

Eric Schöneberg | [scoeerg@gmail.com](scoeerg@gmail.com)
Md Tuhin Ahmed  | [ahmedmd.tuhin@yahoo.com](ahmedmd.tuhin@yahoo.com)

## Version History

* Version 0.01
    * I really hope it works.

## License

I have not yet started on licences.

## Acknowledgments

Thanks to [awesome-readme](https://github.com/matiassingers/awesome-readme) for the ReadMe-Template.