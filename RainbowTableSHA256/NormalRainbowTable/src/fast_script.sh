#!/bin/bash
for i in {1..12}
do
    make CrackFile pl=$i cl=1000
done
