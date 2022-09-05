# sms-BIOS-dumper

Dump SEGA Master System BIOS ROM using a Master EverDrive X7

Exploiting the EverDrive X7 emulation of 32 KiB of SRAM and the fact that it dumps its contents in a save file, here's a program to dump 128 KiB BIOSes in 4 segments.

Which means that to have your whole BIOS dumped you're supposed to dump segments 0 to 3 into 32 KiB save files in 4 different go and then reassemble them in order on a computer.

Pick the requested segment with UP/DOWN with the pad. Press 1/START to dump the segment. A message will confirm the copy to SRAM has completed.

If your BIOS is 256 KiB instead (as the European SMS BIOS "Sonic the Hedgehog") please use the _256KB version. The segments to dump and reassemble will be 8 of course.

