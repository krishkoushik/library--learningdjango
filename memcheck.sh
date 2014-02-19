SECONDS=0
for ((;;))
do
	if [[ -d /proc/${1} ]] ; then
		heap_data=$(grep -s VmData* /proc/${1}/status|sed 's/[^0-9]*//g')
		stack=$(grep -s VmStk* /proc/${1}/status|sed 's/[^0-9]*//g')
		total_mem=$((stack+heap_data))
		echo $total_mem > mes.txt
		if [ $total_mem -ge 5000 ]; then
			kill -9 ${1}
			echo "Memory Limit Exceeded" > mes.txt
			break
		fi
		if [ $SECONDS -ge 30 ]; then
			kill -9 ${1}
			echo "Time Limit Exceeded" > mes.txt
			break
		fi
	else
		echo "Running Successful" > mes.txt
		break
	fi
done
