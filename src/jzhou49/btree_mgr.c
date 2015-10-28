//2015/04/26, Xiaolang Wang

#include "record_mgr.h"
#include "dberror.h"
#include "tables.h"
#include "buffer_mgr.h"
#include "tables.h"

//Global Veriables
int NUM_NODES = -1;
int NUM_ENTRIES = -1;
DataType KEY_TYPE = 0;
int TYPE_LENGTH = sizeof(int);
int N = -1;

BM_BufferPool * BUFFER_POOL;
BTreeHandle * BTREE_HANDLE;
BT_ScanHandle * SCAN_HANDLE;

//Prototypes
RC initIndexManager(void *mgmtData); // done
RC shutdownIndexManager(); // done
RC createBtree(char *idxId, DataType keyType, int n); // done
RC openBtree(BTreeHandle **tree, char *idxId); // done
RC closeBtree(BTreeHandle *tree); // done
RC deleteBtree(char *idxId); // done

RC getNumNodes(BTreeHandle *tree, int *result); // done
RC setNumNodes(BTreeHandle *tree, int value); // done
RC getNumEntries(BTreeHandle *tree, int *result); // done
RC setNumEntries(BTreeHandle *tree, int value); // done
RC getKeyType(BTreeHandle *tree, DataType *result); // done
RC setKeyType(BTreeHandle *tree, DataType value); // done
int getTypeLength(BTreeHandle *tree); // done
RC setTypeLength(BTreeHandle *tree, int value); // done
int getN(BTreeHandle *tree); // done
RC setN(BTreeHandle *tree, int value); // done

RC initNodePage(BTreeHandle *tree, PageNumber pageNum); //done
RC findKey(BTreeHandle *tree, Value *key, RID *result);
RC insertKey(BTreeHandle *tree, Value *key, RID rid);
RC deleteKey(BTreeHandle *tree, Value *key);
RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle);
RC nextEntry(BT_ScanHandle *handle, RID *result);
RC closeTreeScan(BT_ScanHandle *handle);

char *printTree(BTreeHandle *tree);

/////////////////////// init and shutdown index manager /////////////////////////////
/**************************************************************************************
 * Function Name: initInfexManager
 *
 * Description:
 *		Create an index manager with given buffer manager entry
 *
 * Parameters:
 * 		void * mgmtData: buffer manager entry
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC initIndexManager(void *mgmtData) {

	RC rc;

	// init BUffer Manager
	// init parameters for Buffer Manager
	if (BUFFER_POOL == NULL) {
		BUFFER_POOL = (BM_BufferPool *) malloc(sizeof(BM_BufferPool));
	}

	ReplacementStrategy strategy = RS_LRU;
	int numPages = 10;

	// Call initBufferPool() to init Buffer Manager (10, LRU)
	// Shutdown it in shutdownRrecordManager()
	rc = -99;
	rc = initBufferPool(BUFFER_POOL, "", numPages, strategy, NULL);

	if (rc != RC_OK) {
		return rc;
	}

	//allocate memory for global tree handle and scan handle
	BTREE_HANDLE = (BTreeHandle *) malloc(sizeof(BTreeHandle));
	SCAN_HANDLE = (BT_ScanHandle *) malloc(sizeof(BT_ScanHandle));

	return RC_OK;
}
/**************************************************************************************
 * Function Name: shutdownIndexManager
 *
 * Description:
 *		close and free the index manager
 *
 * Parameters:
 * 		n/a
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC shutdownIndexManager() {
	RC rc;

	// Shutdown Buffer Manager
	rc = -99;
	rc = shutdownBufferPool(BUFFER_POOL);

	if (rc != RC_OK) {
		return rc;
	}

	free(BUFFER_POOL);
	BUFFER_POOL = NULL;

	free(BTreeHandle);
	BTreeHandle = NULL;

	free(BT_ScanHandle);
	BT_ScanHandle = NULL;

	return RC_OK;
}

///////////////////// create, destroy, open, and close an btree index /////////////////
/**************************************************************************************
 * Function Name: createBtree
 *
 * Description:
 *		Create a B+ Tree with given name, DataType and number of elements in a node
 *
 * Parameters:
 * 		char *idxId: name of the B+ Tree
 * 		DataType keyType: data type of the tree
 * 		int n: number of elements in one node
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC createBtree(char *idxId, DataType keyType, int n) {
	//
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	// init the return code
	RC rc;

	//let keyType equals to global KEY_TYPE
	KEY_TYPE = keyType;

	// createPageFile
	rc = -99;
	rc = createPageFile(idxId);

	if (rc != RC_OK) {
		return rc;
	}

	//pin page 0, and write NUM_NODES, NUM_ENTRIES, KEY_TYPE = 0 (4 bytes each)
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	int *ptr = (int *) page->data;
	ptr[0] = NUM_NODES;
	ptr[1] = NUM_ENTRIES;
	ptr[2] = keyType;
	ptr[3] = TYPE_LENGTH;
	ptr[4] = n;

	// unpin page 0
	rc = -99;
	rc = pinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

/**************************************************************************************
 * Function Name: openBtree
 *
 * Description:
 *		Open a B+ Tree with given name
 *
 * Parameters:
 * 		BTreeHandle **tree: A pointer to an array of B+ Tree, we will only use the first spot
 * 		char *idxId: Name of the B+ Tree
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC openBtree(BTreeHandle **tree, char *idxId) {

	// Set the name of the file to the Buffer Manager
	BUFFER_POOL->pageFile = idxId;

	// Set the tree structure
	tree->idxId = idxId;

	// Store the entry of the Buffer Manager here in the tree structure
	tree->mgmtData = BUFFER_POOL;

	// store the entry of the tree in the scan_handle structure
	SCAN_HANDLE->tree = tree;

	//printf("END\n\n");
	return RC_OK;
}

/**************************************************************************************
 * Function Name: closeBtree
 *
 * Description:
 *		Close given B+ Tree
 *
 * Parameters:
 * 		BTreeHandle *tree: Entry to the B+ Tree we about to close
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC closeBtree(BTreeHandle *tree) {
	RC rc;

	// Write all updated content in the Buffer Pool
	rc = -99;
	rc = forceFlushPool(BUFFER_POOL);

	if (rc != RC_OK) {
		return rc;
	}
	// Unset the file name in Buffer Manager
	BUFFER_POOL->pageFile = NULL;

	// Unset the tree info
	tree->idxId = NULL;
	tree->keyType = NULL;
	tree->mgmtData = NULL;

	// unset the SCAN_HANDLE structure
	SCAN_HANDLE->mgmtData = NULL;
	SCAN_HANDLE->tree = NULL;

	// unset the BTREE_HANDLE structure
	BTREE_HANDLE->idxId = NULL;
	BTREE_HANDLE->keyType = NULL;
	BTREE_HANDLE->mgmtData = NULL;

	//printf("END\n\n");
	return RC_OK;
}
/**************************************************************************************
 * Function Name: deleteBtree
 *
 * Description:
 *		Delete B+ Tree with given name
 *
 * Parameters:
 * 		char *idxId: name of the B+ Tree we about to delete
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC deleteBtree(char *idxId) {
	RC rc;

	rc = destroyPageFile(idxId);

	if (rc != RC_OK) {
		return rc;
	}

	//printf("END\n\n");
	return RC_OK;
}

////////////////////////// access information about a b-tree ///////////////////////////////
/**************************************************************************************
 * Function Name: getNumNodes
 *
 * Description:
 *		Get the number of nodes in given B+ Tree
 *
 * Parameters:
 * 		BTreeHandle *tree: Entry to the B+ Tree
 * 		int *result: Pointer to the number of the nodes
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC getNumNodes(BTreeHandle *tree, int *result) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	// init the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// get the first # of page 0, is # of nodes
	int *ptr = (int *) page->data;
	*result = ptr[0];

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

RC setNumNodes(BTreeHandle *tree, int value) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	//set the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// set the first # of page value
	int *ptr = (int *) page->data;
	ptr[0] = value;
	page->data = (char *) ptr;

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

/**************************************************************************************
 * Function Name: getNumEtries
 *
 * Description:
 *		Get the number of entries in given B+ Tree
 *
 * Parameters:
 * 		BTreeHandle *tree: Entry to the B+ Tree
 * 		int *result: Pointer to the number of the entries
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC getNumEntries(BTreeHandle *tree, int *result) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	//set the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// get the first # of page 0, is # of nodes
	int *ptr = (int *) page->data;
	*result = ptr[1];

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

RC setNumEntries(BTreeHandle *tree, int value) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	//set the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// get the first # of page 0, is # of nodes
	int *ptr = (int *) page->data;
	ptr[1] = value;
	page->data = (char *) ptr;

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

/**************************************************************************************
 * Function Name: getKeyType
 *
 * Description:
 *		Get the data type of keys in given B+ Tree
 *
 * Parameters:
 * 		BTreeHandle *tree: Entry to the B+ Tree
 * 		DataType *result: Pointer to the DataType of the keys
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC getKeyType(BTreeHandle *tree, DataType *result) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	//set the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// get the first # of page 0, is # of nodes
	int *ptr = (int *) page->data;
	*result = ptr[2];

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

RC setKeyType(BTreeHandle *tree, DataType value) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	//set the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// get the first # of page 0, is # of nodes
	int *ptr = (int *) page->data;
	ptr[2] = value;
	page->data = (char *) ptr;

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

int getTypeLength(BTreeHandle *tree) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	//set the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// get the first # of page 0, is # of nodes
	int *ptr = (int *) page->data;

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return ptr[3];
}

RC setTypeLength(BTreeHandle *tree, int value) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	//set the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// get the first # of page 0, is # of nodes
	int *ptr = (int *) page->data;
	ptr[3] = value;
	page->data = (char *) ptr;

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

int getN(BTreeHandle *tree) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	//set the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// get the first # of page 0, is # of nodes
	int *ptr = (int *) page->data;

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return ptr[4];
}

RC setN(BTreeHandle *tree, int value) {

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	//set the return code
	RC rc;

	// pin page 0, page 0 is the information page of b+ tree
	rc = -99;
	rc = pinPage(BUFFER_POOL, page, 0);

	if (rc != RC_OK) {
		return rc;
	}

	// get the first # of page 0, is # of nodes
	int *ptr = (int *) page->data;
	ptr[4] = value;
	page->data = (char *) ptr;

	//unpin page 0
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

RC initNodePage(BTreeHandle *tree, PageNumber pageNum, int initValue,
		int numKeys, PageNumber pageNumofParent,
		PageNumber pageNumofLeftSibling, int n,) {

	RC rc;

	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	// pin the page
	rc = pinPage(BUFFER_POOL, page, pageNum);

	if (rc != RC_OK) {
		return rc;
	}

	// init the content of page,
	// first int: is init? 0 is not init, 1 is non-leaf, 2 is leaf
	//

	int *ptr = (int *) page;
	ptr[0] = initValue;
	ptr[1] = numKeys;
	ptr[2] = pageNumofParent;

	if (initValue == 2)
		ptr[3] = pageNumofLeftSibling;

	int *content = (int *) mallocmalloc(sizeof(int) * (n + 2 * n + 2));
	memset(content, -1, n + 2 * n + 2);
	memcpr(ptr + 4, content, n + 2 * n + 2);

	//unpin the page
	rc = -99;
	rc = unpinPage(BUFFER_POOL, page);

	if (rc != RC_OK) {
		return rc;
	}

	return RC_OK;
}

//////////////////////////////////// index access ////////////////////////////////////////
/**************************************************************************************
 * Function Name: findKey
 *
 * Description:
 *		Get the RID the search key in given B+ Tree
 *		If not exist, return RC_IM_KEY_NOT_FOUND
 *
 * Parameters:
 * 		BTreeHandle *tree: Entry to the B+ Tree
 * 		Value *key: search key
 * 		RID *result: the RID for the key we found
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC findKey(BTreeHandle *tree, Value *key, RID *result) {

	RC rc;
	// set a page pointer, pointing to pinned page
	BM_PageHandle *page = (BM_PageHandle *) malloc(sizeof(BM_PageHandle));

	// assume mgmtData save the root page number.
	PageNumber pageNum = tree->mgmtData;

	//pin the root page
	rc = -99;
	rc = pinPage(BUFFER_POOL,page,pageNum);

	if (rc != RC_OK) {
		return rc;
	}

	//
	return RC_OK;
}
/**************************************************************************************
 * Function Name: insertKey
 *
 * Description:
 *		Insert a new key and a record pointer to given B+ Tree
 *		If already exist, return RC_IM_KEY_ALREADY_EXISTS
 *
 * Parameters:
 * 		BTreeHandle *tree: Entry to the B+ Tree
 * 		Value *key: new key
 * 		RID rid: the RID for the key we inserted
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC insertKey(BTreeHandle *tree, Value *key, RID rid) {

	return RC_OK;
}
/**************************************************************************************
 * Function Name: deleteKey
 *
 * Description:
 *		Delete a key and its RID from given B+ Tree
 *		If not exist, return RC_IM_KEY_NOT_FOUND
 *
 * Parameters:
 * 		BTreeHandle *tree: Entry to the B+ Tree
 * 		Value *key: deleting key
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC deleteKey(BTreeHandle *tree, Value *key) {

	return RC_OK;
}

/**************************************************************************************
 * Function Name: openTreeScan
 *
 * Description:
 *		Create a new scan handler with given B+ Tree
 *
 * Parameters:
 * 		BTreeHandle *tree: Entry to the B+ Tree
 * 		BT_ScanHandle **handle: pointer to an array of scan handlers, only use the first spot
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle) {

	return RC_OK;
}
/**************************************************************************************
 * Function Name: nextEntry
 *
 * Description:
 *		Get the next entry RID after the given RID in the given B+ Tree
 *		If RID is null, start from beginning
 *
 * Parameters:
 * 		BT_ScanHandle *handle: Entry to the scan handler
 * 		RID *result: The starting RID, after running it's written with the found RID
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC nextEntry(BT_ScanHandle *handle, RID *result) {

	return RC_OK;
}

/**************************************************************************************
 * Function Name: closeTreeScan
 *
 * Description:
 *		Close given scan handler
 *
 * Parameters:
 * 		BT_ScanHandle *handle: Entry to the scan handler
 *
 * Return:
 *		RC: return code
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
RC closeTreeScan(BT_ScanHandle *handle) {

	return RC_OK;
}

////////////////////////////// debug and test functions ///////////////////////////////////
/**************************************************************************************
 * Function Name: printTree
 *
 * Description:
 *		Parse a B+ Tree into string in the following format:
 *			(0)[1,13,2,23,3]
 *			(1)[1.1,1,2.3,11,2]
 *			(2)[1.2,13,3.5,17,3]
 *			(3)[4.4,23,3.2,52]
 *
 * Parameters:
 * 		BTreeHandle *tree: Entry to the B+ Tree
 *
 * Return:
 *		char *: String version of a B+ Tree
 *
 * Author:
 *		Xiaolang Wang <xwang122@hawk.iit.edu>
 *
 * History:
 *		Date        Name                                      Content
 *		----------  ----------------------------------------  ------------------------
 *		2015-04-27  Xiaolang Wang <xwang122@hawk.iit.edu>     Initialization.
 **************************************************************************************/
char *printTree(BTreeHandle *tree) {

	return "0";
}

////////////////////////////////// more help functions //////////////////////////////////

