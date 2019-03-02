#!/bin/bash
set -e
make -j8

filePairs=$1
shadowDirectory=$2
cleanDirectory=$3

parallel -k -a $filePairs --colsep '\t' ./test/execute/execute --relative_norm $shadowDirectory{1} $cleanDirectory{2} >> compare_script.out
#cp compare_script.out compare_script.tmp.2
#paste -d'\n' compare_script.tmp.2 compare_script.tmp > compare_script.out
#rm compare_script.tmp compare_script.tmp.2

exit 0