rm -rf build
mkdir build
cd build
conan install .. --build=missing
cmake .. -G "Visual Studio 16 2019"
cmake --build .
cp ./bin/RemoteShell_Server.exe ../
cp ./bin/RemoteShell_Client.exe ../