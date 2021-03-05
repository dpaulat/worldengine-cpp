pip install conan
pip install numpy
conan profile new default --detect
mkdir build-release
cd build-release
conan install .. -g cmake_find_package -g cmake_paths -s build_type=Release --build=missing
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release
pause
