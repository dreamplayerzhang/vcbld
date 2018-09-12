call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"

cd C:\projects\vcbld

mkdir bin_w32

cd bin_w32

cmake -G "NMake Makefiles" -DCMAKE_PREFIX_PATH=C:\Qt\5.11.1\msvc2015 -DWITH_GUI=ON -DCMAKE_BUILD_TYPE=Release ..

nmake.exe

C:\Qt\5.11.1\msvc2015\bin\windeployqt.exe --no-translations --no-opengl-sw --no-system-d3d-compiler C:\projects\vcbld\bin_w32\release\vcbld-gui.exe