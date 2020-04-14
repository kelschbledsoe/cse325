#
# Gabriella Kelsch Bledsoe
# Project 7 Makefile
# Usage: make -f proj07.makefile
# Makes: proj07
#

proj07: proj07.student.o
					g++ -pthread proj07.student.o -o proj07

proj07.student.o: proj07.student.c
					g++ -Wall -c proj07.student.c
