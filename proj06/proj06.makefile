#
# Gabriella Kelsch Bledsoe
# Project 6 Makefile
# Usage: make -f proj06.makefile
# Makes: proj06
#

proj06: proj06.student.o
					g++ -pthread proj06.student.o -o proj06

proj06.student.o: proj06.student.c
					g++ -Wall -c proj06.student.c
