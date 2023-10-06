#include <conio.h>
#include <dos.h>

typedef void (interrupt far *INTERRUPT_PTR)(void);

static INTERRUPT_PTR oldTimerInterrupt;
static int timer_interrupt_installed = 0;

volatile long int globalTimeMsecs = 0;

static unsigned long timerCount = 0;

void interrupt far timerHandler(void)
{
	++globalTimeMsecs;
	timerCount += 1193;
	if (timerCount >= 65536)
	{
		timerCount -= 65536;
		oldTimerInterrupt();
	}
	else
		outp(0x20, 0x20);
}

static void set_timer_int(INTERRUPT_PTR ptr, int timer_delay)
{
	// Install our timer interrupt handler
	SREGS s;
	segread(&s);
	REGS r;
	r.x.ax = 0x2508;
	s.ds = FP_SEG(ptr);
	r.x.dx = FP_OFF(ptr);
	int86x(0x21, &r, &r, &s);
	timer_interrupt_installed = (ptr == timerHandler);

	// Set timer resolution to 1ms
	outp(0x43, 0x36);
	outp(0x40, (unsigned char)timer_delay);
	outp(0x40, (unsigned char)(timer_delay>>8));
}

void install_timer()
{
	_disable();

	// Save old timer interrupt vector
	SREGS s;
	segread(&s);
	REGS r;
	r.x.ax = 0x3508;
	int86x(0x21, &r, &r, &s);
	oldTimerInterrupt = (INTERRUPT_PTR)MK_FP(s.es, r.x.bx);

	set_timer_int(timerHandler, 1193);

	_enable();
}

void uninstall_timer()
{
	_disable();
	set_timer_int(oldTimerInterrupt, 0);
	_enable();
}
