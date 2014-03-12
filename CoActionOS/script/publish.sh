#!/bin/bash

DEST=$1
VERSION=$2

#package and publish CoActionOS, CoActionOS-Applib, CoActionOS-Bootloader
cd ..
doxygen
cd ..
cd CoActionOS-Applib
doxygen
cd ..
WORKSPACE=$PWD
#directory should not be workspace

rm -Rf $DEST/$VERSION
mkdir -p $DEST/$VERSION/CoActionOS-doc
rsync -r --exclude=.svn ./CoActionOS/doc/html $DEST/$VERSION/CoActionOS-doc
rm -Rf ./CoActionOS/doc/html
mkdir -p $DEST/$VERSION/CoActionOS-doc
rsync -r --exclude=.svn ./CoActionOS-Applib/doc/html $DEST/$VERSION/CoActionOS-Applib-doc
rm -Rf ./CoActionOS-Applib/doc/html

rsync -r --exclude=.svn --exclude=Release* --exclude=publish.sh ./CoActionOS $DEST/$VERSION
rsync -r --exclude=.svn --exclude=Release* ./CoActionOS-Applib $DEST/$VERSION
rsync -r --exclude=.svn --exclude=build* ./CoActionOS-Bootloader $DEST/$VERSION
rsync -r --exclude=.svn --exclude=build* ./CoAction-Hero $DEST/$VERSION
rsync -r --exclude=.svn --exclude=build* ./CoAction-mbed $DEST/$VERSION

rsync -r --exclude=.svn --exclude=build* ./CoActionOS-QtSDK $DEST/$VERSION
rsync -r --exclude=.svn --exclude=build* ./CoActionOS-Link $DEST/$VERSION
rsync -r --exclude=.svn --exclude=build* ./CoActionOS-ISP $DEST/$VERSION

cd $DEST/$VERSION

zip -r ./CoActionOS-$VERSION.zip ./CoActionOS
zip -r ./CoActionOS-$VERSION.zip ./CoActionOS-Applib
zip -r ./CoActionOS-$VERSION.zip ./CoActionOS-Bootloader

zip -r ./CoAction-Hero-$VERSION.zip ./CoAction-Hero
zip -r ./CoAction-mbed-$VERSION.zip ./CoAction-mbed
zip -r ./CoActionOS-QtSDK-$VERSION.zip ./CoActionOS-QtSDK
zip -r ./CoActionOS-Link-$VERSION.zip ./CoActionOS-Link
zip -r ./CoActionOS-ISP-$VERSION.zip ./CoActionOS-ISP

#Zip up the docs and copy them to the server
cd CoActionOS-doc
zip ../CoActionOS-$VERSION-html.zip ./html/*
cd ..
cd CoActionOS-Applib-doc
zip ../CoActionOS-Applib-$VERSION-html.zip ./html/*
cd ..

#scp ./CoActionOS-$VERSION-html.zip tgil@coactionos.com:~/html.zip
#ssh -t tgil@coactionos.com './update-doxygen CoActionOS $VERSION'
#scp ./CoActionOS-Applib-$VERSION-html.zip tgil@coactionos.com:~/html.zip
#ssh -t tgil@coactionos.com './update-doxygen applib $VERSION'


cd $WORKSPACE
cd ..
cd CoActionOS-Desktop

cd CoActionOS-Link/build
../deploy $VERSION
mv ./CoActionOS-Link-$VERSION.dmg $DEST/$VERSION

cd ../..

cd CoActionOS-ISP/build-release
../deploy $VERSION
mv ./CoActionOS-ISP-$VERSION.dmg $DEST/$VERSION

#Now the Compiler
productbuild --root /usr/local/CoActionOS /usr/local/CoActionOS $DEST/$VERSION/CoActionOS-Compiler-4.7.3-$VERSION.pkg

rm -Rf $DEST/$VERSION/CoActionOS
rm -Rf $DEST/$VERSION/CoActionOS-doc
rm -Rf $DEST/$VERSION/CoActionOS-Applib
rm -Rf $DEST/$VERSION/CoActionOS-Applib-doc
rm -Rf $DEST/$VERSION/CoActionOS-Bootloader
rm -Rf $DEST/$VERSION/CoAction-Hero
rm -Rf $DEST/$VERSION/CoAction-mbed
rm -Rf $DEST/$VERSION/CoActionOS-QtSDK
rm -Rf $DEST/$VERSION/CoActionOS-Link
rm -Rf $DEST/$VERSION/CoActionOS-ISP

exit 0