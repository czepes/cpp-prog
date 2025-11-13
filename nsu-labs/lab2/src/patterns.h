#ifndef PATTERNS
#define PATTERNS

#include "simulator.h"

bool put_cell(Cells &cells, int y, int x);
bool put_block(Cells &cells, int y, int x, int height = 2, int width = 2,
               bool fill = true);
bool put_beehive(Cells &cells, int y, int x);
bool put_loaf(Cells &cells, int y, int x);
bool put_boat(Cells &cells, int y, int x);
bool put_tub(Cells &cells, int y, int x);
bool put_blinker(Cells &cells, int y, int x);
bool put_toad(Cells &cells, int y, int x);
bool put_beacon(Cells &cells, int y, int x);
bool put_pulsar(Cells &cells, int y, int x);
bool put_pentadecathlon(Cells &cells, int y, int x);
bool put_glider(Cells &cells, int y, int x);
bool put_lwss(Cells &cells, int y, int x);
bool put_mwss(Cells &cells, int y, int x);
bool put_hwss(Cells &cells, int y, int x);
bool put_glider_gun(Cells &cells, int y, int x);

bool find_block(Cells &cells, int y, int x, int height = 2, int width = 2,
                bool fill = true);

#endif
