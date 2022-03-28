#!/bin/bash

/etc/init.d/mysql stop
find /etc/*rc*d -type l -name "*mysql*" -exec unlink {} \;
