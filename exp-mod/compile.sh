#!/bin/sh
python vercomp.py exp-mod.c --arith exp-mod.2048.arith --ignore-overflow True --progress True --bit-width 128 > log
