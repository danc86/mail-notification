#!/bin/sh
#
# This script is adapted to my own system. You might need to tweak it
# a bit if you want to run it at your local site.
#
# Required software:
#
#	GNU Autoconf 2.57
#	GNU Automake 1.7

export ACLOCAL="aclocal17 -I m4"
export AUTOHEADER="autoheader257"
export AUTOCONF="autoconf257"
export AUTOMAKE="automake17"

run() {
    echo "Running $1..."
    $1 || exit
}

run "$ACLOCAL"
run "$AUTOHEADER"
run "$AUTOCONF"
run "$AUTOMAKE"

echo "Finished successfully."
