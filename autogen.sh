#!/bin/sh

which gnome-autogen.sh || {
    echo "You need to install gnome-common package."
    exit 1
}

. gnome-autogen.sh
