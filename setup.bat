conan profile new default --detect
mkdir build
cd build
conan install .. -g cmake_find_package_multi -s build_type=Debug
conan install .. -g cmake_find_package_multi -s build_type=Release
pause