#!/usr/bin/env bash

# This checks to see that a get cannot read a file with no
# read permissions

# Get available port.
port=$(bash test_files/get_port.sh)

# Start up server.
./httpserver $port > /dev/null &
pid=$!

# Wait until we can connect.
while ! nc -zv localhost $port; do
    sleep 0.01
done

for i in {1..100}; do
    # Test input file.
    file="test_files/wonderful.txt"
    infile="temp.txt"
    outfile="outtemp.txt"



    # Take away perms after writing to infile
    echo "Test Test 123" > $infile
    chmod -r $infile

    # Expected status code.
    expected=403

    # The only thing that is should be printed is the status code.
    actual=$(curl -s -w "%{http_code}" -o $outfile localhost:$port/$infile)

    # Check the status code.
    if [[ $actual -ne $expected ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
        rm -f $infile $outfile
	echo "FAILED"
        exit 1
    fi

done

# Make sure the server is dead.
kill -9 $pid
wait $pid

# Clean up.
rm -f $infile $outfile

echo "PASSED"
exit 0

