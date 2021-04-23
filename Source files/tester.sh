#!/bin/bash

make
compiled=$?
if [[ $compiled != 0 ]]; then
	echo "does not compile"
	exit 1
fi

echo -n "Testing test1.txt : "
./a.out test1.txt -a -l -v > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
	echo "crashed"
	exit 1
fi
diff -w output.txt output1.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
	echo "incorrect output"
else
	echo "pass"
fi


echo -n "Testing test2.txt (this one is super long, but it covers everything): "
./a.out test2.txt -a -l -v > output.txt
executed=$?
if [[ $executed !=  0 ]]; then
	echo "crashed"
	exit 1
fi
diff -w output.txt output2.txt &> /dev/null
correct=$?
if [[ $correct != 0 ]]; then
	echo "incorrect output"
else
	echo "pass"
fi