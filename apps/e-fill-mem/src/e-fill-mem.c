/*
Copyright (C) 2013 Adapteva, Inc.
Contributed by Andreas Olofsson <support@adapteva.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program, see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

#define RAM_SIZE   (0x8000)
#define N          RAM_SIZE/4
void usage();

int main(int argc, char *argv[]){

  e_platform_t platform;
  e_epiphany_t dev;

  unsigned int write_buffer[N];
  unsigned int read_buffer[N];
  int i,j,k;
  int row0,col0,rows,cols;
  unsigned int pattern,addr,read_data;
  int status=1;//pass

  if(argc < 2){
    usage();
    return EXIT_FAILURE;
  }
  else{
    row0    = atoi(argv[1]);
    col0    = atoi(argv[2]);
    rows    = atoi(argv[3]);
    cols    = atoi(argv[4]);
    sscanf(argv[5], "%x", &pattern);
  }

  //Open
  e_init(NULL);
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);

  //Create array
  for (i=0; i<N; i++){
    write_buffer[i] = pattern;
    //printf("%x %x\n",i,buffer[i]);
  }

  //write in buffer
  for (i=row0; i<(row0+rows); i++) {
    for (j=col0; j<(col0+cols); j++) {           
      e_write(&dev, i, j, 0x0, &write_buffer, N*sizeof(int));
    }
  }

  //Checking that writes worked
  for (i=row0; i<(row0+rows); i++) {
    for (j=col0; j<(col0+cols); j++) {   
      for(k=0;k<RAM_SIZE/4;k++){
          addr=4*k;
          e_read(&dev, i, j, addr, &read_data, sizeof(int));
          read_buffer[k]=read_data;
      }
      for(k=0;k<RAM_SIZE/4;k=k+1){
	if(read_buffer[k]!=pattern){
	  printf("FAIL (%d,%d) k=%d data=%x\n",i,j,k,read_buffer[k]);
	  status=0;
	}
      }
    }
  }
  //Close
  e_close(&dev);
  e_finalize();
  
  if(status){
    return EXIT_SUCCESS;
  }
  else{
    return EXIT_FAILURE;
  }
}
void usage(){
  printf("---------------------------------------------------\n");
  printf("Function: Fills memory with a constant pattern\n");
  printf("Usage:    e-fill-mem <row> <col> <rows> <cols> <pat>\n");
  printf("Example:  e-fill-mem  0 0 4 4 0x12345678\n");
  printf("\n");
  printf("Options:\n");
  printf("  row   - target core row coordinate\n");
  printf("  col   - target core column coordinate\n");
  printf("  rows  - number of rows to dump\n");
  printf("  cols  - number of columns to dump\n");
  printf("  pat   - 32-bit hex pattern starting with 0x\n");
  printf("---------------------------------------------------\n");
  return;
}
