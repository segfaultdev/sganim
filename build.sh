#!/usr/bin/sh

gcc -Iinclude $(find . -name "*.c") -Ofast -o render -lm
