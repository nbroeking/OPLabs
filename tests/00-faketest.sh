#!/bin/bash

function echopass {
	echo -e $1' [[1;32mPASS[0m]'
}

sleep 0.5
echopass "Fake Stuff 0"

sleep 0.5
echopass "Fake Stuff 1"

sleep 1.0
echopass "Fake Stuff 2"

exit 0
