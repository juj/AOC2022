#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

char *read_file_to_string(const char *filename)
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

// Given a multiline string 'str', null-terminates the
// first line out of it, and returns a pointer to the
// start of the next line.
char *extract_line(char *str)
{
  while(*str)
  {
    if (*str == '\n')
    {
      *str = 0;
      return str+1;
    }
    ++str;
  }
  return str;
}

int char_to_score(char ch)
{
  return ch >= 'a' ? ch - 'a' : ch - 'A' + 26;
}

int find_duplicate(char *sack1, char *sack2, char *sack3)
{
  unsigned char a[52] = { 0 };

  while(*sack1) a[char_to_score(*sack1++)]  = 1;
  while(*sack2) a[char_to_score(*sack2++)] |= 2;
  while(*sack3)
  {
    int score = char_to_score(*sack3++);
    if (a[score] == 3) return score;
  }
  assert(0 && "unreachable"); // __builtin_unreachable();
  return 0;
}

int main()
{
  char *input = read_file_to_string("input.txt");
  char *sack1 = input;
  long score = 0;

  while(*sack1)
  {
    char *sack2 = extract_line(sack1);
    char *sack3 = extract_line(sack2);
    char *next_line = extract_line(sack3);
    score += find_duplicate(sack1, sack2, sack3) + 1;
    sack1 = next_line;
  }

  printf("Score: %ld\n", score);
  free(input);
  return 0;
}
