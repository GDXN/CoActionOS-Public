#!/bin/bash
PROJ=$1
PREFIX=$2
HOST=$3
$PREFIX/$HOST-objcopy -j .text -j .data -j .startup -O binary ./$PROJ.elf ./$PROJ.bin
$PREFIX/$HOST-objdump -j .text -j .data -j .bss -j .sysmem -d $PROJ.elf > $PROJ.s
$PREFIX/$HOST-size ./$PROJ.elf 
