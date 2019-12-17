#/bin/sh

#startTime=`date +%Y%m%d-%H:%M:%S`
#startTime_sN=`date +%s.%N`
#echo "Start Time $startTime_sN"

val=1
max=5000
for i in `seq 2 $max`
do
	val=`expr $val + $i`
done

echo $val

#endTime=`date +%Y%m%d-%H:%M:%S`
#endTime_sN=`date +%s.%N`
#
#echo "End Time $endTime_sN"
#sumTime=`expr $endTime_s - $startTime_s`
#echo "$startTime ---> $endTime" "Totl:$sumTime seconds"

