#!/bin/bash
#Co. 2020,2023 Waldemar Bajdecki

if [ $# -lt 1 ]
then
	echo "skladnia: $0 plik_typu_c"
	exit
fi

PLIK=`basename $1`
if [ ! -f $PLIK ]
then
    echo $PLIK nie jest plikiem!
    exit; 
fi
if [[  `echo $PLIK |grep ".c$"` != "" ]]
then 
    if [[ `file $PLIK |grep "C source"` != "" ]]
    then
        echo $PLIK to plik wlasciwy, rozpoczynam kompilacje;
	if [ ! -e Makefile ]
	then
#poczatek tworzenia Makefile
echo 'CFLAGS=-Wall -pedantic -std=gnu11 -lpthread

PLIK=$1

all:	
	gcc $(CFLAGS) -c $(PLIK).c	#kompiluje 
	gcc $(CFLAGS) -o $(PLIK) $(PLIK).o #linkuje obiekty

clean:
	rm -f $(PLIK).o' > Makefile
#koniec tworzenia Makefile
	fi
        NAZWA=${PLIK%.c}

        make $NAZWA
    else
        echo "$PLIK nie jest plikiem typu C source"
        exit
    fi
else
    echo $PLIK nie jest plikiem typu c;
    exit
fi
