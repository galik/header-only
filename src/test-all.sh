#!/bin/bash

for prog in *.cpp
do
	name=${prog%*.cpp}
	echo ${name}
	./${name}
done
