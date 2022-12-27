#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

char *read(const char *filename)
{
  FILE *handle = fopen(filename, "r");
  fseek(handle, 0, SEEK_END);
  long size = ftell(handle);
  fseek(handle, 0, SEEK_SET);
  char *mem = (char*)malloc(size+1);
  size_t n = fread(mem, 1, size, handle);
  fclose(handle);
  mem[n] = 0;
  return mem;
}

char *next_line(char *str)
{
  while(*str)
    if (*str++ == '\n') break;

  return str;
}

int far *screen = (int far*)MK_FP(0xB800, 0);

int main()
{
#define N 9
  char *stacks[N] = {
   "PFMQWGRT",
   "HFR",
   "PZRVGHSD",
   "QHPBFWG",
   "PSMJH",
   "MZTHSRPL",
   "PTHNML",
   "FDQR",
   "DSCNLPH"
  };

  _setcursortype(_NOCURSOR);
  clrscr();
  int stackLen[N];
  for(int i = 0; i < N; ++i)
  {
    stackLen[i] = strlen(stacks[i]);
    gotoxy(1, i+1);
    printf("%d %s", i+1, stacks[i]);
  }

  gotoxy(1, 11);

  char *input = read("input.txt");
  while(*input)
  {
    int count, from, to;
    int n = sscanf(input, "move %d from %d to %d", &count, &from, &to);
    input = next_line(input);
    if (n != 3) continue;

    --from;
    --to;

    int far *t = screen + 2 + to * 80;
    int far *f = screen + 2 + from * 80;

    for(int i = 0; i < count; ++i)
    {
      --stackLen[from];
      t[stackLen[to]] = (f[stackLen[from]] & 0xFF) | 0x4F00;
      f[stackLen[from]] = 0x1000;
      delay(15);
      f[stackLen[from]] = 0x0700;
      t[stackLen[to]] = (t[stackLen[to]] & 0xFF) | 0x0700;
      ++stackLen[to];
    }
//    for(int i = 0; i < count; ++i)
 //   {
  //    t[stackLen[to]++] = f[--stackLen[from]];
//       t[stackLen[to]++] = f[stackLen[from]-count+i];
  //    }
  //    stackLen[from] -= count;
 //   }
  }
  _setcursortype(_NORMALCURSOR);
  printf("Final answer: ");
  for(i = 0; i < N; ++i)
  {
    screen[2 + stackLen[i] - 1 + i * 80] =
     (screen[2 + stackLen[i] - 1 + i * 80] & 0xFF) | 0x0A00;
    printf("%c", (char)(screen[2 + stackLen[i] - 1 + i * 80] & 0xFF));
  }
  printf("\n");

  return 0;
}
