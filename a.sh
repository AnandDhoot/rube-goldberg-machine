#!/bin/sh
while [ 1 ]; do
    make		
    timeout 45 mybins/cs251_exe_21
done

