#!/bin/bash
filePairs=$1
removedDirectory=$2
nakedDirectory=$3

make -j8

while read pair
	do
		read removed naked<<<"$pair"
		removed=$removedDirectory$removed
		naked=$nakedDirectory$naked
	done < $filePairs

parallel -k -a $filePairs --colsep '\t' ./test/execute/execute --relative_norm $removedDirectory{1} $nakedDirectory{2} >> compare_script.out


exit 0