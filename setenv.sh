#!/bin/bash
. ./bin/ask.sh
export ESPPORT=$(./bin/ftdi_finder.py)
echo Using FTDI port at $ESPPORT
if ask "Set ESPBAUD to 460800?" Y; then
  export ESPBAUD=460800
fi

unset -f ask
