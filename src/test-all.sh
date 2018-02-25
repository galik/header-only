#!/bin/bash

for prog in $(find . -name "test-1[147]*" -executable|sort)
do
	${prog}
done
