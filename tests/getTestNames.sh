#!/bin/sh

# Aidan Bird 2022
#
# get the names of all the test cases in a checks c file 
#

prog="$(basename "$0")"
cmd_usage="usage: $prog [TESTING FILE]"
[ $# -lt 1 ] && { echo "$cmd_usage"; exit 1; }
cat "$1" | grep 'START_TEST' | sed 's/^START_TEST *(//;s/)$//'
exit 0
