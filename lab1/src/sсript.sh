#!/bin/bash
pwd
D=$(date  +%Y-%m-%d)
T=$(date +%H:%M:%S)
echo  "$D" "$T"
env | grep PATH
