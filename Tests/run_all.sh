#!/bin/sh

set -e

make 

rm -f Tests/_*

TS=`date +%Y%m%d%H%M`

if [ "x$1" != "x" ] ; then
	TS="${TS}_${1}"
fi

for tst in \
	Tests/expmon_reset_all.sh \
	Tests/fru_phase3.sh \
	Tests/test_novram.sh
do
	sh $tst || ( [ $? == 9 ] && sh $tst ) || true
done

mkdir -p Tests/Results/$TS
mv Tests/_* Tests/Results/$TS
cd Tests/Results/$TS && python3 ../../testsumm.py > summary.txt
if [ -d ../baseline ] ; then
	sdiff ../baseline/summary.txt summary.txt
fi
