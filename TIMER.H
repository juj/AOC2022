#ifndef TIMER_H
#define TIMER_H

extern volatile long int globalTimeMsecs;

#define tick() globalTimeMsecs

void install_timer();
void uninstall_timer();

#endif
