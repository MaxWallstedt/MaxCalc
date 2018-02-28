########################################################################
# maxcalc is a command line tool for multiple precision arithmetic     #
# calculations.                                                        #
# Copyright © 2018 Max Wällstedt <max.wallstedt@gmail.com>             #
#                                                                      #
# This file is part of maxcalc.                                        #
#                                                                      #
# maxcalc is free software: you can redistribute it and/or modify      #
# it under the terms of the GNU General Public License as published by #
# the Free Software Foundation, either version 3 of the License, or    #
# (at your option) any later version.                                  #
#                                                                      #
# maxcalc is distributed in the hope that it will be useful,           #
# but WITHOUT ANY WARRANTY; without even the implied warranty of       #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
# GNU General Public License for more details.                         #
#                                                                      #
# You should have received a copy of the GNU General Public License    #
# along with maxcalc.  If not, see <http://www.gnu.org/licenses/>.     #
########################################################################

CC = gcc
CFLAGS1 = -g -pedantic -Wall -Wextra -Werror
CFLAGS2 = -g -ansi -pedantic -Wall -Wextra -Werror
LDFLAGS = -lreadline -lgmp
TARGET = maxcalc
RM = rm -f
SRC1 = main
SRC2 = maxcalc

all: $(TARGET)

$(TARGET): $(SRC1).o $(SRC2).o
	$(CC) $(CFLAGS2) -o $(TARGET) $(SRC1).o $(SRC2).o $(LDFLAGS)

$(SRC1).o: $(SRC1).c $(SRC2).h
	$(CC) -c $(CFLAGS1) $(SRC1).c

$(SRC2).o: $(SRC2).c $(SRC2).h
	$(CC) -c $(CFLAGS2) $(SRC2).c

.PHONY: clean

clean:
	$(RM) $(TARGET)
	$(RM) $(SRC1).o
	$(RM) $(SRC2).o
