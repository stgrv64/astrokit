#!/bin/bash
/etc/init.d/apache2 stop
find /etc/*rc*d -type l -name "*apache*" -exec unlink {} \;
