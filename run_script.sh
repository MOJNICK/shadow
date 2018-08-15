#!/bin/bash
directory=$1
git add -u && git commit -m "$directory"
mkdir ~szozda/dsk/iterate_result/$directory
make -j8 && <allFiles.txt egrep '[[:digit:]]\.png' | xargs -P4 -n1 -I{} bash -c "image=~szozda/dsk/img/linearized/{}; ./test/execute/execute --iterate \$image ~szozda/dsk/iterate_result/$directory/\$(basename \$image)"
feh ~szozda/dsk/iterate_result/$directory