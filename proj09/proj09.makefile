#
# Gabriella Kelsch Bledsoe
# Project 9 Makefile
# Usage: make -f proj09.makefile
# Makes: proj09
#

proj09: proj09.student.o
					g++ -pthread proj09.student.o -o proj09

proj09.student.o: proj09.student.c
					g++ -Wall -c proj09.student.c
