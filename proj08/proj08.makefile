#
# Gabriella Kelsch Bledsoe
# Project 8 Makefile
# Usage: make -f proj08.makefile
# Makes: proj08
#

proj08: proj08.student.o
					g++ -pthread proj08.student.o -o proj08

proj08.student.o: proj08.student.c
					g++ -Wall -c proj08.student.c
