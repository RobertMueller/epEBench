rm powera15.txt  powera7.txt  powermem.txt

echo "Test begins"
sh powersensor_append.sh &
#Put your benchmark here
sysctl kernel.sched_migration_cost=1000
~/bench/bin/Release/epEBench -m vidplay -u 15 -a 0 1 2 3 -t 60
sysctl kernel.sched_migration_cost=500000000
kill -9 `ps aux | grep powersensor_append.sh | awk '{print $2}'`
echo "Test finished"



