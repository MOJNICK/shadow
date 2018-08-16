#!/bin/bash
set -e
make -j8

filePairs=$1
removedDirectory=$2
nakedDirectory=$3

parallel -k -a $filePairs --colsep '\t' ./test/execute/execute --relative_norm $removedDirectory{1} $nakedDirectory{2} >> compare_script.tmp
cp compare_script.out compare_script.tmp.2
paste -d'\n' compare_script.tmp.2 compare_script.tmp > compare_script.out
rm compare_script.tmp compare_script.tmp.2

exit 0