#
# Gabriella Kelsch Bledsoe
# Project 12 Makefile
# Usage: make -f proj12.client.makefile
# Makes: proj12.client
#

proj12.client: proj12.client.o
					g++ proj12.client.o -o proj12.client

proj12.client.o: proj12.client.c
					g++ -Wall -c proj12.client.c
