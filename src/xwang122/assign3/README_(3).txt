~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Assignment-3: 

	The assignment is to accomplish a record manager of a database. All rights reserved.

* Created by:

	Xin Su <xsu11@hawk.iit.edu>

* Cooperated with:

	Chengnan Zhao <czhao18@hawk.iit.edu>,
	Jie Zhou <jzhou49@hawk.iit.edu>,
	Xiaolang Wang <xwang122@hawk.iit.edu>

* Included files:

	// to be filled	

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Assignment-3 Milestone:

	04/08/2015 - DEV Phase complete: Code and Unit Test
	04/18/2015 - SIT Phase complete: System Integration Test
	04/20/2015 - Delivery: Code and Documentation

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Installation instruction:

	// to be filled
	
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Function Description:

	function name				description
	—————————————————————— 		———————————————————————————————————————————————————————————————————————————————————
	initRecordManager()			Initialize the Record Manager with a given entry to the buffer manager
	
	shutdownRecordManager()		Shut down the Record Manager
	
	initTableInfoPage()			Initialize the table info page

	createTable()				Create table file with given name and schema if it doesn't exist
								Then initialize the table info page and create the first reserved page

	openTable()					Open the table with the given name

	closeTable()				Close the passed in table

	deleteTable()				Delete the table with given name

	getRecordLength()			Get the length of a single record

	getTotalRecords()			Get the total records number stored in the table

	setTotalRecords()			Update the total records number to a user entered value

	getTotalPages()				Get the total number of pages stored in the table

	setTotalPages()				Update the total number of pages to a user entered value

	getPageMaxRecords()			Get the max number of records that can be stored in one page

	getNumTuples()				Get the total number of records stored in the table

	checkRID()					Check if the passed in RID is valid

	getReservedPageNum()		Get the reserved page number that records the passed in page

	isPageFree()				Check if the passed in page has empty slots
	
	getBit()					Get the value stored at a certain bit (TRUE/FALSE)

	setBit()					Set the value at a certain bit (TRUE/FALSE)

	createReservedPage()		Pin the reserved page and initialize it
	
	updateReservedPage()		Update the passed in reserved page

	getFirstFreePage()			Get the page number of the first page with free slots

	setFirstFreePage()			Update the page number of the first page on a reserved page

	getNextReservedPage()		Get the page number of next reserved page on the current reserved page

	setNextReservedPage()		Update the page number of next reserved page on the current reserved page
	
	getPrevReservedPage()		Get the page number of previous reserved page on the current reserved page

	setPrevReservedPage()		Update the page number of previous reserved page on the current reserved page

	searchFirstFreePage()		Search for the page number of the first page with free slots

	isPageInitialized()			Check if a certain page is initialized
	
	setPageInitialized()		Update a certain page's initialization status

	getCurrentRecords()			Get the number of records stored in a certain page

	setCurrentRecords()			Update the number of records stored in a certain page

	isSlotOccupied()			Check if a certain slot is occupied

	setSlotOccupied()			Update a certain slot's occupancy status

	checkRecordLength()			Check if the passed in record has a valid length

	initRecordPage()			Initialize a new record page

	getFreeSlotId()				Get the slot number of the first free slot on a certain page

	insertRecord()				Insert a record to the table
	
	deleteRecord()				Delete a record with given RID

	updateRecord()				Update a table slot with the passed in record

	getRecord()					Get a record from a certain table and certain RID
								If not found, return error code

	startScan()					Initialize a new scan manager with given table manager and given expression

	next()						Get the next eligible record in the table

	closeScan()					Unset the scan manager

	getRecordSize()				Get the size of the each record of given schema

	schemaToString()			Parse a schema into string

	createSchema()				Create a new schema with given values

	freeSchema()				Unset a schema
	
	createRecord()				Create an empty record of given schema

	freeRecord()				Unset a record

	getAttr()					Get the value of the attrNumth attribute for given record of given schema and 
								Store it at the first spot in an array of Values

	setAttr()					Update the value of the attrNumth attribute for given record to given value

	condUpdateRecord()			Find all eligible records from the scan manager, then update them with new expression
								We only support for an update with expression "attribute = constant" by now
	
	checkEqualsExpr()			Check if an expression is "attribute = constant"


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Structure Description:

	1. Page 0 stores all the table infomation, including 
										a) record length(int, 4 bytes)
										b) number of total records(int, 4 bytes)
										c) number of total pages(int, 4 bytes)
										d) number of max records per page(int,4 bytes)
										e) table schema(string)
	2. Page 1 is a reserved page, it's used to manage free page information. The first 96 bytes are for page header, and the other 
		4000 bytes are used for free page information. We use only 1 bit for each pages' occupancy status: "1" indicates there EXIST 
		free slots on that page, "0" indicates there EXIST NO free slots on that page. So each reserve page can store 32000 pages' 
		occupancy status.
	3. After every 32000 record pages, we create a new reserved page. So the page numbers for reserved pages are 1, 32001, 64001...
		Each reserved page is linked with the previous and the next reserved page by information in the header. There is following 
		information in a reserved page header: 
										a) page number of the first page with free slots(int, 4 bytes)
										b) page number of the previous reserved page
										c) page number of the next reserved page
	4. Other pages are record pages. Information stored in a record page header includes: 
										a) page initialization status (bool, 1 byte)
										b) number of records stored on this page (int, 4 bytes)
										c) occupancy status for each slot (bool for each slot, 1 * n bytes)
		Records are stored from bottom on each page.

		
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Test Cases:

	// to be filled
	
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Problems to be solved： 

	// to be filled
	