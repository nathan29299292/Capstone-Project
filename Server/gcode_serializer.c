#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "gcode_serializer.h"
#include "util.h"
#include "string_builder.h"


BOOL absolute_mode = NO;
BOOL absolute_mode_flag_set = NO;

const double burnrate_table[] = {0.45, 0.85, 1.25};
const char* output_path = "output.gcode";

static string_builder_t* gcode_stack;
static string_builder_t* gcode_stack_1;
static string_builder_t* gcode_stack_2;

void init_stack() {
    gcode_stack = string_builder_init();
    gcode_stack_1 = string_builder_init();
    gcode_stack_2 = string_builder_init();
}
char* extract_data() {
    char* string = string_builder_lwrap(gcode_stack);
    return string;
}

int extract_data_size() {
   return (int)string_builder_size(gcode_stack);
}

static void emit_gcode(const char* command, const char** param, int stack) {
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
    if (stack == 0) {
        string_builder_appendsb(gcode_stack, gcode_line);
    } else if (stack == 1) {
        string_builder_appendsb(gcode_stack_1, gcode_line);
    } else {
        string_builder_appendsb(gcode_stack_2, gcode_line);
    }
    /* Free the gcode_line. */
    string_builder_destroy(gcode_line);
}

void set_absolute_distance_mode() {
    absolute_mode = YES;
    absolute_mode_flag_set = YES;

    const char* param[] = {NULL};
    emit_gcode("G90", param, 0);
}

void set_incremental_distance_mode() {
    absolute_mode = NO;
    absolute_mode_flag_set = YES;

    const char* param[] = {NULL};
    emit_gcode("G91", param, 0);
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

void move(BOOL fast, point_t from, point_t to, double feedrate, int stack) {
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
        emit_gcode("G0", param, stack);
    } else {
        emit_gcode("G1", param, stack);
    }
}

/* In seconds. */
void dwell(double dwell_time, int stack) {
    char p[32];

    snprintf(p, 32, "%.4f", dwell_time);

    const char* param[] = {"P", p, NULL};
    emit_gcode("G4", param, stack);
}

void move_and_burn(point_t from, point_t to, int burn_index, int stack) {
    move(NO, from, to, 100.0, stack);
    from = to;
    to.z -= Z_BREADTH;
    move(NO, from, to, 100.0, stack);
    dwell(burnrate_table[burn_index], stack);
    from = to;
    to.z += Z_BREADTH;
    move(NO, from, to, 100.0, stack);
}

int dump_gcode_stack() {
    int fd = open(output_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        return -1;
    }

    string_builder_t* final_gcode = string_builder_init();

    string_builder_appendsb(final_gcode, gcode_stack);
    string_builder_appendsb(final_gcode, gcode_stack_1);
    string_builder_appendsb(final_gcode, gcode_stack_2);

    char* gcode = string_builder_lwrap(final_gcode);
    ssize_t size = (ssize_t)string_builder_size(final_gcode);

    ssize_t bytes_written = write(fd, gcode, size);

    if (bytes_written == -1) {
        close(fd);
        return -1;
    }

    if (close(fd) == -1) {
        return -1;
    }

    return 0;
}
