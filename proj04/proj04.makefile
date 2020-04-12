#
# Gabriella Kelsch Bledsoe
# Project 4 Makefile
# Usage: make -f proj04.makefile
# Makes: proj04
#

proj04: proj04.student.o
					g++ proj04.student.o -o proj04

proj04.student.o: proj04.student.c
					g++ -Wall -c proj04.student.c
