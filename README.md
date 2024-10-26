This is a work in progress sine tone generator, made using the Windows API. Upon running the program, a sine tone will play at 440Hz, with a sample rate of 48kHz. I am working on this project to learn about how audio programming works under the hood.

The current state of the program is conversion into using WASAPI instead of WinMM, followed by getting the sine tone to work in low latency (i.e. bandwidth of 10ms).

To build in Windows with CMake and MinGW, create a directory called "build", and from within this directory, run the command `cmake -G "MinGW Makefiles" ..` and run the executable.
