pip install conan
pip install numpy
conan profile new default --detect
mkdir build
cd build
conan install .. -g cmake_find_package_multi -s build_type=Debug --build=missing
conan install .. -g cmake_find_package_multi -s build_type=Release --build=missing
pause