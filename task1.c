#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define STOP_VALUE 1
#define MAX_SCORE  1000

enum { READ_OK, READ_EOF, READ_INVALID };

static int read_int(int *out)
{
    char line[256], *end;
    long value;

    if (fgets(line, sizeof line, stdin) == NULL)
        return READ_EOF;

    /* No newline means the line overflowed the buffer, or input just ended.
       Peek to tell them apart, and discard the rest of an over-long line so
       a truncated value is never accepted. */
    if (strchr(line, '\n') == NULL) {
        int ch = getchar();
        if (ch != EOF) {
            while (ch != '\n' && ch != EOF)
                ch = getchar();
            return READ_INVALID;
        }
    }

    value = strtol(line, &end, 10);
    if (end == line)
        return READ_INVALID;
    while (isspace((unsigned char)*end))
        end++;
    if (*end != '\0' || value > INT_MAX || value < INT_MIN)
        return READ_INVALID;

    *out = (int)value;
    return READ_OK;
}

static void print_combinations(int score)
{
    int a, b, c, d;

    printf("Possible combinations of scoring plays if a team's score is %d:\n",
           score);

    /* Enumerate the 8, 7, 6 and 3 point counts; safeties take the rest,
       which only works when the remainder is even. */
    for (a = 0; a * 8 <= score; a++) {
        int ra = score - a * 8;
        for (b = 0; b * 7 <= ra; b++) {
            int rb = ra - b * 7;
            for (c = 0; c * 6 <= rb; c++) {
                int rc = rb - c * 6;
                for (d = 0; d * 3 <= rc; d++)
                    if ((rc - d * 3) % 2 == 0)
                        printf("%d TD + 2pt, %d TD + FG, %d TD, "
                               "%d 3pt FG, %d Safety\n",
                               a, b, c, d, (rc - d * 3) / 2);
            }
        }
    }
}

int main(void)
{
    int score;

    for (;;) {
        printf("Enter the NFL score (Enter %d to stop): ", STOP_VALUE);
        fflush(stdout);

        switch (read_int(&score)) {
        case READ_EOF:
            printf("\nEnd of input reached. Exiting.\n");
            return 0;
        case READ_INVALID:
            printf("Invalid input: please enter a whole number.\n\n");
            continue;
        }

        if (score == STOP_VALUE) {
            printf("Goodbye!\n");
            return 0;
        }
        if (score < 0 || score > MAX_SCORE) {
            printf("Invalid input: %d is out of range. "
                   "Please enter a score from 0 to %d.\n\n", score, MAX_SCORE);
            continue;
        }

        print_combinations(score);
        printf("\n");
    }
}
