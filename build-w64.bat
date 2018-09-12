call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

cd C:\projects\vcbld

mkdir bin_w64

cd bin_w64

cmake -G "NMake Makefiles" -DCMAKE_PREFIX_PATH=C:\Qt\5.11.1\msvc2017_64 -DWITH_GUI=ON -DCMAKE_BUILD_TYPE=Release ..

nmake.exe

C:\Qt\5.11.1\msvc2017_64\bin\windeployqt.exe --no-translations --no-opengl-sw --no-system-d3d-compiler C:\projects\vcbld\bin_w64\release\vcbld-gui.exe