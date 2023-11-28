#ifndef GCODE_SERIALIZER_H
#define GCODE_SERIALIZER_H
#include "util.h"

#define Z_BREADTH 5.0

/* Define a point. */
typedef struct point_t{
    double x;
    double y;
    double z;
} point_t;

void init_stack() ;

char* extract_data();

int extract_data_size();

void set_absolute_distance_mode();

void set_incremental_distance_mode();

void toggle_distance_modes() ;

void move(BOOL fast, point_t from, point_t to, double feedrate) ;

/* In seconds. */
void dwell(double dwell_time);

void move_and_burn(point_t from, point_t to, int burn_index);
#endif
