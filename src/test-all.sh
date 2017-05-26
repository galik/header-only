#!/bin/bash

for prog in $(find src -name test-*-1[47] -executable|sort)
do
	./${prog}
done
