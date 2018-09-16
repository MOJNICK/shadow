#!/bin/bash

set -ex 

directory=$1
git add -u && git commit -m "$directory"
mkdir ~szozda/dsk/process_2d/$directory
make -j8 && <allFiles.txt egrep '[[:digit:]]\.png' | xargs -P4 -n1 -I{} bash -c "image=~szozda/dsk/img/linearized/{}; ./test/execute/execute --process2d \$image ~szozda/git/Shadow-Removal/linearized_inv/normalized/nor_lin_inv_\$(basename \$image) ~szozda/dsk/process_2d/$directory/\$(basename \$image)"
feh ~szozda/dsk/process_2d/$directory
