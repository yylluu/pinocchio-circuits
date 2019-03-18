#!/bin/sh
python vercomp.py keyPairing.c --arith exp-mod.2048.arith --ignore-overflow True --progress True --bit-width 128 > log
