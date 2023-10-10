#!/bin/sh

gcc -o share ./.src/share.c
echo "[INFO]: Build Successful!! To get started run: ./share -help\n"
echo "[INFO]: It is likely that some hosts may not be able to connect to the PORT at which you are serving!!\n"
