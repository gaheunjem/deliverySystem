#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *contents : package context (message string)
*/
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password




// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {
	
	deliverySystem[x][y].building=0;//initiate buliding num
	deliverySystem[x][y].room=0;//initiate room num
	deliverySystem[x][y].cnt=0;//check the storage is empty now
	strcpy(deliverySystem[x][y].passwd,"aaaa");//initiate passwd
	strcpy(deliverySystem[x][y].context,"bbbb");//initiate context
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	
	int matchpwd;
	int matchmasterpwd;
	char s1[PASSWD_LEN+1];
	printf(" - input password for (%d,%d) storage : ",x,y);
	scanf("%s",&s1);
	
	matchpwd=strcmp(s1, deliverySystem[x][y].passwd);// input passwrd is matching? maching(0) no(1)
	matchmasterpwd=strcmp(s1, masterPassword);// input passwrd is matching with masterpwd? maching(0) no(1)
	
	if(matchpwd==0||matchmasterpwd==0)//input passwrd match correct pwd or is masterpwd
	{
		return 0;
	}
	else if(matchpwd==1&&matchmasterpwd==1)//input passwrd no match correct pwd and is not masterpwd
	{
		return -1;
	}
}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	
	int i,j;
	FILE *fp;
	
	fp=fopen("storage.txt","w");
	
	fprintf(fp,"%d %d\n",systemSize[0],systemSize[1]);//backup row/column on filepath
	fprintf(fp,"%s\n",masterPassword);//backup masterpasswrd on filepath

	//backup number of building, room and passwrd and context on filepath
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			if(deliverySystem[i][j].cnt==1)
			{
				fprintf(fp,"%d %d %d %d %s %s\n",i,j,deliverySystem[i][j].building,deliverySystem[i][j].room,
				deliverySystem[i][j].passwd,deliverySystem[i][j].context);
			}
		}
	}
	
	//if backup wansn't working
	if(fp==NULL)// fp has no useful value
	{
		return -1;
	}
	
	fclose(fp);
	
	return 0;
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	
	
	int i,j;
	int x,y;
	int nBuilding, nRoom;
	char pwd[PASSWD_LEN+1];
	char str[100];
	
	FILE *fp;
	fp=fopen(filepath,"r");
	
	//read to row & column
	fscanf(fp,"%d %d",&systemSize[0],&systemSize[1]);
	
	//create delivery system deliverySystem[][]
	deliverySystem = (storage_t**)malloc(systemSize[0] * sizeof(storage_t*));//row
	
	for (i=0;i<systemSize[0];i++)
	{
		deliverySystem[i] = (storage_t*)malloc(systemSize[1] * sizeof(storage_t));//column
	}
	
	//initiate each storage cnt
	for (i=0;i<systemSize[0];i++)
		for(j=0;j<systemSize[1];j++)
			{
				deliverySystem[i][j].cnt=0;
				
			}
	//create system for context
	for(i=0;i<systemSize[0];i++)
		for(j=0;j<systemSize[1];j++)
		{
			deliverySystem[i][j].context=(char*)malloc(10*sizeof(char));
		}
	
		
	
		
	//read to master password
	fscanf(fp,"%s",masterPassword);
	
	//read number of building, room and passwd, context
	while(!feof(fp))//if didn't reach end of file
	{
		fscanf(fp,"%d %d %d %3d %s %s",&x,&y,&nBuilding,&nRoom,pwd,str);
		
		deliverySystem[x][y].building = nBuilding;
		deliverySystem[x][y].room = nRoom;
		strcpy(deliverySystem[x][y].passwd,pwd);
		deliverySystem[x][y].cnt=1;//check the storage is not empty now.
		strcpy(deliverySystem[x][y].context,str);
		
		storedCnt++;//whole number storage that have something ++
		
	}
	

	storedCnt=storedCnt-1;
	
	
	fclose(fp);
	
	return 0;
	
	if(fp==NULL)
		return -1;
	
}

//free the memory of the deliverySystem
void str_freeSystem(void) {
	
 	int i,j;
 	
 	//free deliverySystem malloc
 	for(i=0;i<systemSize[0];i++)
 	{
 		free(deliverySystem[i]);	
	}
	
	free(deliverySystem);
	
	//free context malloc 
	for(i=0;i<systemSize[0];i++)
		for(j=0;j<systemSize[1];j++)
		{
			free(deliverySystem[i][j].context);
		}
	
	
}

//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	
	
	
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {
	
	// input a package to the right place
	if(deliverySystem[x][y].cnt==0)
	{
		deliverySystem[x][y].building=nBuilding; //allocate buliding num
		deliverySystem[x][y].room=nRoom; //allocate room num
		strcpy(deliverySystem[x][y].context,msg); //allocate context
		strcpy(deliverySystem[x][y].passwd,passwd); //allocate passwd
		
		deliverySystem[x][y].cnt=1;//check the storage is not empty now.
		storedCnt++;
		return 0;
	}
	else //failed to put
	{
		return -1;
	}
	
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
	int matching;
	
	matching=inputPasswd(x,y); // 0 or !0
	
	if(matching==0) //password is matching
	{
		printf(" -----------> extracting the storage (%d,%d)...",x,y);
		storedCnt--;//whole number of cells occupied - number of extracted store
		printStorageInside(x,y);
		initStorage(x,y);
		
		return 0;
	}
	else if(matching!=0)//password is not matching
	{
		printf(" -----------> password is wrong!!\n");
		return -1;
	}
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	
	int i,j;
	int cnt; // number of packages that the storage system has
	int NumB, NumR;//Number of someone's Buliding and Room
	
	for(i=0;i<systemSize[0];i++)
		for(j=0;j<systemSize[1];j++)
		{
			NumB=deliverySystem[i][j].building;
			NumR=deliverySystem[i][j].room;
			
			//if Number of buliding and room is matching and storage is full
			if(nBuilding==NumB && nRoom==NumR && deliverySystem[i][j].cnt==1) 
			{
				printf(" -----------> Found a package in (%d,%d)\n",i,j);
				cnt++;
			}
		}
	
	return cnt;
}

