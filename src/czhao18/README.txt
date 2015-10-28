Test Cases:

	no	case content																	expectation				result		
	！！  ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！  ！！！！！！！！！！！！！！！！！！  ！！！！！！！！！！！！！！！！！！
	1	all functions in the given test case											Return OK			Return OK
	2	createTable(): when SCHEMA == NULL return error									Return error		Return error
	3	createTable(): when length of SCHEMA_STR is equal to 0 return error				Return error		Return error
	4	setTotalRecords(): input value < 0, return error								Return error		Return error
	5	setTotalPages(): input value < 0, return error									Return error		Return error
	6	checkRID(): input RID's page value < 0, return RC_RID_OUT_OF_BOUND				Return error		Return error
	7	checkRID(): if RID's page number is reserve page, return error					Return error		Return error
	8	setBit(): if input value is other than 0 or 1, return error						Return error		Return error
	9	setFirstFreePage: page number value is < 0, return error						Return error		Return error
	10	isPageFree(): page number didn't match reserve page number, return error		Return error		Return error
	11	setCurrentRecords(): input value is < 0, return error							Return error		Return error
	12	isSlotOccupied(): the page number is reserve page, return error					Return error		Return error
	13	setSlotOccupied(): the page number is not valid, return error					Return error		Return error
	14	setSlotOccupied(): the slot number is not valid, return error					Return error		Return error
	15	checkRecordLength(): input length and the getLength method is not the same		Return error		Return error
	16	getFreeSlotId(): input page number has no empty slot, return error				Return error		Return error
	17	insertRecord(): no empty space, need to create page first, return error			Return error		Return error
	18	deleteRecord(): input RID is not valid, return error							Return error		Return error
	19	updateRecord(): input Record's RID is not valid, return error					Return error		Return error
	20	getRecord(): input RID is not valid, return error								Return error		Return error
	21	next(): RID is NULL, return RC_NO_RID											Return error		Return error
	22	next(): not more tuples to read, return RC_RM_NO_MORE_TUPLES					Return error		Return error
	23	createSchema(): using wrong inputs, return error								Return error		Return error
	24	setAttr(): value's data type didn't match schema's, return error				Return error		Return error
	25	createRecord(): input schema is not valid, return error							Return error		Return error
	26  createReservedPage(): wrong reservedPageNum, return error						Return error		Return error
	27  updateReservedPage(): wrong reservedPageNum, return error						Return error		Return error
	28  updateReservedPage(): wrong PageNum, return error								Return error		Return error
	29  setFirstFreePage(): wrong reservedPageNum, return error							Return error		Return error
    30  getNextReservedPage(): wrong reservedPageNum, return error						Return error        Return error
	31  setNextReservedPage(): wrong reservedPageNum, return error						Return error		Return error
	32  getPrevReservedPage(): wrong reservedPageNum, return error						Return error		Return error
