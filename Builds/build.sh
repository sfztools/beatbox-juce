#!/bin/bash
showHelp() {
	echo "Usage: build.sh [option]"
	echo "Build command for Linux and macOS"
	echo ""
	echo "Options are not mandatory, only one at a time."
	echo "-h    Shows this help and exits."
	echo "-r    Sets the build configuration to Release (default)."
	echo "-d    Sets the build configuration to Debug."
	exit 0
}
if [ ! "$1" ] || [ "$1" == "-r" ]; then
	config="Release"
elif [ $1 == "-d" ]; then
	config="Debug"
else
	showHelp
fi
if [ -d $HOME/JUCE ]; then
	echo "A $HOME/JUCE directory is already present, skipping download"
else
	if [ "$OSTYPE" == "linux-gnu" ]; then
		os=linux
	elif [ "$OSTYPE" == "darwin"* ]; then
		os=osx
	fi
	if [ -f "./juce-5.4.3-${os}.zip" ]; then
		echo "./juce-5.4.3-${os}.zip already on disk, skipping download."
	else
		echo "Downloading Juce for ${os}..."
		wget -c -nv "https://d30pueezughrda.cloudfront.net/juce/juce-5.4.3-${os}.zip"
	fi
	unzip ./juce-5.4.3-${os}.zip -d $HOME
	echo "Juce installed in $HOME/JUCE, starting build."
fi
echo "${config} build:"
if [ "$OSTYPE" == "linux-gnu" ]; then
	cd ./LinuxMakefile
	make -j$(nproc) CONFIG=${config}
elif [ "$OSTYPE" == "darwin"* ]; then
	brew update
	cd ./MacOSX
	xcodebuild -project rhythmbox.xcodeproj -alltargets -configuration ${config} build
fi
