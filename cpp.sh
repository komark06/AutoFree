#!/usr/bin/env bash

dir="src include"
CPPCHECK_suppresses="--suppress=unusedFunction --suppress=missingIncludeSystem --inline-suppr"
CPPCHECK_OPTS="-Iinclude -Isrc --enable=all --error-exitcode=1 --force  $CPPCHECK_suppresses"

NORMAL="\e[m"
RED="\e[0;32;31m"
LIGHT_RED="\033[1;31m"
GREEN="\033[0;32;32m"
YELLOW="\033[1;33m"

CPPCHECK=$(which cppcheck)
if [ $? -ne 0 ]; then
    echo "${RED}[WARNING]${NORMAL}: ${YELLOW}cppcheck${NORMAL} not installed." >&2
    exit 1
fi

$CPPCHECK $dir $CPPCHECK_OPTS > /dev/null
if [ $? -ne 0 ]; 
then
    RETURN=1
    echo 
    echo "[${RED}WARNING${NORMAL}]: Fail to pass ${YELLOW}cppcheck${NORMAL} analysis." >&2
    echo
else
    echo "[${GREEN}PASS${NORMAL}]: cppcheck analysis."      
fi
exit $RETURN