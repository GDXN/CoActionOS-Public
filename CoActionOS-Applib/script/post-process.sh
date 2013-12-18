#!/bin/bash
TOOLSPATH=$1
HOST=$2
ABSTRACTION=$3
ARCH=$4
export SUDO_ASKPASS=../script/ask-pass.sh
echo $SUDO_ASKPASS
echo "Installing include files in $TOOLSPATH/$HOST/include"
sudo mkdir -p $TOOLSPATH/$HOST/include/applib
sudo cp -f ../include/* $TOOLSPATH/$HOST/include/applib
echo "Installing library file: libapp_$3_$ARCH.a in $TOOLSPATH/$HOST/lib"
sudo cp -f ./libapp_$3_$ARCH.a $TOOLSPATH/$HOST/lib

exit 0