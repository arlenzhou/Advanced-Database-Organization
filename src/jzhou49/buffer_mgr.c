#include <stdio.h>
#include <stdlib.h>

#include "buffer_mgr.h"
#include "dberror.h"
#include "storage_mgr.h"
#include "dt.h"

/* GLOBAL VARIABLES */
SM_FileHandle *F_HANDLE; // file handler to store file's content and info
SM_PageHandle *MEM_PAGE; // pointer to an area in memory storing the data of a page

/**************************************************************************************
 * Function Name: initPageList
 *
 * Description:
 *		Initialize the PageList to store pages in the Buffer Manager
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *
 * Return:
 *		void
 *
 * Author:
 *		Xin Su <xsu11@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                Content
 *		----------  ----------------------------------  ------------------------
 *		2015-03-15  Xin Su <xsu11@hawk.iit.edu>         Initialization.
 **************************************************************************************/
void initPageList(BM_BufferPool * const bm) {
	PageList *queue = (PageList *) bm->mgmtData;
	PageFrame *pf[bm->numPages]; //-----------'*' miss-------------
	int i;

	for (i = 0; i < bm->numPages; i++) {
		// Allocate memory for a new PageFrame
		pf[i] = (PageFrame*) malloc(sizeof(PageFrame));

		// Initialize the content in the PageFrame
		pf[i]->page->data = NULL;
		pf[i]->page->pageNum = NO_PAGE;
		pf[i]->frameNum = i;
		pf[i]->numReadIO = NO_PAGE;
		pf[i]->numWriteIO = NO_PAGE;
		pf[i]->fixCount = NO_PAGE;
		pf[i]->dirtyFlag = FALSE;
		pf[i]->clockFlag = FALSE;

		pf[i]->previous = NULL;
		pf[i]->next = NULL;

		// Add this new PageFrame to the tail of the PageList

		// if the PageList is in initial state (size = NO_PAGE), then add this new PageFrame as the head
		// else, add this new PageFrame to the tail
		if (queue->size == NO_PAGE) {
			queue->head = pf[i];
		} else {
			queue->tail->next = pf[i];
			pf[i]->previous = queue->tail;
			queue->tail = pf[i];
		}
	}

	// free these memory blocks pf[bm->numPages]/bm->mgmtData when shutting down the Buffer Pool

	// Set all pointers to the initial state
	queue->tail = queue->head;
	queue->current = queue->head;
	queue->size = 0;
}

/**************************************************************************************
 * Function Name: searchPage
 *
 * Description:
 *		Search the requested page in the Buffer Pool,
 *		if found, load the requested page into the BM_pageHandle and return RC_OK
 *		else, return error code
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *		PageNumber pageNum: the page number of the requested page
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xin Su <xsu11@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                Content
 *		----------  ----------------------------------  ------------------------
 *		2015-03-15  Xin Su <xsu11@hawk.iit.edu>         Initialization.
 **************************************************************************************/
RC searchPage(BM_BufferPool * const bm, BM_PageHandle * const page,
		PageNumber pageNum) {
	PageList *queue = (PageList *) bm->mgmtData;

	// if the size of the PageList < 0, then the PageList is not initialized, return RC_PAGELIST_NOT_INITIALIZED
	// else if the size of the PageList = 0, then the PageList is empty, return RC_PAGE_NOT_FOUND
	if (queue->size < 0) {
		return RC_PAGELIST_NOT_INITIALIZED;
	} else if (queue->size == 0) {
		return RC_PAGE_NOT_FOUND;
	}

	/*
	 * If the code comes here, then the PageList is initialized and not empty
	 * The size of the PageList > 0
	 * Search the requested page starting from the head
	 */

	// Set current to the head of the queue
	queue->current = queue->head;

	while (queue->current != queue->tail
			&& queue->current->page->pageNum != pageNum) {
		queue->current = queue->current->next;
	}

	// After the while loop, if the current comes to the tail,
	// then We still need to determine if the tail contains the requested page
	if (queue->current == queue->tail
			&& queue->current->page->pageNum != pageNum) {
		return RC_PAGE_NOT_FOUND;
	}

	/*
	 * If the code comes here, then the requested page is found in the PageList
	 * Set statistics and load the requested page into BM_pageHandle
	 */

	// Set statistics
	queue->current->fixCount++;
	queue->current->numReadIO++;

	// Load the content into BM_PageHandle
	queue->current->page = page;// ---------change position-----------

	return RC_OK;
}

/**************************************************************************************
 * Function Name: appendPage
 *
 * Description:
 *		Read the requested page from the disk and append it to the tail of the PageList
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *		PageNumber pageNum: the page number of the requested page
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xin Su <xsu11@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                Content
 *		----------  ----------------------------------  ------------------------
 *		2015-03-15  Xin Su <xsu11@hawk.iit.edu>         Initialization.
 **************************************************************************************/
RC appendPage(BM_BufferPool * const bm, BM_PageHandle * const page,
		PageNumber pageNum) {
	PageList *queue = (PageList *) bm->mgmtData;

	// Read the requested page from disk into MEM_PAGE
	RC rc = -99; // init return code
	rc = readBlock(pageNum, F_HANDLE, MEM_PAGE);

	if (rc != RC_OK) {
		return rc;
	}

	// if the size of the PageList = 0, then the PageList is empty, add this requested page as the head
	// else, the PageList is not empty, add the requested page to the tail of the PageList
	if (queue->size == 0) {
		queue->head->page->data = MEM_PAGE;
		queue->head->page->pageNum = pageNum;
		queue->head->numReadIO = 1;
		queue->head->numWriteIO = 0;
		queue->head->fixCount = 1;
		queue->head->dirtyFlag = FALSE;
		queue->head->clockFlag = FALSE;

		queue->head->previous = NULL;
		queue->head->next = NULL;

		// Now there is only 1 page, and all pointers point to it

	} else {
		queue->tail->next->page->data = MEM_PAGE;
		queue->tail->next->page->pageNum = pageNum;
		queue->tail->next->numReadIO = 1;
		queue->tail->next->numWriteIO = 0;
		queue->tail->next->fixCount = 1;
		queue->tail->next->dirtyFlag = FALSE;
		queue->tail->next->clockFlag = FALSE;

		queue->tail->next->previous = queue->tail;
		queue->tail->next->next = NULL;
		queue->tail = queue->tail->next;

		// Set the current pointer to the requested page
		queue->current = queue->tail;
	}

	// Increment the size of the PageList
	queue->size++;

	// Load the requested page into BM_PageHandle
	/*page = queue->current->page;*/queue->current->page = page;//---------change the position----------------

	return RC_OK;
}

/**************************************************************************************
 * Function Name: replacePage
 *
 * Description:
 *		Replace the current page with the requested page read from the disk
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *		PageNumber pageNum: the page number of the requested page
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xin Su <xsu11@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                Content
 *		----------  ----------------------------------  ------------------------
 *		2015-03-15  Xin Su <xsu11@hawk.iit.edu>         Initialization.
 **************************************************************************************/
RC replacePage(BM_BufferPool * const bm, BM_PageHandle * const page,
		PageNumber pageNum) {
	PageList *queue = (PageList *) bm->mgmtData;
	RC rc = -99; // init return code

	// If the removable page is dirty, then write it back to the disk before remove it.
	if (queue->current->dirtyFlag == TRUE) {
		MEM_PAGE = queue->current->page->data;
		rc = writeBlock(pageNum, F_HANDLE, MEM_PAGE);

		if (rc != RC_OK) {
			return rc;
		}
	}

	// Read the requested page from disk to the removable page's spot
	rc = -99; // reset return code
	rc = readBlock(pageNum, F_HANDLE, MEM_PAGE);

	if (rc != RC_OK) {
		return rc;
	}

	// Load the requested page to the current PageFrame in the BM_BufferPool
	queue->current->page->data = MEM_PAGE;
	queue->current->page->pageNum = pageNum;
	queue->current->numReadIO = 1;
	queue->current->numWriteIO = 0;
	queue->current->fixCount = 1;
	queue->current->dirtyFlag = FALSE;
	queue->current->clockFlag = FALSE;
	// Don't need to change the previous and next pointers

	// Load the requested into BM_PageHandle
	/*page = queue->current->page;*/queue->current->page = page;

	return RC_OK;
}

/**************************************************************************************
 * Function Name: FIFO
 *
 * Description:
 *		FIFO replacement strategy
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *		PageNumber pageNum: the page number of the requested page
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xin Su <xsu11@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                Content
 *		----------  ----------------------------------  ------------------------
 *		2015-03-15  Xin Su <xsu11@hawk.iit.edu>         Initialization.
 **************************************************************************************/
RC FIFO(BM_BufferPool * const bm, BM_PageHandle * const page,
		PageNumber pageNum) {
	// First search the page in the queue
	RC rc = -99; // init return code
	rc = searchPage(bm, page, pageNum);

	// If find the page, then simply return RC_OK
	if (rc != RC_PAGE_NOT_FOUND) {
		return rc;
	}

	/*
	 * If the code comes here, then the Buffer Manager doesn't have the page loaded.
	 * We have to read the page from the disk and load it into BM_PageHandle.
	 */

	PageList *queue = (PageList *) bm->mgmtData;

	// If the Buffer Manager has vacancy for the requested page,
	// then read the page from the disk and append it to the tail of the PageList
	if (queue->size < bm->numPages) {
		rc = -99; // reset return code
		rc = appendPage(bm, page, pageNum);

		return rc;
	}

	/*
	 * If the code comes here, then neither the Buffer Manager has the page loaded nor has vacancy for the requested page.
	 * The size of the PageList = bm->numPages
	 * We have to replace an existing page in the Buffer Manager with the requested page.
	 */

	// Find the first removable page, starting from head
	// Set the current pointer to the head of the queue to start the traversal
	queue->current = queue->head;

	while (queue->current != queue->tail && queue->current->fixCount != 0) {
		queue->current = queue->current->next;
	}

	// After the while loop, if the current pointer comes to the tail
	// then we still need to determine if the tail's fixCount = 0
	if (queue->current == queue->tail && queue->current->fixCount != 0) {
		return RC_NO_REMOVABLE_PAGE;
	}

	/*
	 * If the code comes here, then a removable page is found, and the current pointer is pointing to it
	 */

	// Replace the removable page with the requested page
	rc = -99; // reset the return value
	rc = replacePage(bm, page, pageNum);

	return rc;
}

/**************************************************************************************
 * Function Name: LRU
 *
 * Description:
 *		LRU replacement strategy
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *		PageNumber pageNum: the page number of the requested page
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xin Su <xsu11@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                Content
 *		----------  ----------------------------------  ------------------------
 *		2015-03-15  Xin Su <xsu11@hawk.iit.edu>         Initialization.
 **************************************************************************************/
RC LRU(BM_BufferPool * const bm, BM_PageHandle * const page, PageNumber pageNum) {
	// Run FIFO first
	RC rc = -99; // init return code
	rc = FIFO(bm, page, pageNum);

	// If FIFO meets error, then return the error code
	if (rc != RC_OK) {
		return rc;
	}

	/*
	 * If the code comes here, then FIFO complete.
	 * Now the current pointer points to the requested page
	 * All wee need to do is to move the current page to the tail
	 */

	PageList *queue = (PageList *) bm->mgmtData;

	// If the requested page is not in the tail spot, then move it to the tail
	if (queue->current != queue->tail) {
		// Remove the current PageFrame.
		queue->current->previous->next = queue->current->next;
		queue->current->next->previous = queue->current->previous;

		// Add the current PageFrame to the tail
		queue->current->previous = queue->tail;
		queue->current->next = NULL;

		queue->tail->next = queue->current;
		queue->tail = queue->tail->next;

		// Now the current pointer still points to the requested page
	}

	return RC_OK;
}

/**************************************************************************************
 * Function Name: CLOCK
 *
 * Description:
 *		CLOCK replacement strategy
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *		PageNumber pageNum: the page number of the requested page
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xin Su <xsu11@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                Content
 *		----------  ----------------------------------  ------------------------
 *		2015-03-15  Xin Su <xsu11@hawk.iit.edu>         Initialization.
 **************************************************************************************/
RC CLOCK(BM_BufferPool * const bm, BM_PageHandle * const page,
		PageNumber pageNum) {
	PageList *queue = (PageList *) bm->mgmtData;

	// First search the page in the queue.
	RC rc = -99; // init return code
	rc = searchPage(bm, page, pageNum);

	// if find the page, set the current page's clockFlag to TRUE
	// else if other errors happen, return rc
	if (rc == RC_OK) {
		queue->current->clockFlag = TRUE;
		return rc;
	} else if (rc != RC_PAGE_NOT_FOUND) {
		return rc;
	}

	/*
	 * If the code comes here, then the Buffer Manager doesn't have the page loaded.
	 * We have to read the page from the disk and load it into BM_PageHandle.
	 */

	// If the Buffer Manager has vacancy for the requested page,
	// then read the page from the disk and append it to the tail of the PageList
	if (queue->size < bm->numPages) {
		rc = -99; // reset return code
		rc = appendPage(bm, page, pageNum);

		if (rc == RC_OK) {
			// Set the clock pointer to the next to the current pointer
			// if the PageList is not full, set the clock pointer to the next to the current pointer
			// else, set the clock pointer to the head
			if (queue->size < bm->numPages) {
				queue->clock = queue->current->next;
			} else if (queue->size == bm->numPages) {
				queue->clock = queue->head;
			}
		}

		return rc;
	}

	/*
	 * If the code comes here, then neither the Buffer Manager has the page loaded nor has vacancy for the requested page.
	 * The size of the PageList = bm->numPages
	 * We have to replace an existing page in the Buffer Manager with the requested page.
	 */

	// Logic of CLOCK replacement strategy
	// Now the current points to should point to the page that was requested last time
	while (queue->clock->clockFlag != FALSE) {
		queue->clock->clockFlag = FALSE;

		if (queue->clock == queue->tail) {
			queue->clock = queue->head;
		} else {
			queue->clock = queue->clock->next;
		}
	}

	// We find the first PageFrame whose clockFlag is FALSE
	// Set the current pointer to the clock pointer
	queue->current = queue->clock;
	/*RC*/rc = -99;// do not need to redefine RC
	rc = replacePage(bm, page, pageNum);

	if (rc == RC_OK) {
		// After the replacement of the requested page, set its clockFlag to TRUE
		queue->clock->clockFlag = TRUE;

		// TO DO - Whether we need to set the clock pointer to the next to the current pointer?
		if (queue->clock == queue->tail) {
			queue->clock = queue->head;
		} else {
			queue->clock = queue->clock->next;
		}
	}

	return rc;
}

// Buffer Manager Interface Pool Handling

/**************************************************************************************
 * Function Name: initBufferPool
 *
 * Description:
 *		Initialize the Buffer Pool
 *
 * Parameters:
 *		BM_BufferPool *const bm: the Buffer Pool Handler that the user wants to initiate
 *		const char * const pageFileName: the name of the requested page file
 *		const int numPages: capacity of the Buffer Pool
 *		ReplacementStrategy strategy; replacement strategy
 *		void *stratData: N/A
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Chengnan Zhao <czhao18@hawk.iit.edu>
 *
 * History:
 *		Date        Name									Content
 *		----------  --------------------------------------	------------------------
 *		2015-03-14  Chengnan Zhao <czhao18@hawk.iit.edu>	Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>				Delete the unused queue
 *															Add comments
 *															Add the judgment of the rc returned by openPageFile
 **************************************************************************************/
RC initBufferPool(BM_BufferPool * const bm, const char * const pageFileName,
		const int numPages, ReplacementStrategy strategy, void *stratData) {
	// make sure the capacity of the Buffer Pool is valid
	if (numPages <= 0) {
		return RC_INVALID_NUMPAGES;
	}

	// init F_HANDLE
	F_HANDLE = (SM_FileHandle *) malloc(sizeof(SM_FileHandle));

	// init MEM_PAGE
	MEM_PAGE = (char *) calloc(PAGE_SIZE, sizeof(char));//not malloc

	// init Storage Manager
	initStorageManager();

	// Open the file with the name of the requested page file
	RC rc = -99; // init return code
	rc = openPageFile(bm->pageFile, F_HANDLE);

	if (rc != RC_OK) {
		return rc;
	}

	// init BM_BufferPool
	bm->pageFile = pageFileName; // set the name of the requested page file
	bm->numPages = numPages; // set the capacity of the Buffer Pool
	bm->strategy = strategy; // set the replacement strategy

	// init the PageList and store the entry in bm->mgmtData
	initPageList(bm);

	return RC_OK;
}

/**************************************************************************************
 * Function Name: shutdownBufferPool
 *
 * Description:
 *		Shut down the buffer pool
 *
 * Parameters:
 *		BM_BufferPool *const bm: buffer pool that users try to shut down
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Chengnan Zhao <czhao18@hawk.iit.edu>
 *
 * History:
 *		Date        Name									Content
 *		----------  --------------------------------------	------------------------
 *		2015-03-14  Chengnan Zhao <czhao18@hawk.iit.edu>	Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>				Add judgment of the rc returned by forceFlushPool
 *															Modify the logic of freeing the PageList
 *															Add comments
 *															Add judgment of the rc returned by closePageFile
 *															Add the free code of F_HANDLE and MEM_PAGE
 **************************************************************************************/
RC shutdownBufferPool(BM_BufferPool * const bm) {
	// Force to flush all pages updated in the Buffer Pool back to the disk
	RC rc = -99; // init return code
	rc = forceFlushPool(bm);

	if (rc != RC_OK) {
		// TO DO
	}

	// Free allocated memory blocks (PageFrame) of the PageList
	PageList *queue = (PageList *) bm->mgmtData;
	queue->current = queue->tail;

	// if the capacity is 1, simply free the only block
	// else (the capacity > 1), free the PageFrame from the tail back to the head
	if (bm->numPages == 1) {
		free(bm->mgmtData);
	} else {
		while (queue->current != queue->head) {
			queue->current = queue->current->previous;
			free(queue->current->next);
		}

		// After the while loop, the current pointer points to the head,
		// then free the only left block
		free(queue->head);
	}

	// Turn off the Storage Manager by closing the page file
	rc = -99; // reset return code
	rc = closePageFile(F_HANDLE);

	if (rc != RC_OK) {
		return rc;
	}

	// Then free the F_HANDLE and MEM_PAGE
	free(F_HANDLE);
	free(MEM_PAGE);

	return RC_OK;
}

/**************************************************************************************
 * Function Name: forceFlushPool
 *
 * Description:
 *		Clear the data in the buffer pool
 *
 * Parameters:
 *		BM_BufferPool *const bm: buffer pool that need to clear the data.
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Chengnan Zhao <czhao18@hawk.iit.edu>
 *
 * History:
 *		Date        Name									Content
 *		----------  --------------------------------------	------------------------
 *		2015-03-14  Chengnan Zhao <czhao18@hawk.iit.edu>	Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>				Modify the PageList queue
 *															Add comments
 *															Add judgment of the rc returned by writeBlock
 **************************************************************************************/
RC forceFlushPool(BM_BufferPool * const bm) {
	PageList *queue = (PageList *) bm->mgmtData;
	RC rc;

	queue->current = queue->head;

	// The loop ends when the current pointer points to the next to the tail (NULL)
	while (queue->current != NULL && queue->current->dirtyFlag == TRUE) {
		// if the page is dirty, write it back to the disk
		rc = -99;
		rc = writeBlock(queue->current->page->pageNum, F_HANDLE,
				queue->current->page->data);

		if (rc != RC_OK) {
			return rc;
		}

		// Set the dirtyFlag to FALSE after the write
		queue->current->dirtyFlag = FALSE;

		queue->current = queue->current->next;
	}

	return RC_OK;
}

// Buffer Manager Interface Access Pages

/**************************************************************************************
 * Function Name: markDirty
 *
 * Description:
 *		Mark the dirty page
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Jie Zhou <jzhou49@hawk.iit.edu>
 *
 * History:
 *		Date        Name								Content
 *		----------  ----------------------------------	----------------------------
 *		2015-03-12  Jie Zhou <jzhou49@hawk.iit.edu>		Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>			Modify the logic of searching the requested page
 *														Add comments
 **************************************************************************************/
RC markDirty(BM_BufferPool * const bm, BM_PageHandle * const page) {
	PageList *queue = (PageList *) bm->mgmtData;
	RC rc = -99; // init the return code

	// Search the requested page
	rc = searchPage(bm, page, page->pageNum);

	if (rc != RC_OK) {
		return rc;
	}

	// Mark the requested page (pointed by the current pointer) dirty
	queue->current->dirtyFlag = TRUE;

	return RC_OK;
}

/**************************************************************************************
 * Function Name: unpinPage
 *
 * Description:
 *		Unpin a page
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Jie Zhou <jzhou49@hawk.iit.edu>
 *
 * History:
 *		Date        Name								Content
 *		----------  ----------------------------------	----------------------------
 *		2015-03-12  Jie Zhou <jzhou49@hawk.iit.edu>		Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>			Modify the logic of unpinning the requested page
 *														Add comments
 **************************************************************************************/
RC unpinPage(BM_BufferPool * const bm, BM_PageHandle * const page) {
	PageList *queue = (PageList *) bm->mgmtData;
	RC rc = -99;

	// Search the requested page
	rc = searchPage(bm, page, page->pageNum);

	if (rc != RC_OK) {
		return rc;
	}

	queue->current->fixCount--;
	return RC_OK;
}

/**************************************************************************************
 * Function Name: forcePage
 *
 * Description:
 *		Write the requested page back to the page file on disk
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Jie Zhou <jzhou49@hawk.iit.edu>
 *
 * History:
 *		Date        Name								Content
 *		----------  ----------------------------------	----------------------------
 *		2015-03-13  Jie Zhou <jzhou49@hawk.iit.edu>		Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>			Modify the logic of forcing to write the requested page back
 *														Add comments
 **************************************************************************************/
RC forcePage(BM_BufferPool * const bm, BM_PageHandle * const page) {
	PageList *queue = (PageList *) bm->mgmtData;
	RC rc = -99;

	// Search the requested page
	rc = searchPage(bm, page, page->pageNum);

	if (rc != RC_OK) {
		return rc;
	}

	// Write the requested page back to the disk
	rc = -99;
	rc = writeBlock(queue->current->page->pageNum, F_HANDLE,
			queue->current->page->data);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

/**************************************************************************************
 * Function Name: pinPage
 *
 * Description:
 *		Pin the page with page number pageNum
 *
 * Parameters:
 *		BM_BufferPool * const bm: Buffer Pool Handler
 *		BM_PageHandle * const page: Buffer Page Handler
 *		PageNumber pageNum: the page number of the requested page
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Jie Zhou <jzhou49@hawk.iit.edu>
 *
 * History:
 *		Date        Name								Content
 *		----------  ----------------------------------	----------------------------
 *		2015-03-17  Jie Zhou <jzhou49@hawk.iit.edu>		Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>			Modify the logic of pinning the requested page
 *														Add comments
 **************************************************************************************/
RC pinPage(BM_BufferPool * const bm, BM_PageHandle * const page,
		const PageNumber pageNum) {
	RC rc = -99; // init the return code

	if (bm->strategy == 0) {
		rc = FIFO(bm, page, pageNum);
	} else if (bm->strategy == 1) {
		rc = LRU(bm, page, pageNum);
	} else if (bm->strategy == 2) {
		rc = CLOCK(bm, page, pageNum);
	}

	return rc;
}

// Statistics Interface

/**************************************************************************************
 * Function Name: getFrameContents
 *
 * Description:
 *		 Returns an array of PageNumbers (of size numPages) where the i-th element
 *		 is the number of the page stored in the i-th page frame.
 *		 An empty page frame is represented using the constant NO_PAGE.
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		PageNumber *: an array of PageNumber of size numPages
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  --------------------------------------	------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>	Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>				Optimize the code in format
 **************************************************************************************/
PageNumber *getFrameContents(BM_BufferPool * const bm) {
	PageList *queue = (PageList *) bm->mgmtData;

	PageNumber pageNumArray[bm->numPages]; // an array of size numPages

	queue->current = queue->head; // set current to head

	int pos = 0; // let pos be the position in array, initially it's 0

	while (queue->current != queue->tail) { // while current.next is not null
		pageNumArray[pos] = queue->current->page->pageNum;

		// put the value at current into the current position of array
		queue->current = queue->current->next;

		// move current to next
		pos++; // pos moves to next position
	}

	// Now the current pointer points to the tail
	// Include the tail's info into the array
	pageNumArray[pos] = queue->current->page->pageNum;

	return pageNumArray;
}

/**************************************************************************************
 * Function Name: getDirtyFlags
 *
 * Description:
 *		 Returns an array of bools (of size numPages) where the i-th element is TRUE
 *		 if the page stored in the i-th page frame is dirty.
 *		 Empty page frames are considered as clean.
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		bool *: an array of bools indicating each page is dirty or not
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  --------------------------------------  ------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>	Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>				Optimize the code in format
 **************************************************************************************/
bool *getDirtyFlags(BM_BufferPool * const bm) {
	PageList *queue = (PageList *) bm->mgmtData;

	bool dirtyFlagArray[bm->numPages]; // an array of size numPages

	queue->current = queue->head; //set current to head

	int pos = 0; // let pos be the position in array, initially it's 0
	while (queue->current != queue->tail) {
		dirtyFlagArray[pos] = queue->current->dirtyFlag;

		// put the value at current into the current position of array
		queue->current = queue->current->next;

		// move current to next
		pos++; // pos moves to next position
	}

	// Now the current pointer points to the tail
	// Include the tail's info into the array
	dirtyFlagArray[pos] = queue->current->dirtyFlag;

	return dirtyFlagArray;
}

/**************************************************************************************
 * Function Name: getFixCounts
 *
 * Description:
 *		 Returns an array of ints (of size numPages) where the i-th element
 *		 is the fix count of the page stored in the i-th page frame.
 *		 Return 0 for empty page frames.
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		int *: an array of fix count of each page in the BufferPool
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  --------------------------------------  ------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>	Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>				Optimize the code in format
 **************************************************************************************/
int *getFixCounts(BM_BufferPool * const bm) {
	PageList *queue = (PageList *) bm->mgmtData;

	int fixCountArray[bm->numPages]; // an array of size numPages

	queue->current = queue->head; //set current to head

	int pos = 0; // let pos be the position in array, initially it's 0
	while (queue->current != queue->tail) {
		fixCountArray[pos] = queue->current->fixCount;

		// put the value at current into the current position of array
		queue->current = queue->current->next;

		// move current to next
		pos++; // pos moves to next position
	}

	// Now the current pointer points to the tail
	// Include the tail's info into the array
	fixCountArray[pos] = queue->current->fixCount;

	return fixCountArray;
}

/**************************************************************************************
 * Function Name: getNumReadIO
 *
 * Description:
 *		Returns the number of pages that have been read from disk since a buffer pool
 *		has been initialized
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		int: a total number of all pages' numReadIO in BufferPool
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  --------------------------------------  ------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>	Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>				Optimize the code in format
 **************************************************************************************/
int getNumReadIO(BM_BufferPool * const bm) {
	PageList *queue = (PageList *) bm->mgmtData;
	int totalNumReadIO = 0;

	queue->current = queue->head; //set current to head

	while (queue->current != queue->tail) {
		totalNumReadIO += queue->current->numReadIO; // add ReadIO of current page to numReadIO
		queue->current = queue->current->next; // move current to next
	}

	// Now the current pointer points to the tail
	// Include the tail's info into the total amount
	totalNumReadIO += queue->current->numReadIO;

	return totalNumReadIO;
}

/**************************************************************************************
 * Function Name: getNumWriteIO
 *
 * Description:
 *		Return the number of pages written to the page file since the buffer pool has
 *		been initialized
 *
 * Parameters:
 *		BM_BufferPool *const bm: a BufferPool
 *
 * Return:
 *		int: a total number of all pages' numWriteIO in BufferPool
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  --------------------------------------  ------------------------
 *		2015-03-11  Xiaolang Wang <xwang122@hawk.iit.edu>	Initialization
 *		2015-03-20	Xin Su <xsu11@hawk.iit.edu>				Optimize the code in format
 **************************************************************************************/
int getNumWriteIO(BM_BufferPool * const bm) {
	PageList *queue = (PageList *) bm->mgmtData;
	int totalNumWriteIO = 0;

	queue->current = queue->head; //set current to head

	while (queue->current != queue->tail) {
		totalNumWriteIO += queue->current->numWriteIO; // add WriteIO of current page to numReadIO
		queue->current = queue->current->next; // move current to next
	}

	// Now the current pointer points to the tail
	// Include the tail's info into the total amount
	totalNumWriteIO += queue->current->numWriteIO;

	return totalNumWriteIO;
}
