#!/bin/bash
TOOLSPATH=$1
HOST=$2
ARCH=$3
PROJ=$4
export SUDO_ASKPASS=${PROJ}/script/ask-pass.sh
echo "Installing include files in $TOOLSPATH/$HOST/include"
sudo mkdir -p $TOOLSPATH/$HOST/include/hwpl
sudo mkdir -p $TOOLSPATH/$HOST/include/hwdl
sudo mkdir -p $TOOLSPATH/$HOST/include/dev
sudo mkdir -p $TOOLSPATH/$HOST/lib/ldscripts

sudo rsync -r --exclude=.svn ../include/* $TOOLSPATH/$HOST/include
#sudo cp -Rf ../include/* $TOOLSPATH/$HOST/include
if [ "$ARCH" != "link" ]; then
	sudo cp -Rf ../ldscript/* $TOOLSPATH/$HOST/lib/ldscripts
fi

if [ "$ARCH" == "link" ]; then
sudo cp -Rf ../src/link/Link.h $TOOLSPATH/$HOST/include/hwpl
fi

if [ "$ARCH" == "CRT" ]; then
	sudo cp ./libcrt.a $TOOLSPATH/$HOST/lib
else
	echo "Installing library file: libcaos_$ARCH.a in $TOOLSPATH/$HOST/lib"
	sudo cp -f ./libcaos_$ARCH.a $TOOLSPATH/$HOST/lib
fi

exit 0