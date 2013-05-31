#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "skein/skein.h"

const u08b_t* xkcdhash = (u08b_t*) "\x5b\x4d\xa9\x5f\x5f\xa0\x82\x80\xfc\x98\x79\xdf\x44\xf4\x18\xc8\xf9\xf1\x2b\xa4\x24\xb7\x75\x7d\xe0\x2b\xbd\xfb\xae\x0d\x4c\x4f\xdf\x93\x17\xc8\x0c\xc5\xfe\x04\xc6\x42\x90\x73\x46\x6c\xf2\x97\x06\xb8\xc2\x59\x99\xdd\xd2\xf6\x54\x0d\x44\x75\xcc\x97\x7b\x87\xf4\x75\x7b\xe0\x23\xf1\x9b\x8f\x40\x35\xd7\x72\x28\x86\xb7\x88\x69\x82\x6d\xe9\x16\xa7\x9c\xf9\xc9\x4c\xc7\x9c\xd4\x34\x7d\x24\xb5\x67\xaa\x3e\x23\x90\xa5\x73\xa3\x73\xa4\x8a\x5e\x67\x66\x40\xc7\x9c\xc7\x01\x97\xe1\xc5\xe7\xf9\x02\xfb\x53\xca\x18\x58\xb6";

int best = 1024;
pthread_mutex_t bestlock;

int hashcmp(const u08b_t* a, const u08b_t* b){
  int tr = 0;
  int i;
  for(i=0;i<16;i++){
    tr += __builtin_popcountll(((uint64_t*) a)[i] ^ ((uint64_t*) b)[i]);
  }
  return tr;
}

void* hasherthread(void* user){
  char input[128];
  strcpy(input, (char*) user);
  Skein1024_Ctxt_t skeinctxt;
  int mybest = 1024;
  u08b_t hash[128];

  while(1){
    Skein1024_Init(&skeinctxt, 1024);
    Skein1024_Update(&skeinctxt, (u08b_t*) input, 128);
    Skein1024_Final(&skeinctxt, hash);
    
    int diff = hashcmp(xkcdhash, hash);

    if(diff < mybest){
      pthread_mutex_lock(&bestlock);
      mybest = diff;
      if(mybest < best){
	best = mybest;
	printf("NEW BEST: %d\n", diff);
	printf("Input: %.128s\n", input);
      } else {
	mybest = best;
      }
      pthread_mutex_unlock(&bestlock);
    }

    //WARNING UGLY HACK
    //MAY LOOP YOUR THREADS, EAT YOUR BABIES
    int i;
    for(i=0;i<128;i++){
      unsigned char c = (unsigned char)hash[i];
      //replace any bytes in input which correspond to alphanumeric characters in hash
      //with the corresponding bytes from hash.
      //There is no guarantee this will not loop pretty quickly, but it doesn't seem to
      //happen and this way is faster than random generator or hex-string-ifying hash.
      if((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122)){
	input[i] = c;
      }
    }
  }
  printf("Done.\n");
  return NULL;
}

int main(int argc, char** argv){
  int threadc = atoi(argv[1]);
  char** inputs = argv + 2;
  pthread_t threads[threadc];
  pthread_mutex_init(&bestlock, NULL);

  int i;
  for(i=0;i<threadc;i++){
    pthread_create(&(threads[i]), NULL, hasherthread, inputs[i]);
  }
  for(i=0;i<threadc;i++){
    pthread_join(threads[i], NULL);
  }
  return EXIT_SUCCESS;
}

