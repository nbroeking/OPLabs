#!/bin/bash

test_scripts=$(find -name test.sh)

function echoinfo {
	echo -e '[1;34m'$1'[0m'
}

function echosuccess {
	echo -e '[1;32m'$1'[0m'
}

function echoerror {
	echo -e '[1;31m'$1'[0m'
}

fail=0
fail_dirs=""

for i in $test_scripts ; do
    dir=$(dirname $i)
    echoinfo "=== descending into directory $dir ==="
    pushd "$dir" &>/dev/null

    echoinfo "=== Executing top level test script: $i ==="
    ./$(basename $i) $@ | while read i ; do echo "    +  $i"; done
    rc=${PIPESTATUS[0]}

    if [ $rc -ne 0 ] ; then
        fail=1
	    echoerror "=== One or more tests for $dir failed ==="
        fail_dirs="$(basename $dir) $fail_dirs"
    else
	    echoinfo "=== Tests for $dir sucessfully completed ==="
    fi

    popd &> /dev/null
done

if [ $fail -eq 0 ] ; then
    echosuccess "=== All tests succeeded ==="
else
    echoerror "=== Tests failed for [$fail_dirs] ==="
fi

exit $fail
