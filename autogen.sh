#!/bin/sh

# change this to the path to mkinstalldirs on your system
MKINSTALLDIRS="/usr/local/gnu-autotools/share/automake-1.9/mkinstalldirs"

run() {
    echo "Running $*..."
    $* || exit
}

run intltoolize --force

# po/Makefile.in.in requires mkinstalldirs, but intltoolize does not provide it
run ln -sf "$MKINSTALLDIRS" mkinstalldirs

run libtoolize --force
run aclocal -I m4
run autoheader
run autoconf
run automake --add-missing --force-missing

echo "Finished successfully."
