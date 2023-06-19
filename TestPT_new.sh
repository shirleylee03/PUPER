#!/bin/bash
#---------------------------------------------------
#usage:
#./TestPT_new.sh <execfilename> <input-dir> <instances-list>
#./TestPT_new.sh PUPER ../Benchmarks list.txt

path=`pwd`
echo "pwd: $path"

result=$path"/"$1"_results.txt"
echo "Result is presented in: $result"

log_file=$path"/"$1"_log.txt"
echo "Log is presented in: $log_file"

source=$path"/"$2
echo "Input directory is: $source"

list=$path"/"$3
echo "Instances to be checked are presented in: $list"

if [ ! -n "$1" ]
then
	echo "usage: ./TestPT.sh <execfilename> <input-dir> <instances-list>"
	echo "TestPT.sh, <execfilename>, <input-dir>, <instances-list> should be in the same directory"
elif [ ! -n "$2" ]
then
	echo "usage: ./TestPT.sh <execfilename> <input-dir> <instances-list>"
	echo "TestPT.sh, <execfilename>, <input-dir>, <instances-list> should be in the same directory"
elif [ ! -n "$3" ]
then
	echo "usage: ./TestPT.sh <execfilename> <input-dir> <instances-list>"
	echo "TestPT.sh, <execfilename>, <input-dir>, <instances-list> should be in the same directory"
else
	for line in `cat $list`
	do
    	flag=0
    	name=$line
	#echo $name
	
    	for element in `ls $source` #遍历整个测试集
    	do  
 		if [ $name == $element ]    #找到该测试用例
		then
			instance_name=${line%.*}
			#运行
			#默认公式
			./$1 $2/$line |tee -a $result
			#对应公式
			#./$1 -property $2/$instance_name.xml $2/$line |tee -a $result
			#slice+默认公式
			#./$1 -PDNetSlice $2/$line |tee -a $result
			#slice+对应公式
			#./$1 -PDNetSlice -property $2/$instance_name.xml $2/$line |tee -a $result
			#写结果
			#echo -n -e "${element%.*}\t" >> $result       #将该例子的名字先写到result.txt中
			flag=1
			break
		fi
    	done
		if [ $flag -eq 0 ]
		then
			echo "Input:$line  does not exist"
		fi
	done
fi
