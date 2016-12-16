#!/bin/bash

cd /home/pi/Lampi/arduino

md5_file="md5hash"
hex_file="src/src.ino.with_bootloader.hex"
conf_file="avrdude.conf"

old_md5=$(cat "$md5_file")
new_md5=$(md5sum "$hex_file")
if [ "$old_md5" != "$new_md5" ];
then
	echo "updated arduino file found.  Updating"
	sudo avrdude -p atmega328p -C "$conf_file" -c pi_1 -U flash:w:$hex_file:i && echo "$new_md5" > "$md5_file"
else
	echo "arduino file has not changed.  Skipping"
fi

cd -
