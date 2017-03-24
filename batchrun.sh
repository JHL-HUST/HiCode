#!/bin/bash
if [ ! $# == 2 ]; then
	echo "Usage: $0 UniversityName Semaphore"
	echo "e.g.: $0 Caltech36 16"
	exit
fi

INSTANCE="$1"

BaseAlgs=("IM" "MOD")
#candidates:"LC" "OSLOM"
Frameworks=("RemoveEdge" "ReduceEdge" "ReduceWeight")

LayersNum=("2" "3" "4" "5")

tmp_fifofile="/tmp/$.fifo"
mkfifo $tmp_fifofile      # 新建一个fifo类型的文件
exec 6<>$tmp_fifofile      # 将fd6指向fifo类型
rm $tmp_fifofile
thread=$2 # 此处定义线程数

for ((i=0;i<$thread;i++));do
	echo
done >&6 # 事实上就是在fd6中放置了$thread个回车符

for BaseAlg in ${BaseAlgs[@]}; do
	for Framework in ${Frameworks[@]}; do
		for Num in ${LayersNum[@]}; do
			read -u6
			# 一个read -u6命令执行一次，就从fd6中减去一个回车符，然后向下执行，
			# fd6中没有回车符的时候，就停在这了，从而实现了线程数量控制
			Task="$INSTANCE/$BaseAlg/$Framework/$Num"
			LogDir="log/$Task/100Thres3/test0"
			LogFile="$LogDir/batchrun.log"
			ConfigFile="config/${Task}.config"
			#echo "$ConfigFile"
			mkdir -p $LogDir
			echo "--$BaseAlg"
			echo "----$Framework"
			echo "**** Start: $Task"
			{ # 此处子进程开始执行，被放到后台
				$HIDDEN_DIR/Main $ConfigFile>$LogFile 2>&1 && { # 此处可以用来判断子进程的逻辑
				echo "**** Finish: $Task" 
				} || {
				echo "**** Error: $Task"
				}
			echo >&6 # 当进程结束以后，再向fd6中加上一个回车符，即补上了read -u6减去的那个
			} &
		done
	done
done
wait # 等待所有的后台子进程结束
exec 6>&- # 关闭df6
exit 0
