# RemoteShell

## Description
The goal of this project is to build a server and a client that allows a remote user to control a shell from another computer.
This project can be compiled and run on Windows and Linux operating systems.

## Getting Started

### Installing conan
Conan is a dependency manager used by our program to make it works. To compile it, you need to [install it](https://conan.io/downloads.html) if this isn't already installed on your machine. Then, there is some remote links to add to conan. Execute these commands: 
```bash
conan remote add https://bintray.com/conan/conan-center https://api.bintray.com/conan/conan/conan-center
conan remote add https://bintray.com/epitech/public-conan https://api.bintray.com/conan/epitech/public-conan
conan remote add https://bintray.com/bincrafters/public-conan https://api.bintray.com/conan/bincrafters/public-conan
```

### Installing cmake
The compilation is made with CMake. This program is needed to compile the project. Follow [this link](https://cmake.org/install/) to download it if this isn't already installed on your machine.

### Compiling the project

To compile the project, there is 2 files : _compile_win.bat_ and _compile_unix.sh_. Depending on your operating system, choose the correct binary to execute.
After the execution of the script, you will find two binaries named _RemoteShell_Server(.exe)_ and _RemoteShell_Client(.exe)_

#### Warning
If you are using _compile_win.bat_, execute this script with a shell that handle basic unix commands (cd, cp, mv, mkdir). 
For example, Powershell.exe and cmd.exe **do not** handle these commands. Instead, use [git bash](https://git-scm.com/downloads).

### Execute project
```
./RemoteShell_Server(.exe)
```

```
./RemoteShell_Client(.exe) [ip] [port]
```
