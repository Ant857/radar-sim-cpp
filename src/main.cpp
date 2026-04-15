#include "radar_params.h"
#include <cstdio>
 
void print_radar_params(const RadarParams& p);
 
int main() {
    RadarParams params;
    print_radar_params(params);
    return 0;
}
