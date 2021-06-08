call tools\setup-common.bat
mkdir build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release
pause
