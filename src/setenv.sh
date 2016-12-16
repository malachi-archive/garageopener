#!/bin/bash
export ESPPORT=$(./ftdi_finder.py)
echo Using FTDI port at $ESPPORT
