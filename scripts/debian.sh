#!/bin/bash

######################################################################
#                                                                    #
# this script should work for debian and its derivatives #
#                                                                    #
######################################################################

# check if root
# https://stackoverflow.com/questions/18215973/how-to-check-if-running-as-root-in-a-bash-script
if [ "$EUID" -ne 0 ]; then
    echo "debian.sh: Please run as root"
    echo "debian.sh: Example: sudo ./debian.sh"
    exit
fi

# ubuntu being stupid
# https://github.com/actions/runner-images/issues/7589#issuecomment-1559929391

# no need to re-add it if it already exist. Also alert them that it already exists.
if [ $(dpkg --print-foreign-architectures) = "i386" ]; then
    echo "debian.sh: 32bit (i368) architecture already added"
else
    dpkg --add-architecture i386
fi

apt update -qq
apt install --no-install-recommends -yq \
    mesa-common-dev libx11-dev libxfixes-dev libxcomposite-dev \
    gcc g++ make libgtk-3-dev:i386 libgtk-3-dev gcc-multilib \
    g++-multilib

