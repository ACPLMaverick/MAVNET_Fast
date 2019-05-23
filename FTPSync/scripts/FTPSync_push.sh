#!/bin/bash

read -p "Are you sure you want to perform PUSH operation? You may accidentally delete or mess up something! [Y/N]: " prompt

if [ $prompt == y ]; then . ./FTPSync.sh PUSH; fi;
if [ $prompt == Y ]; then . ./FTPSync.sh PUSH; fi;
