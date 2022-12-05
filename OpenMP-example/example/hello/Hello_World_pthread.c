///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006 Intel Corporation 
// All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met: 
//
// * Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
// * Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution. 
// * Neither name of Intel Corporation nor the names of its contributors 
// may be used to endorse or promote products derived from this software 
// without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

// This code is for Chapter 2
// Reference to Listing 2.2
// 
// Possible compilation instructions under Linux:
// 
// Intel Compiler: icc -lpthread -o hello_pthread Hello_World_pthread.c
// gcc compiler:   gcc -lpthread -o hello_pthread Hello_World_pthread.c  
// g++ compiler:   g++ -lpthread -o hello_pthread Hello_World_pthread.c 
//
// Note: (1) There is a Makefile in this package as well. 
//           The Makefile is used for both OpenMP and pthread examples. 
//           Make sure to install Intel Compiler before using given Makefile
//       (2) This code is being tested under 32-bit OS environment only. 
//           For EM64T, you might encounter some warnings, but the executable
//           is going to run anyway. 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	5

void *PrintHello(void *threadid)
{
   printf("\n%d: Hello World!\n", threadid);
   pthread_exit(NULL);
}

int 
main(int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc, t;
   for (t=0; t < NUM_THREADS; t++) {
      printf("Creating thread %d\n", t);
      rc = pthread_create( &threads[t], NULL,
                           PrintHello,(void *)t);
      if (rc) {
        printf("ERROR return code from pthread_create(): %d\n",
               rc);
        exit(-1);
      }
   }
   pthread_exit(NULL);
}
