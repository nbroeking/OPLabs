#/bin/sh

# kill all instances currently running of
# mercury
killall mercury

# start merucry with the library path
# we give it
LD_LIBRARY_PATH="lib/" ./mercury config.json >&1 &
pid=$!

# give the binary some time to run
sleep 1

exec 1>/dev/null

# request the binary surpress stdout and stderr
kill -SIGUSR1 $pid

# disown the process and send it to the background
disown
