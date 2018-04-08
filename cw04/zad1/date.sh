#!/bin/bash

tput civis -- invisible
while [ 1 ]; do
    echo -ne "\t$(date)\\r"
    sleep 1
done    
