sed -n -e 's/\(,,,00\.0*\)\(.*\)\(000000,\)\(.*\)\(0000,$\)/\2\4/gp' square.CSV  > square.csv