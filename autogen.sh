#!/bin/sh

run() {
    echo "Running $1..."
    $1 || exit
}

. autotools.conf

run "$ACLOCAL"
run "$AUTOHEADER"
run "$AUTOCONF"
run "$AUTOMAKE"

echo "Finished successfully."
