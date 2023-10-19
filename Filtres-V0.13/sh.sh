#rm bin/*; rm trace_impl; rm trace_tete;
rm tmpt/*
clear
printf "[***] Compilation ... \n"

#	Compiler
python3 compiler.py
if [ $? -eq 1 ]
then
	printf "\n[\033[91m***\033[0m] Erreure. Pas d'execution.\n"
	exit
fi

#	Executer
printf "[\033[95m***\033[0m] ========= Execution du programme =========\n"

#valgrind --leak-check=yes --track-origins=yes ./prog
#./prog & ./prog & ./prog & ./prog
./prog
if [ $? -ne 0 ]
then
	printf "[***] Erreur durant l'execution.\n"
	#valgrind --leak-check=yes --track-origins=yes ./prog
	sudo systemd-run --scope -p MemoryMax=100M gdb ./prog
	exit
fi
