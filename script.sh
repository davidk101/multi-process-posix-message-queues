#!/bin/bash
gcc main.c -lrt
./a.out getdomainname
./a.out gethostname
./a.out uname 
