#!/bin/bash

mkdir -p testlog/

TESTDIR='_x86_obs/tests'
MAKETEST='make tests'

function echosuccess {
	echo -e '[1;32m'$1'[0m'
}

function echoinfo {
	echo -e '[1;37m'$1'[0m'
}

function echoerror {
	echo -e '[1;31m'$1'[0m'
}

echo "    +"
$MAKETEST 2>testlog/make-stderr.log | tee testlog/make-stdout.log | while read l ; do echo "    + $l" ; done
echo "    +"

rc=$?
if [[ "$rc" -ne 0 ]] ; then
	failed=1
	echo "$i: Failed!" >&2
	echo "---- Error Log ----" >&2
	cat $stderrlog >&2
	echoerror "build: Failed"
    exit 1
else
	echosuccess "build: Success"
fi

exec 2>runtests.err.log

failed=0
for i in $TESTDIR/* ; do
	stdoutlog=testlog/$(basename $i)-stdout.log
	stderrlog=testlog/$(basename $i)-stderr.log

    echoinfo "$i"
    echo "    + "
	$i 2>$stderrlog | tee $stdoutlog | while read l ; do echo "    + $l" ; done
    echo "    + "
	rc=$?
	if [[ "$rc" -ne 0 ]] ; then
		failed=1
		echo "$i: Failed!" >&2
		echo "---- Error Log ----" >&2
		cat $stderrlog >&2
		echoerror "$i: Failed"
	else
		echosuccess "$i: Success"
	fi
done

if [ $failed -ne 0 ] ; then
	echoerror "\n\n    One or more tests have failed!\n\n"
	exit 1
fi

exit 0
