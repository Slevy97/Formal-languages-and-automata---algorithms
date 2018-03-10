#Makefile

CC = g++ -std=c++11
CFLAGS = -c -Wall

all: lnfa_to_nfa nfa_to_dfa dfa_min
	lnfa_to_nfa
	nfa_to_dfa
	dfa_min

lnfa_to_nfa: lnfa_to_nfa.o
	$(CC) lnfa_to_nfa.o -o lnfa_to_nfa.exe
	DEL lnfa_to_nfa.o

lnfa_to_nfa.o: lnfa_to_nfa.cpp
	$(CC) $(CFLAGS) lnfa_to_nfa.cpp -o lnfa_to_nfa.o

nfa_to_dfa: nfa_to_dfa.o
	$(CC) nfa_to_dfa.o -o nfa_to_dfa.exe
	DEL nfa_to_dfa.o

nfa_to_dfa.o: nfa_to_dfa.cpp
	$(CC) $(CFLAGS) nfa_to_dfa.cpp -o nfa_to_dfa.o

dfa_min: dfa_min.o
	$(CC) dfa_min.o -o dfa_min.exe
	DEL dfa_min.o

dfa_min.o: dfa_min.cpp
	$(CC) $(CFLAGS) dfa_min.cpp -o dfa_min.o

clean: 
	DEL nfa_to_dfa.o lnfa_to_nfa.o dfa_min.o
	DEL nfa_to_dfa.exe lnfa_to_nfa.exe dfa_min.exe