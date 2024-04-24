#!/bin/bash
# This is a small script that renames all the files in the corpus directory to have sequential names
# This is to avoid issues like spaces in names and name conflicts etc...

cd ./corpus
ls -v | cat -n | while read n f; do mv -n "$f" `sha256sum "$f" | head -c 40`_$n.png; done 
#ls -v | cat -n | while read n f; do mv -n '$f' "$n.png"; done 
