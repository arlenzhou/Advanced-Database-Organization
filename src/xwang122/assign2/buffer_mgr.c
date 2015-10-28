#include <stdio.h>

#include "buffer_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"

/* GLOBAL VARIABLES */
SM_FileHandle *F_HANDLE; // file handler to store file's content and info
SM_PageHandle *MEM_PAGE; // pointer to an area in memory storing the data of a page
PageList *STRATEGY_QUEUE; // queue for replacement strategy use

void initStrategyQueue() {

}

RC FIFO() {

}

RC LRU() {

}

/*=================================================================================================*/
// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool * const bm, const char * const pageFileName,
		const int numPages, ReplacementStrategy strategy, void *stratData) {

}

RC shutdownBufferPool(BM_BufferPool * const bm) {

}

RC forceFlushPool(BM_BufferPool * const bm) {

}

/*=================================================================================================*/
// Buffer Manager Interface Access Pages
RC markDirty(BM_BufferPool * const bm, BM_PageHandle * const page) {

}

RC unpinPage(BM_BufferPool * const bm, BM_PageHandle * const page) {

}

RC forcePage(BM_BufferPool * const bm, BM_PageHandle * const page) {

}
RC pinPage(BM_BufferPool * const bm, BM_PageHandle * const page,
		const PageNumber pageNum) {

}

/*==========================================xwang122==============================================*/
// Statistics Interface
/**************************************************************************************
 * Function Name: getFrameContents
 *
 * Description:
 *		 Returns an array of PageNumbers (of size numPages) where the ith element
 *		 is the number of the page stored in the ith page frame.
 *		 An empty page frame is represented using the constant NO_PAGE.
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		PageNumber: an array of PageNumber of size numPages
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
PageNumber *getFrameContents(BM_BufferPool * const bm) {
	PageNumber pageNumberArray[bm->numPages]; // an array of size numPages
	pageNumberArray = malloc(bm->numPages, sizeof(PageNumber)); // allocate memory

	((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->head; //set current to head

	int pos = 0; // let pos be the position in array, initially it's 0
	while(((PageList *)(bm->mgmtData))->current.next != NULL){ // while current.next is not null
		pageNumberArray[pos] = ((PageList *)(bm->mgmtData))->current->page->pageNum;
					// put the value at current into the current position of array
		((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->current->next;
					// move current to next
		pos ++; // pos moves to next position
	}
	pageNumberArray[pos] = ((PageList *)(bm->mgmtData))->current->page->pageNum;

	return pageNumberArray;
}

/**************************************************************************************
 * Function Name: getDirtyFlags
 *
 * Description:
 *		 Returns an array of bools (of size numPages) where the ith element is TRUE
 *		 if the page stored in the ith page frame is dirty.
 *		 Empty page frames are considered as clean.
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		bool: an array of bools indicating each page is dirty or not
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
bool *getDirtyFlags(BM_BufferPool * const bm) {
	bool boolArray[bm->numPages]; // an array of size numPages
	boolArray = malloc(bm->numPages, sizeof(PageNumber)); // allocate memory

	((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->head; //set current to head

	int pos = 0; // let pos be the position in array, initially it's 0
	while(((PageList *)(bm->mgmtData))->current.next != NULL){ // while current.next is not null
		boolArray[pos] = ((PageList *)(bm->mgmtData))->current->dirtyFlag;
					// put the value at current into the current position of array
		((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->current->next;
					// move current to next
		pos ++; // pos moves to next position
	}
	boolArray[pos] = ((PageList *)(bm->mgmtData))->current->dirtyFlag;

	return boolArray;
}

/**************************************************************************************
 * Function Name: getFixCounts
 *
 * Description:
 *		 Returns an array of ints (of size numPages) where the ith element
 *		 is the fix count of the page stored in the ith page frame.
 *		 Return 0 for empty page frames.
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		int: an array of fix count of each page in the BufferPool
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
int *getFixCounts(BM_BufferPool * const bm) {
	int fixCountArray[bm->numPages]; // an array of size numPages
	fixCountArray = malloc(bm->numPages, sizeof(PageNumber)); // allocate memory

	((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->head; //set current to head

	int pos = 0; // let pos be the position in array, initially it's 0
	while(((PageList *)(bm->mgmtData))->current.next != NULL){ // while current.next is not null
		fixCountArray[pos] = ((PageList *)(bm->mgmtData))->current->fixCount;
					// put the value at current into the current position of array
		((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->current->next;
					// move current to next
		pos ++; // pos moves to next position
	}
	fixCountArray[pos] = ((PageList *)(bm->mgmtData))->current->fixCount;

	return fixCountArray;
}

/**************************************************************************************
 * Function Name: getNumReadIO
 *
 * Description:
 *		Returns the number of pages that have been read from disk since a buffer pool
 *		has been initialized.
 *		You code is responsible to initializing this statistic at pool creating time
 *		and update whenever a page is read from the page file into a page frame.
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		int: a total number of all pages' ReadIO in BufferPool
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
int getNumReadIO(BM_BufferPool * const bm) {
	int numReadIO = 0;

	((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->head; //set current to head
	while(((PageList *)(bm->mgmtData))->current.next != NULL){ // while current.next is not null
		numReadIO += ((PageList *)(bm->mgmtData))->current->numReadIO;
							// add ReadIO of current page to numReadIO
		((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->current->next;
							// move current to next
	}
	numReadIO += ((PageList *)(bm->mgmtData))->current->numReadIO;

	return numReadIO;
}

/**************************************************************************************
 * Function Name: getNumWriteIO
 *
 * Description:
 *		Return the number of pages written to the page file since the buffer pool has
 *		been initialized.
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		int: a total number of all pages' WriteIO in BufferPool
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
int getNumWriteIO(BM_BufferPool * const bm) {
	int numWriteIO = 0;

	((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->head; //set current to head

	while(((PageList *)(bm->mgmtData))->current.next != NULL){ // while current.next is not null
		numWriteIO += ((PageList *)(bm->mgmtData))->current->numWriteIO;
							// add WriteIO of current page to numReadIO
		((PageList *)(bm->mgmtData))->current = ((PageList *)(bm->mgmtData))->current->next;
							// move current to next
	}
	numWriteIO += ((PageList *)(bm->mgmtData))->current->numWriteIO;

	return numWriteIO;
}
