#!/bin/bash
. ./bin/ask.sh
ESPPORT=./bin/ftdi_finder.py
if ask "Set ESPPORT to $ESPPORT?" Y; then
  export ESPPORT
  echo Using FTDI port at $ESPPORT
fi
if ask "Set ESPBAUD to 460800?" Y; then
  export ESPBAUD=460800
fi

unset -f ask
