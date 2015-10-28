#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dberror.h"
#include "storage_mgr.h"

/* ====================================jzhou49==================================== */
// TODO - ADD comment: for function header
void initStorageManager(void)
{
	//initialize the storageManagement
	printf("StorageManager Initialized.\n");
}

// TODO - ADD comment: for function header
RC createPageFile(char *fileName)
{
	// set up a file pointer fp, open a file with mode "wb+".
	// "wb+" Create an empty file and open it for update (both for input and output).
	// TODO - ADD validation: what if the file already exists?
	FILE *fp = NULL;
	fp = fopen(fileName, "wb+");

	// check the fp, if the fp is NULL, return msg
	if (fp == NULL) {
		return RC_FILE_NOT_FOUND;
	}

	// Allocates a ock of memory for an array of PAGE_SIZE elements,
	// initializes all its bits to zero.
	char *written;
	int status;

	// TODO - ADD comment: for variables
	// TODO - UPDATE variables' name
	written = (char*)calloc(PAGE_SIZE, sizeof(char));
	status = fwrite(written, PAGE_SIZE, sizeof(char), fp);

	if (status == 0) {
		return RC_WRITE_FAILED;
	}
	else
		return RC_OK;
}

// TODO - ADD comment: for function header
RC openPageFile(char *fileName, SM_FileHandle *fHandle)
{
	int num; // page numbers of the file

	// open the existed file
	FILE *fp = NULL;
	fp = fopen(fileName, "r+");

	// check status open file
	if (fp == NULL) {
		return RC_FILE_NOT_FOUND;
	}

	// calculate the file's number of pages 
	fseek(fp, 0L, SEEK_END);
	num = ftell(fp) / PAGE_SIZE;
	fseek(fp, 0L, SEEK_SET);

	// write the file's information into fHandle
	fHandle->fileName = fileName;
	fHandle->totalNumPages = num;
	fHandle->curPagePos = 0;
	fHandle->mgmtInfo = fp;

	return RC_OK;
}

// TODO - ADD comment: for function header
RC closePageFile(SM_FileHandle *fHandle)
{
	// close the file and capture the status.
	int status;

	status = fclose(fHandle->mgmtInfo);
	if (status == 0){
		return RC_OK;
	}
	else
		// TODO - ADD definition
		return RC_FILE_CLOSE_FAILED; // newly added msg in dberror.h
}


/* ====================================xwang122========================================= */
/**
* Function Name: destroyPageFile
*
* Description:
*     destroy (delete) a page file.
*
* Parameters:
*     char *fileName: the name of the file that is to delete
*
* Return:
*     RC: return code
*
* Author:
*     Xiaolang Wang <xwang122@hawk.iit.edu>
*
* History:
*     Date        Name                                      Content
*     ----------  ----------------------------------------  ------------------------
*     2015-02-09  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization
*     2015-02-10  Xiaolang Wang <xwang122@hawk.iit.edu>     correct the remove() function call, add comments
**/

RC destroyPageFile(char *fileName){

	//use isRemoved to check if the file is deleted
	int isRemoved = remove(fileName);

	//if isRemoved == 0, the file remove successfully, return RC_OK
	//if not, remove is not successed, return RC_FILE_NOT_FOUND
	if (isRemoved == 0){
		return RC_OK;
	}
	else {
		return RC_FILE_NOT_FOUND;
	}
}


/**
* Function Name: readBlock
*
* Description:
*     The method reads the pageNumth block from a file and stores its content in the memory pointed to by the memPage page handle.
If the file has less than pageNum pages, the method should return RC_READ_NON_EXISTING_PAGE.
*
* Parameters:
*     int pageNum: the number of the page that is about to read
*  	  SM_FileHandle *fHandle: the file handler that contains the file info
*     SM_PageHandle memPage: the page handler that points to the stream to be written to the file
*
* Return:
*     RC: return code
*
* Author:
*     Xiaolang Wang <xwang122@hawk.iit.edu>
*
* History:
*     Date        Name                                      Content
*     ----------  ----------------------------------------  ------------------------
*     2015-02-09  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization
*     2015-02-10  Xiaolang Wang <xwang122@hawk.iit.edu>     correct the all incorrect *fHandle to fHandle,
*															add comments,
*															correct totalPage to totalNumPage,
*															add check length of the read result == PAGE_SIZE
*	  2015-02-11  Xiaolang Wang <xwang122@hawk.iit.edu>     add fseek to find the starting point
**/

RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
	//check if the page number is valid
	//invalid page number includes pageNum > totalNumPage and pageNum < 1
	//if invalid, return RC_READ_NON_EXISTING_PAGE
	if (fHandle->totalNumPage < (pageNum - 1) || pageNum < 1) {
		return RC_READ_NON_EXISTING_PAGE;
	}
	else {
		fHandle->curPagePos = pageNum - 1; // set current page to the entered pageNum - 1 

		long offset = sizeof(char) * (pageNum - 1) * PAGE_SIZE // calculate the offset from the starting point

			//use isSet to check if the pointer is successfully set
			//if isSet != 0, return RC_SET_POINTER_FAILED
			int isSet = fseek(fHandle->mgmtInfo, offset, SEEK_SET);
		if (isSet != 0){
			return RC_SET_POINTER_FAILED;
		}
		else {
			//isRead is used to check if read successfully
			//if isRead != PAGE_SIZE, the read is not successful, return RC_READ_FAILED
			//else return RC_OK
			size_t isRead = fread(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
			if (isRead != PAGE_SIZE) {
				return RC_READ_FAILED;
			}
			else {
				return RC_OK
			}
		}
	}
}


/**
* Function Name: getBlockPos
*
* Description:
*     Return the current page position in a file
*
* Parameters:
*  	  SM_FileHandle *fHandle: the file handle that contains the file info
*
* Return:
*     int: current page number
*
* Author:
*     Xiaolang Wang <xwang122@hawk.iit.edu>
*
* History:
*     Date        Name                                      Content
*     ----------  ----------------------------------------  ------------------------
*     2015-02-09  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization
*     2015-02-10  Xiaolang Wang <xwang122@hawk.iit.edu>     correct the all incorrect *fHandle to fHandle,
*															add comments
**/

int getBlockPos(SM_FileHandle *fHandle){

	//curPagePos in the structure is to store the current page position, simply return it
	return fHandle->curPagePos;
}

/**
* Function Name: readFirstBlock
*
* Description:
*     Return the first page position of a file.
*
* Parameters:
*  	  SM_FileHandle *fHandle: the file handle that contains the file info
*     SM_PageHandle memPage: the page handler that points to the stream to be written to the file
*
* Return:
*     int: current page number
*
* Author:
*     Xiaolang Wang <xwang122@hawk.iit.edu>
*
* History:
*     Date        Name                                      Content
*     ----------  ----------------------------------------  ------------------------
*     2015-02-09  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization
*     2015-02-10  Xiaolang Wang <xwang122@hawk.iit.edu>     correct the all incorrect *fHandle to fHandle,
*															add comments
**/
RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage){
	// call readBlock with pageNum = 1
	return readBlock(1, fHandle, memPage);
}


/* ====================================czhao18==================================== */
/*************************************************************************************
* Function Name: readPreviousBlock
*
* Description:
*		read page from the previous block
*
* Parameters:
*     int numPage: stores the page number which is current page number minus one, should be current page position
*     SM_FileHandle *fHandle: the file handle that contains the file info
*     SM_PageHandle memPage: the page handler that points to the stream to be written to the file
*
* Return:
*     RC: return code
*
* Author:
*     Chengnan Zhao <czhao18@hawk.iit.edu>
*
* History:
*     Date        Name									Content
*     ----------  --------------------------------------	------------------------
*     2015-02-10  Chengnan Zhao <czhao18@hawk.iit.edu>	Add method description
**************************************************************************************/
RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// TODO - ADD comment: for variables
	int numPage = fHandle->curPagePos;
	return readBlock(numPage, fHandle, memPage);
}

/*************************************************************************************
* Function Name: readCurrentBlock
*
* Description:
*     read page from the Current block
*
* Parameters:
*     int numPage: stores the page number which is current page position plus one
*     SM_FileHandle *fHandle: the file handle that contains the file info
*     SM_PageHandle memPage: the page handler that points to the stream to be written to the file
*
* Return:
*     RC: return code
*
* Author:
*     Chengnan Zhao <czhao18@hawk.iit.edu>
*
* History:
*     Date        Name									Content
*     ----------  --------------------------------------  ------------------------
*     2015-02-10  Chengnan Zhao <czhao18@hawk.iit.edu>	Add methods description
**************************************************************************************/
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// TODO - ADD comment: for variables
	int numPage = fHandle->curPagePos + 1;
	return readBlock(numPage, fHandle, memPage);
}

/*************************************************************************************
* Function Name: readNextBlock
*
* Description:
*     read one page from the Next block
*
* Parameters:
*     int numPage: stores the page number which is current page number plus one, should be current page position plus two
*     SM_FileHandle *fHandle: the file handle that contains the file info
*     SM_PageHandle memPage: the page handler that points to the stream to be written to the file
*
* Return:
*     RC: return code
*
* Author:
*     Chengnan Zhao <czhao18@hawk.iit.edu>
*
* History:
*     Date        Name									Content
*     ----------  --------------------------------------  ------------------------
*     2015-02-10  Chengnan Zhao <czhao18@hawk.iit.edu>	Add methods description
**************************************************************************************/
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// TODO - ADD comment: for variables
	int numPage = fHandle->curPagePos + 2;
	return readBlock(numPage, fHandle, memPage);
}

/*************************************************************************************
* Function Name: readLastBlock
*
* Description:
*     read page from the last block
*
* Parameters:
*     int numPage: stores the page number which is last page number
*     SM_FileHandle *fHandle: the file handle that contains the file info
*     SM_PageHandle memPage: the page handler that points to the stream to be written to the file
*
* Return:
*     RC: return code
*
* Author:
*     Chengnan Zhao <czhao18@hawk.iit.edu>
*
* History:
*     Date        Name									Content
*     ----------  --------------------------------------  ------------------------
*     2015-02-10  Chengnan Zhao <czhao18@hawk.iit.edu>	Add methods description
**************************************************************************************/
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// TODO - ADD comment: for variables
	int numPage = fHandle->totalNumPages;
	return readBlock(numPage, fHandle, memPage);
}


/* ====================================xsu11==================================== */
/*************************************************************************************
* Function Name: writeBlock
*
* Description:
*     Write one page to the file (disk) start from absolute position.
*
* Parameters:
*     int pageNum: the start point of the page number in the file
*     SM_FileHandle *fHandle: the file handle that contains the file info
*     SM_PageHandle memPage: the page handler that points to the stream to be written to the file
*
* Return:
*     RC: return code
*
* Author:
*     Xin Su <xsu11@hawk.iit.edu>
*
* History:
*     Date        Name                                Content
*     ----------  ----------------------------------  ------------------------
*     2015-02-09  Xin Su <xsu11@hawk.iit.edu>         Initialization
**************************************************************************************/
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	// If pageNum < 0 or file's totalNumPages <= pageNum,
	// then the page cannot be written to the file.
	// Return RC_WRITE_FAILED.
	if (pageNum < 0 || fHandle->totalNumPages <= pageNum) {
		return RC_WRITE_FAILED;
	}

	// Get the offset and seek the position in the file
	// If the position that supposes to be the start point is not found,
	// then the page cannot be written to the file.
	// Return RC_SEEK_FILE_POSITION_ERROR
	int offset = pageNum * PAGE_SIZE * sizeof(char); // offset in the file from the absolute position
	int seekLabel = fseek(fHandle->mgmtInfo, offset, SEEK_SET); // return label

	if (seekLabel != 0) {
		return RC_SEEK_FILE_POSITION_ERROR;
	}

	// If the writing operation does not complete,
	// then the page is not written to the file completely.
	// Return RC_WRITE_FAILED
	// TODO - what if only part of the stream is written to the file?
	int writtenSize = fwrite(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo); // return size

	if (writtenSize != PAGE_SIZE) {
		return RC_WRITE_FAILED;
	}

	fHandle->curPagePos = pageNum;
	fHandle->totalNumPages++;

	return RC_OK;
}

/*************************************************************************************
* Function Name: writeCurrentBlock
*
* Description:
*     Write one page to the file (disk) start from current position.
*
* Parameters:
*     SM_FileHandle *fHandle: the file handle that contains the file info
*     SM_PageHandle memPage: the page handler that points to the stream to be written to the file
*
* Return:
*     RC: return code
*
* Author:
*     Xin Su <xsu11@hawk.iit.edu>
*
* History:
*     Date        Name                                Content
*     ----------  ----------------------------------  ------------------------
*     2015-02-09  Xin Su <xsu11@hawk.iit.edu>         Initialization
**************************************************************************************/
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC rc = writeBlock(fHandle->curPagePos, fHandle, memPage); // return code
	return rc;
}

/*************************************************************************************
* Function Name: appendEmptyBlock
*
* Description:
*     Write one empty page to the file (disk) start from current position.
*
* Parameters:
*     SM_FileHandle *fHandle: the file handle that contains the file info
*
* Return:
*     RC: return code
*
* Author:
*     Xin Su <xsu11@hawk.iit.edu>
*
* History:
*     Date        Name                                Content
*     ----------  ----------------------------------  ------------------------
*     2015-02-09  Xin Su <xsu11@hawk.iit.edu>         Initialization
**************************************************************************************/
RC appendEmptyBlock(SM_FileHandle *fHandle) {
	FILE *fp = fHandle->mgmtInfo; // file pointer to the file being handled
	SM_PageHandle emptyPage = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char)); // create an empty page
	int rc = -99; // return code, initialized as -99

	int writtenSize = fwrite(emptyPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo); // return size

	if (writtenSize != PAGE_SIZE) {
		rc = RC_WRITE_FAILED;
	}
	else {
		fHandle->curPagePos++;
		fHandle->totalNumPages++;
		rc = RC_OK;
	}

	fclose(fp);
	free(emptyPage);

	return rc;
}

/*************************************************************************************
* Function Name: ensureCapacity
*
* Description:
*		If the file has less than numberOfPages pages then increase the size to numberOfPages.
*
* Parameters:
*		int numberOfPages: the number of the pages that the file needs to be increased to
*     SM_FileHandle *fHandle: the file handle that contains the file info
*
* Return:
*     RC: return code
*
* Author:
*     Xin Su <xsu11@hawk.iit.edu>
*
* History:
*     Date        Name                                Content
*     ----------  ----------------------------------  ------------------------
*     2015-02-09  Xin Su <xsu11@hawk.iit.edu>         Initialization
**************************************************************************************/
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {
	if (fHandle->totalNumPages < numberOfPages) {
		for (int i = 0; i < (numberOfPages - fHandle->totalNumPages); i++) {
			appendEmptyBlock(fHandle);
		}
	}
	return RC_OK;
}
