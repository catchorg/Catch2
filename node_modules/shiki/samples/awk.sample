#!/bin/awk -f
BEGIN {
# How many lines
    lines=0;
    total=0;
}
{
# this code is executed once for each line
# increase the number of files
    lines++;
# increase the total size, which is field #1
    total+=$1;
}
END {
# end, now output the total
    print lines " lines read";
    print "total is ", total;
    if (lines > 0 ) {
	print "average is ", total/lines;
    } else {
	print "average is 0";
    }
}

#From https://www.grymoire.com/Unix/Awk.html