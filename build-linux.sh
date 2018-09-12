#!/bin/bash

wget -P ~/linuxdeployqt https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage

cd ~/linuxdeployqt

chmod a+x linuxdeployqt-continuous-x86_64.AppImage

sudo apt-get --yes install qtbase5-dev qt5-qmake

export QT_SELECT=qt5

cd ~/projects/vcbld

mkdir bin_linux

cd bin_linux

cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++-8 -DWITH_GUI=ON -DCMAKE_BUILD_TYPE=Release ..

make -j$(nproc)

~/linuxdeployqt/linuxdeployqt-continuous-x86_64.AppImage ~/projects/vcbld/bin_linux/release/vcbld-gui -appimage -no-copy-copyright-files -no-translations

mv ~/projects/vcbld/bin_linux/vcbld-gui-x86_64.AppImage ~/projects/vcbld/bin_linux/release/vcbld-gui.AppImage
