#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define ABS_ZERO_C (-273.15)
#define EPSILON    1e-9   /* so that typing exactly -273.15 is accepted */
#define MAX_TEMP   1e9    /* strtod parses 1e300; that is not a temperature */

/* Each band covers [previous limit, limit) in Celsius. */
static const struct {
    double      limit;
    const char *name;
    const char *advice;
} BANDS[] = {
    {      0.0, "Freezing",     "Bundle up, it is freezing outside!" },
    {     10.0, "Cold",         "Wear a jacket."                     },
    {     25.0, "Comfortable",  "Enjoy the weather!"                 },
    {     35.0, "Hot",          "Drink lots of water!"               },
    { HUGE_VAL, "Extreme Heat", "Stay indoors and keep cool!"        }
};

static int band_of(double celsius)
{
    int i = 0;
    while (celsius >= BANDS[i].limit)
        i++;
    return i;
}

static void quit_eof(void)
{
    printf("\nEnd of input reached. Exiting.\n");
    exit(1);
}

/* Returns 0 if the line overflowed the buffer, discarding the rest so a
   truncated value is never accepted.  A missing newline can also mean input
   just ended, so the next character is peeked at to tell them apart. */
static int read_line(char *buf, size_t size)
{
    if (fgets(buf, (int)size, stdin) == NULL)
        quit_eof();

    if (strchr(buf, '\n') == NULL) {
        int ch = getchar();
        if (ch != EOF) {
            while (ch != '\n' && ch != EOF)
                ch = getchar();
            return 0;
        }
    }
    return 1;
}

static int blank(const char *s)
{
    while (isspace((unsigned char)*s))
        s++;
    return *s == '\0';
}

static double ask_temperature(void)
{
    char   line[256], *end;
    double value = 0.0;
    int    ok;

    for (;;) {
        printf("Enter the temperature value: ");
        fflush(stdout);

        ok = read_line(line, sizeof line);
        if (ok) {
            value = strtod(line, &end);
            ok = (end != line && blank(end) && isfinite(value));
        }

        if (!ok)
            printf("Invalid input: please enter a number "
                   "(for example 30 or -12.5).\n");
        else if (fabs(value) > MAX_TEMP)
            printf("Invalid input: %g is outside the supported range.\n", value);
        else
            return value;
    }
}

static char ask_scale(const char *prompt)
{
    char line[256], *p, letter;

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);

        if (read_line(line, sizeof line)) {
            p = line;
            while (isspace((unsigned char)*p))
                p++;
            letter = (char)toupper((unsigned char)*p);
            if ((letter == 'C' || letter == 'F' || letter == 'K') && blank(p + 1))
                return letter;
        }
        printf("Invalid input: please enter C, F, or K.\n");
    }
}

static double to_celsius(double value, char scale)
{
    if (scale == 'F') return (value - 32.0) * 5.0 / 9.0;
    if (scale == 'K') return value - 273.15;
    return value;
}

static double from_celsius(double celsius, char scale)
{
    if (scale == 'F') return celsius * 9.0 / 5.0 + 32.0;
    if (scale == 'K') return celsius + 273.15;
    return celsius;
}

int main(void)
{
    double temperature, celsius;
    char   from, to;
    int    band;

    /* Value and scale are asked together: whether a temperature is possible
       depends on the scale it was given in, so both are re-asked. */
    for (;;) {
        temperature = ask_temperature();
        from        = ask_scale("Enter the original scale (C, F, or K): ");
        celsius     = to_celsius(temperature, from);

        if (celsius >= ABS_ZERO_C - EPSILON)
            break;

        printf("Invalid temperature: %.2f %c is below absolute zero. "
               "Please try again.\n\n", temperature, from);
    }

    to   = ask_scale("Enter the scale to convert to (C, F, or K): ");
    band = band_of(celsius);

    printf("Converted temperature: %.2f %c\n", from_celsius(celsius, to), to);
    printf("Temperature category: %s\n", BANDS[band].name);
    printf("Weather advisory: %s\n", BANDS[band].advice);

    return 0;
}
