/*Xiaolang Wang, 2015/02/17, CS525_project_1,
 * codes of some failure tests for different functions
 * include readPreviousBlock and readNextBlock
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "storage_mgr.c"

// test name
char *testName;

/* test output files */
#define TESTPF "test_pagefile.bin"

int main(void){
	extern SM_FileHandle fh;
	extern SM_PageHandle ph;

	testName = "test multiple pages content";
	ph = (SM_PageHandle) malloc(PAGE_SIZE);

//test of Create with a close in it
	createPageFileTest1(TESTPF);

//test of Create with a remove in it
	createPageFileTest2(TESTPF);

//create and open page file
	createPageFile (TESTPF);
	openPageFile (TESTPF, &fh);
	strcmp(fh.fileName, TESTPF);
	fh.totalNumPages = 1;

//test of readPreviousBlock the case current page is the first page
	fh.curPagePos = 0; // set the curPagePos to the first page
	printf("%d\n",readPreviousPage(&fh,ph));

//test of readNextBlock
	fh.curPagePos = fh.totalNumPages - 1; // set the curPagePos to the last page
	printf("%d\n",readNextPage(&fh,ph));

return 0;
}


//Test1 is for close the file before read
RC createPageFileTest1(char *fileName) {
	RC rc = -99; // return value.

	FILE *fp = fopen(fileName, "wb+");

	// check the fp, if the fp is NULL, return msg
	if (fp == NULL) {
		return RC_FILE_NOT_FOUND;
	}

	SM_PageHandle emptyPage = (SM_PageHandle) calloc(PAGE_SIZE, sizeof(char));

	////////////////////////////////////////////////////////////////////////////////
	//close the file;
	fclose(fileName);
	////////////////////////////////////////////////////////////////////////////////

	// write an empty page into the file
	int writeStatus = fwrite(emptyPage, sizeof(char), PAGE_SIZE, fp);

	if (writeStatus == 0) {
		rc = RC_WRITE_FILE_FAILED;
	} else {
		rc = RC_OK;
	}

	// free heap memory
	free(emptyPage);

	// close file
	int closeLabel = fclose(fp);

	if (closeLabel == EOF) {
		rc = RC_CLOSE_FILE_FAILED;
	}

	return rc;
}

//Test2 is for remove the file before read
RC createPageFileTest2(char *fileName) {
	RC rc = -99; // return value.

	FILE *fp = fopen(fileName, "wb+");

	// check the fp, if the fp is NULL, return msg
	if (fp == NULL) {
		return RC_FILE_NOT_FOUND;
	}

	SM_PageHandle emptyPage = (SM_PageHandle) calloc(PAGE_SIZE, sizeof(char));

	////////////////////////////////////////////////////////////////////////////////
	//close the file;
	remove(fileName);
	////////////////////////////////////////////////////////////////////////////////

	// write an empty page into the file
	int writeStatus = fwrite(emptyPage, sizeof(char), PAGE_SIZE, fp);

	if (writeStatus == 0) {
		rc = RC_WRITE_FILE_FAILED;
	} else {
		rc = RC_OK;
	}

	// free heap memory
	free(emptyPage);

	// close file
	int closeLabel = fclose(fp);

	if (closeLabel == EOF) {
		rc = RC_CLOSE_FILE_FAILED;
	}

	return rc;
}
