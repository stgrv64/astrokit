#!/bin/bash

t[0]="0,1,2,3"
t[1]="0,1,3,2"
t[2]="0,2,1,3"
t[3]="0,2,3,1"
t[4]="0,3,1,2"
t[5]="0,3,2,1"

t[6]="1,0,2,3"
t[7]="1,0,3,2"
t[8]="1,2,0,3"
t[9]="1,2,3,0"
t[10]="1,3,0,2"
t[11]="1,3,2,0"

t[12]="2,0,1,3"
t[13]="2,0,3,1"
t[14]="2,1,0,3"
t[15]="2,1,3,0"
t[16]="2,3,0,1"
t[17]="2,3,1,0"

t[18]="3,0,1,2"
t[19]="3,0,2,1"
t[20]="3,1,0,2"
t[21]="3,1,2,0"
t[22]="3,2,0,1"
t[23]="3,2,1,0"

i=0
while [ 1 ]; do
  echo ${t[$i]}
  [ $i -eq 24 ]&& { break ; }
  i=$((i+1))
done
