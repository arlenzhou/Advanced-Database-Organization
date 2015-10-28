~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Assignment-4: 

	In this assignment we are implementing a B+-tree index. 

* Created by:

	Xin Su <xsu11@hawk.iit.edu>

* Cooperated with:

	Chengnan Zhao <czhao18@hawk.iit.edu>,
	Jie Zhou <jzhou49@hawk.iit.edu>,
	Xiaolang Wang <xwang122@hawk.iit.edu>

* Included files:

	Makefile 
	buffer_mgr.c 
	buffer_mgr.h 
	buffer_mgr_stat.c 
	buffer_mgr_stat.h 
	btree_mgr.c 
	btree_mgr.h 
	dberror.c 
	dberror.h 
	dt.h 
	expr.c 
	expr.h 
	record_mgr.c 
	record_mgr.h 
	rm_serializer.c 
	storage_mgr.h 
	tables.h 
	test_assign4_1.c 
	test_expr.c 
	test_helper.h

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Assignment-4 Milestone:

	04/28/2015 - DEV Phase complete: Coding and Unit Test
	04/30/2015 - SIT Phase complete: System integration test and destructive test
	05/05/2015 - Delivery: deliver code and documentation to server and Blackboard

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Installation instruction:

	0. Log in to server and cd to the directory of the code:

$ cd /home/class/spring-15/cs525/xsu11/assignment4
$ pwd
/home/class/spring-15/cs525/xsu11/assign4
$ ls -l


	1. Run “make clean” to clean the compiled files, executable files and log files if there is any:

$ make clean

	2. Run “make” command to compile the code:

$ make
gcc -c storage_mgr.c
gcc -c dberror.c
gcc -c test_assign1_1.c
gcc -o test_assign1_1 storage_mgr.o dberror.o test_assign1_1.o
gcc -c test_assign1_2.c
gcc -o test_assign1_2 storage_mgr.o dberror.o test_assign1_2.o
gcc -c buffer_mgr_stat.c
gcc -c buffer_mgr.c
gcc -c test_assign2_1_modified.c
gcc -o test_assign2_1_modified storage_mgr.o buffer_mgr_stat.o buffer_mgr.o dberror.o test_assign2_1_modified.o -lpthread -std=gnu99 -I.
gcc -c test_assign2_2.c
gcc -o test_assign2_2 storage_mgr.o buffer_mgr_stat.o buffer_mgr.o dberror.o test_assign2_2.o -lpthread -std=gnu99 -I.
gcc -c expr.c
gcc -c record_mgr.c
gcc -c rm_serializer.c
gcc -c test_assign3_1.c
gcc -o test_assign3_1 buffer_mgr.o buffer_mgr_stat.o dberror.o expr.o record_mgr.o rm_serializer.o storage_mgr.o test_assign3_1.o -lpthread -std=gnu99 -I.
gcc -c test_assign3_2.c
gcc -o test_assign3_2 buffer_mgr.o buffer_mgr_stat.o dberror.o expr.o record_mgr.o rm_serializer.o storage_mgr.o test_assign3_2.o -lpthread -std=gnu99 -I.
gcc -c btree_mgr.c
gcc -c test_assign4_1.c
gcc -o test_assign4_1 btree_mgr.o storage_mgr.o buffer_mgr_stat.o buffer_mgr.o rm_serializer.o dberror.o expr.o record_mgr.o test_assign4_1.o -lpthread -std=gnu99 -I. -lm
gcc -c test_expr.c
gcc -o test_expr buffer_mgr.o buffer_mgr_stat.o dberror.o expr.o record_mgr.o rm_serializer.o storage_mgr.o test_expr.o -lpthread -std=gnu99 -I.
$ ls -l
total 392
-rwxrwxr-x 1 arlenzhou arlenzhou 37431 May  5 16:51 btree_mgr.c
-rw-rw-r-- 1 arlenzhou arlenzhou  1434 May  5 13:25 btree_mgr.h
-rw-rw-r-- 1 arlenzhou arlenzhou 49117 Apr 20 00:33 buffer_mgr.c
-rwxrwxr-x 1 arlenzhou arlenzhou  3397 Apr 10 11:25 buffer_mgr.h
-rwxrwxr-x 1 arlenzhou arlenzhou  2181 Apr 10 11:25 buffer_mgr_stat.c
-rwxrwxr-x 1 arlenzhou arlenzhou   309 Apr 10 11:25 buffer_mgr_stat.h
-rwxrwxr-x 1 arlenzhou arlenzhou   597 Apr 10 11:25 dberror.c
-rw-rw-r-- 1 arlenzhou arlenzhou  4513 May  4 14:57 dberror.h
-rwxrwxr-x 1 arlenzhou arlenzhou   195 Apr 10 11:25 dt.h
-rwxrwxr-x 1 arlenzhou arlenzhou  3851 Apr 17 15:10 expr.c
-rwxrwxr-x 1 arlenzhou arlenzhou  2895 Apr 17 15:10 expr.h
-rw-rw-r-- 1 arlenzhou arlenzhou     0 May  5 12:03 makefile~
-rw-rw-r-- 1 arlenzhou arlenzhou  2545 May  5 15:09 Makefile
-rw-rw-r-- 1 arlenzhou arlenzhou  2500 May  5 15:06 Makefile~
-rwxrwxr-x 1 arlenzhou arlenzhou 12921 May  5 17:57 README.txt
-rw-rw-r-- 1 arlenzhou arlenzhou 92645 Apr 22 17:04 record_mgr.c
-rwxrwxr-x 1 arlenzhou arlenzhou  1532 Apr 17 15:10 record_mgr.h
-rw-rw-r-- 1 arlenzhou arlenzhou  7223 Apr 20 00:33 rm_serializer.c
-rw-rw-r-- 1 arlenzhou arlenzhou 22592 Apr 17 15:10 storage_mgr.c
-rwxrwxr-x 1 arlenzhou arlenzhou  1960 Apr 10 11:25 storage_mgr.h
-rw-rw-r-- 1 arlenzhou arlenzhou  3561 May  1 16:25 tables.h
-rwxrwxr-x 1 arlenzhou arlenzhou  2585 Apr 10 11:25 test_assign1_1.c
-rwxrwxr-x 1 arlenzhou arlenzhou 10106 Apr 10 11:25 test_assign1_2.c
-rwxrwxr-x 1 arlenzhou arlenzhou  7625 Apr 10 11:25 test_assign2_1.c
-rwxrwxr-x 1 arlenzhou arlenzhou  9864 Apr 10 11:25 test_assign2_1_modified.c
-rwxrwxr-x 1 arlenzhou arlenzhou  4273 Apr 17 15:10 test_assign2_2.c
-rw-rw-r-- 1 arlenzhou arlenzhou 17095 Apr 20 00:33 test_assign3_1.c
-rw-rw-r-- 1 arlenzhou arlenzhou 17547 Apr 22 18:20 test_assign3_2.c
-rw-rw-r-- 1 arlenzhou arlenzhou  6673 Sep 16  2014 test_assign4_1.c
-rwxrwxr-x 1 arlenzhou arlenzhou  3835 Apr 10 11:25 test_expr.c
-rwxrwxr-x 1 arlenzhou arlenzhou  2467 Apr 10 11:25 test_helper.h
-rw-rw-r-- 1 arlenzhou arlenzhou  4096 May  5 15:10 testidx

	3. Run test_assign4_1 and redirect the output to file t1.log:

$ ./test_assign4_1 > t1.log


	4. If you want to re-run the executable files, start from step 3. If you want to re-compile the files, start from step 1.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Function Description:

	function name		description
	——————————————————————  ————————————————————————————————————————————————————————————————
	createBtree()     Create a B+ Tree with given name, DataType and number of 				elements in a node

	openBtree()	      Open a B+ Tree with given name

	closeBtree()	Close given B+ Tree

	deleted()		Delete B+ Tree with given name
	
	getPageInfo()	get the page information
	
	setPageInfo()	set the page information	
	
	getNumNodes()	get the total number of nodes
	
	setNumNodes()     set the total number of nodes	
	
	getNumEntries()	get the total number of entries
	
	setNumEntries()	set the total number of entries
	
	getKeyType()	get the key type
	
	setKeyType()      set the key type
	
	getTypeLength()	get the length of type
	
	setTypeLength()	set the length of type
	
	getN()		get the numbers of N
	
	setN()		set the numbers of N
	
	getRootPage()	get the current root page
	
	setRootPage()	set the root page
	
	createNodePage()  create node page
	
	getNodeState()    return the node state, leaf or non-leaf

	setNodeState	set the node state, leaf or non-leaf

	getCurrentKeys	get current keys

	setCurrentKeys    set current keys

	getParent		get parent node

	setParent		set parent node

	getLeftSibling    get left sibling

	setLeftSibling    set left sibling

	getRightSibling   get right sibling

	setRightSibling   set right sibling

	getKey 		get the key

	setKey		set the key

	getKeyPointer     get the key pointer

	setKeyPointer	set the key pointer

	findKey		find the key

	insertKey    	insert the key

	deleteKey		delete the key

	nextEntry		find the next entry

	combineNode		combine the nodes

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

