/*============================================================================
* ET: embedded test (super-simple embedded testing framework)
* GitHub: https://github.com/QuantumLeaps/Embedded-Test
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005 Quantum Leaps, <state-machine.com>.
*
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
============================================================================*/
#include "et.h" /* ET: embedded test */

/*..........................................................................*/
static void print_str(char const *str);
static void print_dec(unsigned const num);
static void print_summary(unsigned ok);
static void test_end(void);

static unsigned l_test_count;
static unsigned l_skip_count;
static unsigned l_skip_last;

/*..........................................................................*/
int main(int argc, char *argv[]) {
    ET_onInit(argc, argv);

    print_str("\nET embedded test " ET_VERSION
              ", https://github.com/QuantumLeaps/ET\n");
    print_str("---------------- group: ");
    print_str(ET_group_);
    print_str(" -----------------\n");

    ET_run_();

    test_end();
    print_summary(1U);

    ET_onExit(0); /* success */
}

/*..........................................................................*/
int ET_test_(char const *title, int skip) {
    test_end();
    ++l_test_count;
    ET_onPrintChar('[');
    print_dec(l_test_count);
    print_str("] \"");
    print_str(title);
    print_str("\" ");
    if (skip) {
        ++l_skip_count;
    }
    else {
        setup();
        ET_onPrintChar('.');
    }
    l_skip_last = skip;
    return skip == 0;
}
/*..........................................................................*/
static void test_end(void) {
    if (l_test_count > 0) {
        if (l_skip_last) {
            print_str(" SKIPPED\n");
            l_skip_last = 0;
        }
        else {
            teardown();
            print_str(". PASSED\n");
        }
    }
}
/*..........................................................................*/
void ET_fail_(char const *cond, char const *group, int line) {
    print_str(" FAILED\n--> ");
    print_str(group);
    ET_onPrintChar(':');
    print_dec(line);
    ET_onPrintChar(' ');
    print_str(cond);
    ET_onPrintChar('\n');
    print_summary(0U);

    ET_onExit(-1); /* failure */
}

/*..........................................................................*/
static void print_summary(unsigned ok) {
    print_str("------------ ");
    print_dec(l_test_count);
    print_str(" test(s), ");
    print_dec(l_skip_count);
    print_str(" skipped -------------\n");
    print_str(ok ? "OK\n" : "FAILED\n");
}
/*..........................................................................*/
static void print_str(char const *str) {
    for (; *str != '\0'; ++str) {
        ET_onPrintChar(*str);
    }
}

/*..........................................................................*/
static void print_dec(unsigned const num) {
    /* find power of 10 of the first decimal digit of the number */
    unsigned pwr10 = 1U;
    for (; num > (pwr10 * 9U); pwr10 *= 10U) {
    }
    /* print the decimal digits of the number... */
    do {
        ET_onPrintChar((char)('0' + ((num / pwr10) % 10U)));
        pwr10 /= 10U;
    } while (pwr10 != 0U);
}
