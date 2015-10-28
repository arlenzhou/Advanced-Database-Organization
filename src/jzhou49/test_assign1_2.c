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
  testMultiPageContent(); 

  return 0;
}

/**************************************************************************************
 * Function Name: testMultiPageContent
 *
 * Description:
 *		test multiple pages content
 *
 * Parameters:
 *		SM_FileHandle: store the information of page file
 *		SM_PageHandle: current page content
 *		int i: for the iteration count
 *
 * Return:
 *		NONE
 *
 * Author:
 *		Jie Zhou <jzhou49@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                Content
 *		----------  ----------------------------------	------------------------
 *		2015-02-16  Jie Zhou <jzhou49@hawk.iit.edu>     Initialization..
 **************************************************************************************/
void
testMultiPageContent(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = "test multiple pages content";

  ph = (SM_PageHandle) malloc(PAGE_SIZE);

  // create a new page file
  TEST_CHECK(createPageFile (TESTPF));
  TEST_CHECK(openPageFile (TESTPF, &fh));
  printf("created and opened file\n");
  
  // read first page into handle
  TEST_CHECK(readFirstBlock (&fh, ph));
  // the page should be empty (zero bytes)
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
  printf("first block was empty\n");

  // append empty block
  TEST_CHECK(appendEmptyBlock(&fh));
  

  //check the appended block content
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in second page of freshly initialized page");
  printf("second block was empty\n");

  // change ph to be a string and write that one to disk
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = '1';
  TEST_CHECK(writeCurrentBlock (&fh, ph));
  printf("writing second/current block\n");
  if(getBlockPos(&fh) == 1)
	printf("current block position is correct");
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == i), "expected one byte in second page");
  printf("second block was all one\n");


  // read previous page, first block
  TEST_CHECK(readPreviousBlock(&fh,ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page");
  printf("previous block was empty\n");
  if(getBlockPos(&fh) == 0)
	printf("current block position is correct\n");
  
  // read next page, second block
  TEST_CHECK(readNextBlock(&fh,ph));
  printf("%d\n",getBlockPos(&fh));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 1), "expected one byte in second page");
  printf("next block was one bytes\n");
  if(getBlockPos(&fh) == 1)
	printf("current block position is correct");


  // ensure the block is 3
  TEST_CHECK(ensureCapacity(3,&fh));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
  printf("third block was empty\n");

  printf("hahaha%d\n",getBlockPos(&fh));

  // change ph to be a string and write that one to disk
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = (i % 10) + '0';
  TEST_CHECK(writeCurrentBlock (&fh, ph));
  printf("writing last block\n");

  // read last block
  TEST_CHECK(readLastBlock(&fh,ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == (i % 10)+ '0' ), "expected one to nine byte in last/third page");
  printf("third block was correct\n");
 
  /*newly added end*/

  // destroy new page file
  TEST_CHECK(destroyPageFile (TESTPF));  
  
  TEST_DONE();
}
