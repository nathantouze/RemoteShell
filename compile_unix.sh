rm -rf build
mkdir build
cd build
conan install .. --build=missing
cmake .. -G "Unix Makefiles"
cmake --build .
cp ./bin/RemoteShell_Server ../
cp ./bin/RemoteShell_Client ../