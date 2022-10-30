#!/usr/bin/sh

gcc -Iinclude $(find . -name "*.c") -Ofast -s -o render -lm
