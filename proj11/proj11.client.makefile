#
# Gabriella Kelsch Bledsoe
# Project 11 Makefile
# Usage: make -f proj11.client.makefile
# Makes: proj11.client
#

proj11.client: proj11.client.o
					g++ proj11.client.o -o proj11.client

proj11.client.o: proj11.client.c
					g++ -Wall -c proj11.client.c
