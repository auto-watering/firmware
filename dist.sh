#!/bin/bash
set -e

if [ -d dist ]; then
	rm -r dist
fi
mkdir dist

cp *.h dist
cp *.ino dist
cp ChangeLog dist
cp AUTHOR LICENSE dist

sed -i 's/#define WIFI_ESSID.*/#define WIFI_ESSID/g' dist/config_wifi.h
sed -i 's/#define WIFI_PASSWORD.*/#define WIFI_PASSWORD/g' dist/config_wifi.h

TAG=$(git describe --tags)
CWD=$(pwd)
pushd dist
zip -r "${CWD}/auto_watering-${TAG}.zip" *
popd

rm -r dist
echo "Dist archive created at ${CWD}/auto_watering-${TAG}.zip"

