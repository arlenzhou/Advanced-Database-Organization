#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// test name
char *testName;

/* test output files */
#define TESTPF "test_pagefile.bin"

/* prototypes for test functions */
static void testCreateOpenClose(void);
static void testSinglePageContent(void);

/* main function running all tests */
int
main (void)
{
  testName = "";

  // test cases added
  testError(); 

  return 0;
}


void
testError(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = "test Error content";

  ph = (SM_PageHandle) malloc(PAGE_SIZE);
  
  if(strcmp("RC_NON_EXISTING_PAGE",readBlock(-1,&fh,ph)) == 0)
  {
	puts("correct return value\n");
  }

  if(strcmp("RC_NON_EXISTING_PAGE",writeBlock(-1,&fh,ph)) == 0)
  {
	puts("correct return value\n");
  }
 
  // destroy new page file
  TEST_CHECK(destroyPageFile (TESTPF));  
  
  TEST_DONE();
}
