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
cp config.h.example dist
cp HomeAssistant-dashboard-example.yaml dist
cp README.md dist
cp platformio.ini dist
cp dist.sh dist

rm dist/config.h

TAG=$(git describe --tags)
CWD=$(pwd)
pushd dist
zip -r "${CWD}/auto_watering-${TAG}.zip" *
popd

rm -r dist
echo "Dist archive created at ${CWD}/auto_watering-${TAG}.zip"

