/**
 * maxcalc is a command line tool for multiple precision arithmetic
 * calculations.
 * Copyright © 2018 Max Wällstedt <max.wallstedt@gmail.com>
 *
 * This file is part of maxcalc.
 *
 * maxcalc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * maxcalc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with maxcalc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "maxcalc.h"

static void maxcalc_from_tty()
{
	char *line = NULL, *prev = NULL;

	using_history();

	while ((line = readline("> ")) != NULL) {
		if (line[0] != '\0') {
			/* Perform maxcalc on non-empty line. */

			maxcalc(line);

			/* Add line to history if it is not identical
			   to the previously entered line. */

			if (prev == NULL || strcmp(line, prev) != 0) {
				add_history(line);
			}
		}

		free(prev);
		prev = line;
	}

	free(prev);
	clear_history();
	rl_clear_history();
	rl_set_prompt(NULL);
	fputc('\n', stdout);
}

static void maxcalc_from_file(FILE *input)
{
	char *line = NULL;
	size_t n = 0;
	ssize_t nread;

	while ((nread = getline(&line, &n, input)) != -1) {
		/* Remove trailing newline character. */
		if (nread > 0 && line[nread - 1] == '\n') {
			line[nread - 1] = '\0';
			--nread;
		}

		/* Perform maxcalc on non-empty line. */
		if (line[0] != '\0') {
			maxcalc(line);
		}
	}

	free(line);
}

int main(int argc, char *argv[])
{
	FILE *input;

	if (argc == 1) {
		if (isatty(STDIN_FILENO)) {
			maxcalc_from_tty();
		} else {
			maxcalc_from_file(stdin);
		}
	} else {
		input = fopen(argv[1], "r");

		if (input == NULL) {
			perror(argv[1]);
			exit(EXIT_FAILURE);
		}

		maxcalc_from_file(input);
		fclose(input);
	}

	return 0;
}
