#!/bin/bash

app_name="RhythmBox"
build_dir="./LinuxMakefile/build"
resource_dir="./../resources"

rm -rf "./${app_name}"

if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
	echo "Downloading Linuxdeploy..."
	wget -c -nv "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
	chmod +x linuxdeploy-x86_64.AppImage
else
	echo "Linuxdeploy already exists, skipping download."
fi
for size in 16 32 48 128 256; do
	dirname="${app_name}/usr/share/icons/hicolor/${size}x${size}/apps"
	mkdir -p $dirname
	cp "${resource_dir}/icons/icon_${size}px.png" "./${dirname}/${app_name}.png"
done

# TODO: Add AppStream support in AppImage
#       uncomment this when https://github.com/AppImage/AppImageKit/issues/856
#       and/or 31 will be fixed
#
# mkdir -p "./${app_name}/usr/share/metainfo/"
# cp "${resource_dir}/linux/${app_name}.appdata.xml" "./${app_name}/usr/share/metainfo/"

if [ ! -f "${build_dir}/${app_name}" ]; then
	./build.sh -r
fi
./linuxdeploy-x86_64.AppImage --appdir=${app_name} \
                              --desktop-file=${resource_dir}/linux/${app_name}.desktop \
                              --executable=${build_dir}/${app_name} \
                              --output=appimage
