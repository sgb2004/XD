#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void INIT();
void READ();
void WRITE();
void EUP();
void LOAD();
void START();
void TERMINATE();

struct Machine {
  char mem[100][4];
  char IR[4];
  char C;
  int IC;
  char R[4];
  int SI;
} M;

FILE* ProgramCard;
FILE* LinePrinter;

void assign(char* A, const char* B) {
  memcpy(A, B, 4);
}

int num(char IR[4]) {
  return (IR[2]-'0')*10+(IR[3]-'0');
}

int numb(char IR[4]) {
  return (IR[0]-'0')*1000+(IR[1]-'0')*100+(IR[2]-'0')*10+(IR[3]-'0');
}

void memdump() {
  for(int i=0; i<99; i++) {
    printf("%d\t", i+1);
    for(int j=0; j<4; j++) {
      printf("%c", M.mem[i][j]);
    }
    printf("\n");
  }
}
void mempeak(int n) {
  for(int i=0; i<20; i++) {
    printf("%d\t", i+1);
    for(int j=0; j<4; j++) {
      printf("%c", M.mem[i+n][j]);
    }
    printf("\n");
  }
}

void LOAD() {
  char Buffer[41];
  int load_program =0;
  int rp =0;
  while(fgets(Buffer, 41, ProgramCard)) {
    if(Buffer[0] == '$') {
       if(Buffer[1] == 'A'){
         INIT();
         load_program=1;
       }
       if(Buffer[1] == 'D') {
         START();
         break;
       };
       if(Buffer[1] == 'E') LOAD();
       continue;
    }  

    if(load_program) {
      char *r = Buffer;
      int l=0;
      int i=0;
      while(1) {
        char *m = M.mem[rp+i];
        switch (*r) {
          case 'G' : l=4;break;
          case 'P' : l=4;break;
          case 'L' : l=4;break;
          case 'S' : l=4;break;
          case 'C' : l=4;break;
          case 'B' : l=4;break;
          case 'H' : l=1;break;
        }
        if(*r == 'H'){
          *m = 'H';
          break;
        }
        for(int i=0; i<l ;i++, r++) {
          *(m+i) = *r;
        }
        if(i >= 10) break;
        i++;
      }
      rp+= i;
    }
  }
}

void READ() {
  char Buffer[45];
  fgets(Buffer, 45, ProgramCard);
  int mem = num(M.IR);
  int c=0;
  while(Buffer[c] != '\n' && c<40) {
    M.mem[mem+c/4][c%4] = Buffer[c];
    c++;
  }
}

void WRITE() {
  int mem = num(M.IR);
  for(int i=0; i<10; i++) {
    for(int j=0; j<4; j++)
      fputc(M.mem[mem+i][j], LinePrinter);
  }
  fputc('\n', LinePrinter);
}

void MOS() {
  if(M.SI == 1) READ();
  if(M.SI == 2) WRITE();
  if(M.SI == 3) TERMINATE(); 
}

void EUP() {
  assign(M.IR, M.mem[M.IC]);
  M.IC++;
  if(M.IR[0] == 'L') assign(M.R, M.mem[num(M.IR)]);
  if(M.IR[0] == 'S') assign(M.mem[num(M.IR)], M.R);
  if(M.IR[0] == 'C') M.C = memcmp(M.mem[num(M.IR)], M.R, 4) == 0; 
  if(M.IR[0] == 'B') if(M.C) { M.IC = num(M.IR); M.C = 0; }
  if(M.IR[0] == 'H'){ M.SI=3; MOS(); return;}
  if(M.IR[0] == 'G'){ M.SI=1; MOS(); }
  if(M.IR[0] == 'P'){ M.SI=2; MOS(); }
  EUP();
}

void INIT() {
  memset(&M, ' ', sizeof(struct Machine));
}

void START() {
  M.IC = 0;
  EUP();
}

void TERMINATE() {
  fputc('\n', LinePrinter);
  fputc('\n', LinePrinter);
  INIT();
  LOAD();
}

int main() {
  ProgramCard = fopen("./input.txt", "r");
  LinePrinter = fopen("./output.txt", "w");

  if(ProgramCard == NULL || LinePrinter == NULL) {
    printf("ERROR OPENING input and output files, program exsiting");
    exit(0);
  }
  LOAD();
}
