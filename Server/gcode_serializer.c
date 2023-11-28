#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "string_builder.h"

BOOL absolute_mode = NO;
BOOL absolute_mode_flag_set = NO;

const double burnrate_table[] = {0.15, 0.45, 0.85, 1.25};
#define Z_BREADTH 5.0

/* Define a point. */
typedef struct point_t{
    double x;
    double y;
    double z;
} point_t;

static string_builder_t* gcode_stack;

static void emit_gcode(const char* command, const char** param) {
    string_builder_t* gcode_line = string_builder_init();

    /* Append the main command itself. */
    string_builder_appends(gcode_line, command);
    string_builder_appends(gcode_line, " ");

    for (int i = 0; param[i] != NULL; i++) {
        /* We want it such that the parameters are arranged such
         * that, [a,b,c,d] -> "ab cd", this allows us to arrange. We
         * accomplish this using the modulo operator. This allows us
         * to set specific parameter options in the gcode without having
         * to worry about appending another string :-( which would not be
         * fun. */
        if (i % 2 == 0) {
            string_builder_appends(gcode_line, param[i]);
        } else {
            /* Create a space. */
            string_builder_appends(gcode_line, param[i]);
            string_builder_appends(gcode_line, " ");
        }
    }
    /* Create a new line. */
    string_builder_appends(gcode_line, "\n");

    /* Append to the main gcode stack. */
    string_builder_appendsb(gcode_stack, gcode_line);
    /* Free the gcode_line. */
    string_builder_destroy(gcode_line);
}

void set_absolute_distance_mode() {
    absolute_mode = YES;
    absolute_mode_flag_set = YES;

    const char* param[] = {NULL};
    emit_gcode("G90", param);
}

void set_incremental_distance_mode() {
    absolute_mode = NO;
    absolute_mode_flag_set = YES;

    const char* param[] = {NULL};
    emit_gcode("G91", param);
}

void toggle_distance_modes() {
    if(absolute_mode_flag_set == NO) {
        set_absolute_distance_mode();
    } else if (absolute_mode == YES) {
        set_incremental_distance_mode();
    } else {
        set_absolute_distance_mode();
    }
}

void move(BOOL fast, point_t from, point_t to, double feedrate) {
    /* Final strings for the numbers. */
    double x_f = to.x;
    double y_f = to.y;
    double z_f = to.z;

    char x[32];
    char y[32];
    char z[32];
    char feedrate_s[32];

    if (!absolute_mode) {
        x_f = to.x - from.x;
        y_f = to.y - from.y;
        z_f = to.z - from.z;
    }

    snprintf(x, 32, "%.4f", x_f);
    snprintf(y, 32, "%.4f", y_f);
    snprintf(z, 32, "%.4f", z_f);
    snprintf(feedrate_s, 32, "%.0f", feedrate);

    const char* param[] = {"X", x, "Y", y, "Z", z, "F", feedrate_s, NULL};

    if (fast) {
        emit_gcode("G0", param);
    } else {
        emit_gcode("G1", param);
    }
}

/* In seconds. */
void dwell(double dwell_time) {
    char p[32];

    snprintf(p, 32, "%.4f", dwell_time);

    const char* param[] = {"P", p, NULL};
    emit_gcode("G4", param);
}

void move_and_burn(point_t from, point_t to, int burn_index) {
    move(NO, from, to, 100.0);
    from = to;
    to.z -= Z_BREADTH;
    move(NO, from, to, 100.0);
    dwell(burnrate_table[burn_index]);
    from = to;
    to.z += Z_BREADTH;
    move(NO, from, to, 100.0);
}


static const point_t a = {23.4, 0, Z_BREADTH};
static const point_t b = {23.4, 12.2, Z_BREADTH};
int main() {
    gcode_stack = string_builder_init();
    toggle_distance_modes();
    move_and_burn(a, b, 2);
    char* string = string_builder_lwrap(gcode_stack);
    printf("%s\n", string);
    printf("%d\n", (int)string_builder_size(gcode_stack));

    return 0;
}
