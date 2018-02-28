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

SRCDIR = src
TARGET = maxcalc
CP = cp
RM = rm -f

all: $(TARGET)

$(TARGET): $(SRCDIR)/$(TARGET)
	$(CP) $(SRCDIR)/$(TARGET) $(TARGET)

$(SRCDIR)/$(TARGET):
	$(MAKE) -C $(SRCDIR)

.PHONY: clean

clean:
	$(MAKE) -C $(SRCDIR) clean
	$(RM) $(TARGET)
