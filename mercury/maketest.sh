#!/bin/bash

if [ ! $1 ] ; then
    echo "Need argument"
    exit 1
fi

DEST=include/mercury/$1
rm -rf $DEST
cp -r test_skel/ $DEST

find $DEST -type f -exec sed -i 's/\$\$/'$1'/g' {} \;

