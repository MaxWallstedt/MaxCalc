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


#include "maxcalc.h"

#include <stdio.h>
#include <ctype.h>
#include <gmp.h>

static const char *gl_input = NULL;
static size_t gl_pos = 0;
static size_t gl_line = 1;
static size_t gl_col = 1;

static void syntax_error(const char *expected)
{
	fprintf(
		stderr,
		"%lu:%lu:syntax error: expected %s, found '%c'.\n",
		gl_line,
		gl_col,
		expected,
		gl_input[gl_pos]
	);
}

static void skip_ws()
{
	while (isspace(gl_input[gl_pos])) {
		++gl_pos;
		++gl_col;
	}
}

static void mpz_power(mpz_t rop, const mpz_t base, const mpz_t exp)
{
	/* Exponentiation by squaring. */

	mpz_t x, y, n, one, two;

	/* Initialise. */

	mpz_init_set(x, base);
	mpz_init_set_si(y, 1);
	mpz_init_set(n, exp);
	mpz_init_set_si(one, 1);
	mpz_init_set_si(two, 2);

	/* a^(-b) = (1/a)^b */

	if (mpz_cmp_si(n, 0) < 0) {
		mpz_tdiv_q(x, one, x);
		mpz_neg(n, n);
	}

	while (mpz_cmp_si(n, 1) > 0) {
		if (mpz_even_p(n)) {
			mpz_mul(x, x, x);
			mpz_tdiv_q(n, n, two);
		} else {
			mpz_mul(y, x, y);
			mpz_mul(x, x, x);
			mpz_sub(n, n, one);
			mpz_tdiv_q(n, n, two);
		}
	}

	/* Store the result. */

	mpz_mul(rop, x, y);

	/* Clean up. */

	mpz_clears(x, y, n, one, two, NULL);
}

static int number(mpz_t res)
{
	mpz_t tmp1, tmp2;
	int isneg = 0;

	if (gl_input[gl_pos] == '+') {
		++gl_pos;
		++gl_col;
	} else if (gl_input[gl_pos] == '-') {
		++gl_pos;
		++gl_col;
		isneg = 1;
	}

	skip_ws();

	if (!isdigit(gl_input[gl_pos])) {
		syntax_error("digit");
		return -1;
	}

	mpz_init(tmp2);
	mpz_init_set_si(tmp1, gl_input[gl_pos] - '0');
	++gl_pos;
	++gl_col;

	while (isdigit(gl_input[gl_pos])) {
		mpz_set_si(tmp2, gl_input[gl_pos] - '0');
		mpz_mul_si(tmp1, tmp1, 10);
		mpz_add(tmp1, tmp1, tmp2);
		++gl_pos;
		++gl_col;
	}

	if (isneg) {
		mpz_neg(tmp1, tmp1);
	}

	mpz_set(res, tmp1);
	mpz_clears(tmp1, tmp2, NULL);

	return 0;
}

static int expression(mpz_t res);

static int power(mpz_t res)
{
	if (gl_input[gl_pos] == '(') {
		/* Advance the input. */

		++gl_pos;
		++gl_col;

		/* Parse parenthesised expression. */

		skip_ws();

		if (expression(res) == -1) {
			return -1;
		}

		skip_ws();

		if (gl_input[gl_pos] != ')') {
			syntax_error("')'");
			return -1;
		}

		/* Advance the input. */

		++gl_pos;
		++gl_col;
	} else {
		if (number(res) == -1) {
			return -1;
		}
	}

	return 0;
}

static int factor(mpz_t res)
{
	mpz_t power1, power2;

	/* Initialise. */

	mpz_inits(power1, power2, NULL);

	/* Parse first power. */

	skip_ws();

	if (power(power1) == -1) {
		mpz_clears(power1, power2, NULL);
		return -1;
	}

	skip_ws();

	/* Parse all following powers. */

	while (gl_input[gl_pos] == '^') {
		/* Advance the input. */

		++gl_pos;
		++gl_col;

		/* Parse next power. */

		skip_ws();

		if (power(power2) == -1) {
			mpz_clears(power1, power2, NULL);
			return -1;
		}

		skip_ws();

		/* Evaluate the operation. */

		mpz_power(power1, power1, power2);
	}

	/* Store the result. */

	mpz_set(res, power1);

	/* Clean up. */

	mpz_clears(power1, power2, NULL);

	return 0;
}

static int term(mpz_t res)
{
	mpz_t factor1, factor2;
	int op;

	/* Initialise. */

	mpz_inits(factor1, factor2, NULL);

	/* Parse first factor. */

	skip_ws();

	if (factor(factor1) == -1) {
		mpz_clears(factor1, factor2, NULL);
		return -1;
	}

	skip_ws();

	/* Parse all following factors. */

	while (gl_input[gl_pos] == '*' || gl_input[gl_pos] == '/') {
		/* Get operation. */

		op = gl_input[gl_pos];

		/* Advance the input. */

		++gl_pos;
		++gl_col;

		/* Parse next factor. */

		skip_ws();

		if (factor(factor2) == -1) {
			mpz_clears(factor1, factor2, NULL);
			return -1;
		}

		skip_ws();

		/* Evaluate the operation. */

		if (op == '*') {
			mpz_mul(factor1, factor1, factor2);
		} else {
			mpz_tdiv_q(factor1, factor1, factor2);
		}
	}

	/* Store the result. */

	mpz_set(res, factor1);

	/* Clean up. */

	mpz_clears(factor1, factor2, NULL);

	return 0;
}

static int expression(mpz_t res)
{
	mpz_t term1, term2;
	int op;

	/* Initialise. */

	mpz_inits(term1, term2, NULL);

	/* Parse first term. */

	skip_ws();

	if (term(term1) == -1) {
		mpz_clears(term1, term2, NULL);
		return -1;
	}

	skip_ws();

	/* Parse all following terms. */

	while (gl_input[gl_pos] == '+' || gl_input[gl_pos] == '-') {
		/* Get operation. */

		op = gl_input[gl_pos];

		/* Advance the input. */

		++gl_pos;
		++gl_col;

		/* Parse next term. */

		skip_ws();

		if (term(term2) == -1) {
			mpz_clears(term1, term2, NULL);
			return -1;
		}

		skip_ws();

		/* Evaluate the operation. */

		if (op == '+') {
			mpz_add(term1, term1, term2);
		} else {
			mpz_sub(term1, term1, term2);
		}
	}

	/* Store the result. */

	mpz_set(res, term1);

	/* Clean up. */

	mpz_clears(term1, term2, NULL);

	return 0;
}

/**
 * Parses 'line' and outputs the results.
 */
void maxcalc(const char *line)
{
	mpz_t res;

	/* Initialise. */

	mpz_init(res);
	gl_input = line;

	/* Parse input. */

	if (expression(res) == 0) {
		/* EOL condition. */

		if (gl_input[gl_pos] != '\0') {
			syntax_error("'\\0'");
		} else {
			/* Output the results. */

			fputs("ans = ", stdout);
			mpz_out_str(stdout, 10, res);
			fputc('\n', stdout);
		}
	}

	/* Clean up. */

	gl_col = 1;
	++gl_line;
	gl_pos = 0;
	gl_input = NULL;
	mpz_clear(res);
}
