#
# Gabriella Kelsch Bledsoe
# Project 5 Makefile
# Usage: make -f proj05.makefile
# Makes: proj05
#

proj05: proj05.student.o
					g++ -pthread proj05.student.o -o proj05

proj05.student.o: proj05.student.c
					g++ -Wall -c proj05.student.c
