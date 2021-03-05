pip install conan
pip install numpy
conan profile new default --detect
mkdir build-debug
cd build-debug
conan install .. -g cmake_find_package -g cmake_paths -s build_type=Debug --build=missing
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CONFIGURATION_TYPES=Debug
pause
