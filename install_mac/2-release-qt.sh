#!/bin/sh
# Copy Qt frameworks to marble bundle
# and make search paths for them relative to bundle

PREFIX=${HOME}/apps/Marble.app/Contents/MacOS

# Edit version when any library is upgraded
LIBJPEG=libjpeg.dylib
LNKJPEG=libjpeg.dylib
LIBPNG=libpng.dylib
LNKPNG=libpng.dylib

QTVER=4.2.3
QTPREFIX=/Library/Frameworks
QTFRAMEWORKS="QtCore QtGui QtNetwork QtSql QtSvg QtXml Qt3Support QtDesigner QtTest"

# Copy supporting frameworks to application bundle
cd $PREFIX/lib

for FRAMEWORK in $QTFRAMEWORKS
do
	LIBFRAMEWORK=$FRAMEWORK.framework/Versions/4/$FRAMEWORK
	if test ! -f $LIBFRAMEWORK; then
		mkdir -p $FRAMEWORK.framework/Versions/4
		cp $QTPREFIX/$LIBFRAMEWORK $LIBFRAMEWORK
		install_name_tool -id @executable_path/libs/$LIBFRAMEWORK $LIBFRAMEWORK
	fi
done

# Update path to supporting frameworks
for FRAMEWORK in QtGui QtNetwork QtSql QtSvg QtXml Qt3Support QtDesigner QtTest
do
	install_name_tool -change QtCore.framework/Versions/4/QtCore \
		@executable_path/lib/QtCore.framework/Versions/4/QtCore \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done
for FRAMEWORK in QtSvg Qt3Support QtDesigner
do
	install_name_tool -change QtGui.framework/Versions/4/QtGui \
		@executable_path/lib/QtGui.framework/Versions/4/QtGui \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
	install_name_tool -change QtXml.framework/Versions/4/QtXml \
		@executable_path/lib/QtXml.framework/Versions/4/QtXml \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done
for FRAMEWORK in Qt3Support
do
	install_name_tool -change QtNetwork.framework/Versions/4/QtNetwork \
		@executable_path/lib/QtNetwork.framework/Versions/4/QtNetwork \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
	install_name_tool -change QtSql.framework/Versions/4/QtSql \
		@executable_path/lib/QtSql.framework/Versions/4/QtSql \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done
for FRAMEWORK in QtGui QtSvg Qt3Support QtDesigner
do
	install_name_tool -change $LNKPNG @executable_path/lib/$LNKPNG \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done
cd ../
LIBQJPEG=/Developer/Applications/Qt/plugins/imageformats/libqjpeg.dylib
if test ! -f $LIBQJPEG; then
	mkdir imageformats
	cp $QTPREFIX/plugins/$LIBQJPEG $LIBQJPEG
	# Update path to supporting libraries
	install_name_tool -change QtCore.framework/Versions/4/QtCore \
		@executable_path/lib/QtCore.framework/Versions/4/QtCore \
		$LIBQJPEG
	install_name_tool -change QtGui.framework/Versions/4/QtGui \
		@executable_path/lib/QtGui.framework/Versions/4/QtGui \
		$LIBQJPEG
	install_name_tool -change $LNKJPEG @executable_path/lib/$LNKJPEG $LIBQJPEG
	install_name_tool -change $LNKPNG @executable_path/lib/$LNKPNG $LIBQJPEG
fi
cd ../../../

for FILE in \
	marble \
	lib/libmarblewidget.5.0.0.dylib
do
	for FRAMEWORK in QtCore QtGui QtNetwork QtSql QtSvg QtXml Qt3Support QtDesigner QtTest
	do
		install_name_tool -change $FRAMEWORK.framework/Versions/4/$FRAMEWORK \
			@executable_path/lib/$FRAMEWORK.framework/Versions/4/$FRAMEWORK \
			$PREFIX/$FILE
	done
done
