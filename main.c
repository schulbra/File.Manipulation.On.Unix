/*------------------------------------------------------------------------------------------------|
--      Name:           Brandon Schultz
---     Date:           1 - 23 - 22
------  Description:    This program:
------      Reads directory entries
------      Finds  a file in the current directory based on user specified criteria
------      Reads and processes the data in the chosen file
------      Creates a directory + files and writes processed data to these files
------
-----   - References: https://www.tutorialspoint.com/cprogramming/c_structures.htm
------              https://www.geeksforgeeks.org/comparator-function-of-qsort-in-c/
------              https://en.wikipedia.org/wiki/C_standard_library
------              https://www.tutorialspoint.com/cprogramming/c_type_casting.htm
------              https://stackoverflow.com/questions/5057987/whats-the-point-of-const-void
----                https://stackoverflow.com/questions/34842224/when-to-use-const-void
---                 http://www.cplusplus.com/reference/cstdlib/qsort/
--                  https://www.javatpoint.com/tokens-in-c
--                  https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r
--                  https://stackoverflow.com/questions/266357/tokenizing-strings-in-c
-                   https://www.geeksforgeeks.org/size_t-data-type-c-language/
--                  https://stackoverflow.com/questions/4770985/how-to-check-if-a-string-starts-with-another-string-in-c
--                  https://stackoverflow.com/questions/28533553/what-is-the-default-mode-for-open-calls-with-o-creat-and-how-to-properly-set-i
--                  Students.c and various other sample code via Canvas
|------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>       // - header defines variable types functions for manipulating date and time
#include <unistd.h>     // - defines miscellaneous symbolic constants and types, and declares functions
#include <fcntl.h>      // - file/DIR manipulations

// - Used in check that file begins with proper " movies_..." title. 
// by comparing  two strings character by character until
// NULL is encountered or chars in string are equal. context is 
// provided where this formally used later in program
#define PREFIX "movies"

/*------------------------------------------------------------------------------------------------|
- Structure used to represent various movie attributes as defined in movies_sample_1.csv :
- Additional context is provided below :
|------------------------------------------------------------------------------------------------*/
struct movie
{
    char* title;           //  Movie's title.
    int year;              //  Movie's year of release between 1900-2021.
    char languages[5][21]; //  Language(s) movie is in.
    double rating;         //  Movies designated Rating value between 1-10
    struct movie* next;
}; //Movie;

/*------------------------------------------------------------------------------------------------|
- Structure used to parse current line of movie data and create data structure representing movie
  item containg attributes listed above for "struct Movie"
- Additional context is provided below :
- Note, the token setup is heavily inspired by student.c from Canvas
|------------------------------------------------------------------------------------------------*/
struct movie* createMovie(char* currLine)
{
    // -Create movie object on curr line:
    // -structs cannot have functions for themselves so ned to use function pointers.
    //   - struct Movie                     =   pointers base type
    //   -  *currMovie                      =   name of pointer variable =
    //   -  malloc(sizeof(struct Movie))    =   bytes allocated in mem for Movie
    struct movie* currMovie = malloc(sizeof(struct movie));
    char* saveptr;
    // Token 1 = Movie Title |________________________________
    char* token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);
    /// Token 2 = Movie Year  |____________
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);
    //// Token 3 = Movie Lang ||____________________

    char* langPtr;
    char* langToken = strtok_r(NULL, "[]", &saveptr);
    char* langLine = langToken;
    langToken = strtok_r(langLine, ";", &langPtr);
    int index = 0;
    //--------------------------------------
    //Tokenize LANG STRING
    while (langToken != NULL)
    {
      strcpy(currMovie->languages[index], langToken);
      langToken = strtok_r(NULL, ";", &langPtr);
      index++;
    };
    //--------------------------------------
    //_______________________||||
    ///// Token 4 = Movie Rating |||___________     
    token = strtok_r(NULL, ",\n", &saveptr);
    currMovie->rating = strtod(token, NULL);
    currMovie->next = NULL; // Next LL node = NULL in new Movie entry
   return currMovie;
}

/*----------------------------------------------------------------------------------------------|
// - Returns LL of movies by parsing data from each line of file.
-----------------------------------------------------------------------------------------------*/
struct movie* processMoviesFileNameExtension(char* moviesFileNameExtension)
{
     // Opens and reads from file designated in terminal
    FILE* movieFile = fopen(moviesFileNameExtension, "r");
    char* currLine = NULL;
    char* stringBuffer = NULL;
    size_t len = 0;
    ssize_t nread;
    char* token;
    struct movie* Head = NULL;      // Head of LL
    struct movie* Tail = NULL;      // Tail of LL
    //  Skip First line of CSV, then read line by line: 
    // skipLine = getline(&currLine, &length, movieFile);
    getline(&stringBuffer, &len, movieFile);
    free(stringBuffer);
    int moviesCount = 0;
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        //  newMovieNode = curr line of movie in dataset from csv/file
        struct movie* newMovieNode = createMovie(currLine);
        // -Check if node is first node in LL, if so, attach to
        // node the head and tail that mill make up start/end of LL.
        if (Head == NULL)
        {
          Head = newMovieNode;    // Node is 1st of LL, Head = currNode
          Tail = newMovieNode;    //                    Tail = currNode
        }
        else
        {
         Tail->next = newMovieNode;  // Node is not 1st of LL, add to LL
         Tail = newMovieNode;        // continue on moving tail
        }
        moviesCount++;
    }
    printf("|  Processing File... %s", moviesFileNameExtension);
    free(currLine);
    fclose(movieFile);
    return Head;
}

/*----------------------------------------------------------------------------------------------|
// - Clears mem used on processsed LL:
-----------------------------------------------------------------------------------------------*/
void clearMemory(struct movie* Head)
{
    struct movie* temp;
    while (Head != NULL)
    {
     temp = Head;
     Head = Head->next;
     free(temp->title);
     free(temp);
    }
}

/*----------------------------------------------------------------------------------------------|
// - Checks files fopr beginning with proper " movies_..." title and if file has csv extension:
-----------------------------------------------------------------------------------------------*/
int chekFileType(char* fileName)
{
    char* saveptr,* ptr,* tempStr;                                         
    int bool = 0;
    if (strncmp(PREFIX, fileName, strlen(PREFIX)) == 0)                      //
    {
        // Check that file begins with proper " movies_..." title.
        // by comparing  two strings character by character until
        // NULL is encountered or chars in string are equal:
        tempStr = calloc(strlen(fileName) + 1, sizeof(char));
        strcpy(tempStr, fileName);
        // If filename = Null, It doesnt begin with proper " movies_title..."
        ptr = strtok_r(tempStr, ".", &saveptr);
        ptr = strtok_r(NULL, ".", &saveptr);
        // If filename != Null, It does begin with proper " movies_title..."
        // and if  csv extension exists return one after clearing memory:
        /*if (ptr != NULL && strcmp(ptr, "csv") == 0) bool = 1;*/
        if (ptr != NULL && strcmp(ptr, "csv") == 0)                         
         bool = 1;
        free(tempStr);      // clear memory
    }
    return bool;            // return title decision
}

/*----------------------------------------------------------------------------------------------|
// Funciton for Returning name of file to be processed:
-----------------------------------------------------------------------------------------------*/
char* getFile(char* fileNameProcessed,char* directoryName)                         
{
    /*if (fileNameProcessed != NULL)
    free(fileNameProcessed);*/
    if (fileNameProcessed != NULL) free(fileNameProcessed);
    fileNameProcessed = calloc(strlen(directoryName) + 1, sizeof(char));
    strcpy(fileNameProcessed, directoryName);
    return fileNameProcessed;
}

/*----------------------------------------------------------------------------------------------|
// Funciton for sorting and obtaining files by size after passing title and extension checks:
-----------------------------------------------------------------------------------------------*/
char* getFileMovies_csvLgSm(char fileLgOrSmUI)
{
    // - checks current directory for largest/smallest .csv file with proper title conditions.
    // - It checks each file/enmtry in current directory for data containg individual file size
    // until all entrices have been parsed:
    DIR* currDir = opendir(".");            //open curr dir
    //http://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
     struct dirent* currDirectory;                   // curr dir struct
     struct stat dirStat;                   // used to store entry/file size of individual files
     char* fileNameProcessed = NULL;
     int currEntryFileSize = (fileLgOrSmUI == 'L') ? 0 : INT32_MAX;                  //    
    // Go through entries until directory has been completely parsed:
    while ((currDirectory = readdir(currDir)) != NULL)
    {
      // if file currently checked matches name conditions mentioned constantly get/store
      // data concering curr entrie/files size:
      if (chekFileType(currDirectory->d_name) == 0)
            continue;
        stat(currDirectory->d_name, &dirStat);
        if (fileLgOrSmUI == 'L' && dirStat.st_size > currEntryFileSize)
        {
          fileNameProcessed = getFile(fileNameProcessed,currDirectory->d_name);
          currEntryFileSize = dirStat.st_size;
        }
        else if (fileLgOrSmUI == 'S' && dirStat.st_size < currEntryFileSize)
        {
         fileNameProcessed = getFile(fileNameProcessed,currDirectory->d_name);
         currEntryFileSize = dirStat.st_size;
        }
    }
    closedir(currDir);          // close parsed directory and ret file name:
    return fileNameProcessed;
}

/*----------------------------------------------------------------------------------------------|
// Funciton for creating a new directory and printing a message with the name of the directory 
// that has been created
-----------------------------------------------------------------------------------------------*/
char* makeDirectory()\
{
    char* madeDirName;                                        //  dir name
    int randomNumber = random() % 100000;                     //  random number +
    int len = strlen("schulbra.movies.") + 5;                 //  your_unid.movies
    madeDirName = calloc(len + 1, sizeof(char));              //  =
    sprintf(madeDirName, "schulbra.movies.%i", randomNumber); // schulbra.movies_num.between.0-99999
    int checkPerm = mkdir(madeDirName, 0750);                     // rwxr-x---        =       0750
                                       //owner: read, write execute permissions, group: read/execute
    if (checkPerm == -1)
        printf("Directory Can't Be Created...\n");
        printf("\n|Created Directory %s \n", madeDirName);
        printf("\n");
    return madeDirName;
}

/*----------------------------------------------------------------------------------------------|
// Funciton for creating new files:
-----------------------------------------------------------------------------------------------*/
void makeDirectoryFiles(char* madeFile, char* madeFileTitle)  
{
 int fd;
 //rw-r-----        =       0640        Owner: r/w              Group: r
 fd = open(madeFile, O_RDWR | O_CREAT | O_APPEND, 0640);
  if (fd == -1)
      //printf("open() failed on \"%s\"\n", madeFile);               
      printf("Failed to Open \"%s\"\n", madeFile);
  write(fd, madeFileTitle, strlen(madeFileTitle));
  write(fd, "\r\n", 2);
  close(fd);
}

/*----------------------------------------------------------------------------------------------|
// - Funciton for generation of files in made directory:
-----------------------------------------------------------------------------------------------*/
//  add comment tomorrow**************************************************************
//https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm

void buildNewFilesInNewDir(char* currDirectory,struct movie* List)
{
 char* newFilePath;
  while (List != NULL)
  {
   int len = strlen(currDirectory) + 9;
    newFilePath = calloc(len + 1, sizeof(char));
        sprintf(newFilePath, "%s/%d.txt", currDirectory, List->year);
        makeDirectoryFiles(newFilePath, List->title);
        free(newFilePath);
        List = List->next;
  }
}

/*----------------------------------------------------------------------------------------------|
// - Funciton confirming that UI file name input exists in directory:
-----------------------------------------------------------------------------------------------*/
int UI_fileNameChk(char* UIFileName)
{
 DIR* currDir = opendir(".");
 struct dirent* currDirectory;
 while ((currDirectory = readdir(currDir)) != NULL)
 {
   if (strcmp(UIFileName, currDirectory->d_name) == 0)
   {
    closedir(currDir);
    return 1;
   }
  }
  closedir(currDir);
  return 0;
}

/*----------------------------------------------------------------------------------------------|
// - Funciton displaying prompt for UI:
-----------------------------------------------------------------------------------------------*/
char* promptUserFile()
{
    char* UIFileName = calloc(256, sizeof(char));
    printf("\nEnter A File Name: ");
    scanf("%s", UIFileName);
    return UIFileName;
}


/*------------------------------------------------------------------------------------------
 - The below is a UI that was intended to be included with the final commmit, however, due 
 to time constraints and compiling issues after inclusion of API for operations on
 files/directories I opted for a less magnificent version. One day I will fix
 this.
----------------------------------------------------------------------------------------*/

/*
int main(int argc, char* argv[])
{
    struct Movie* list = processMoviesFileNameExtension(argv[1]);
    printf("\n");
    int numberOfMovies = totalMovieList(list);
    printf(" _________________________________________________________________________________  \n");
    printf("| -      -  Name: Brandon Schultz                                            ---- | \n");
    printf("| --     -  Date: 1-12-22                                                     --- | \n");
    printf("| ---    -  Assignment 1: Movies                                               -- | \n");
    printf("| ----   -  Description: What am I?  A Machine Clearly.                         - | \n");
    printf("| ---       You can call me AMC and I LOVE MOVIES...                            - | \n");
    printf("| --        listed in CSV files. You provide me the goods                      -- | \n");
    printf("| -         and I'll show you:                                                --- | \n");
    printf("| --         1. All Movies Released In Defined Year                          ---- | \n");
    printf("| ---        2. Highest Rated Movie In Defined Year                           --- | \n");
    printf("| ----       3. Title+Release Date of All Movies In Defined Year               -- | \n");
    printf("| ---        4. Quit Program                                                    - | \n");
    printf("| --         and if you dont like that well                                    -- | \n");
    printf("| -          THATS SHOW BUSINESS BABYYYYYYY                                   --- | \n");
    printf("|_________________________________________________________________________________| \n\n");
    printf(" |       - Processed (the goods) and...\n");
    printf(" ||      - Processed the file %s and parsed data for %i films.\n", argv[1], numberOfMovies);
    printf(" |||                \n");
    printf(" ||||    - Now what?\n");
    // printf("|________________________________________________________|\n");
    int inputOptionSelection;
    int inputOption;
    char* language;
    while (1 == 1)
    { //printf("  - Processed (the goods)
        printf("         _______________________________________________________________\n");
        printf("        |   - Valid Input = 1-4 ONLY    |    - Selection Description:   |\n");
        printf("        |_______________________________________________________________|\n\n");
        printf("     1. Show movies released in the specified year                               ||||\n");
        printf("     2. Show highest rated movie for each year                                    |||\n");
        printf("     3. Show the title and year of realease of all movies in a specific language   ||\n");
        printf("     4. Exit from the program                                                       |\n");
        printf("   \n        - Enter a choice from 1 to 4: ");
        scanf("%i", &inputOptionSelection);
        // printf("   - Choose using your hands \n");

        if (inputOptionSelection == 1)
        {
            printf("Enter the year for which you want to see movies: ");
            scanf("%i", &inputOption);
            printMovieYear(list, inputOption);
        }

        else if (inputOptionSelection == 2)
        {
            topMovieRating(list);
        }

        else if (inputOptionSelection == 3)
        {
            printf("      ---  Enter the language for which you want to see movies: ");
            scanf("%s", language);
            printMoviesWithLanguage(list, language);
        }

        else if (inputOptionSelection == 4)
        {
            return EXIT_SUCCESS;
        }

        else if (inputOptionSelection >= 5)
        {
            printf(" ---- Nothing was found...\n");
            printf(" ---- Try another input value: ");
            printf(" \n");

        }
    }
}*/




/*----------------------------------------------------------------------------------------------|
// - Funciton(s) displaying various prompts for running of program:
//      -   1. Select file to process
            2. Exit the program
            Enter a choice 1 or 2: 1
            Which file you want to process?
            Enter 1 to pick the largest file
            Enter 2 to pick the smallest file
        Enter 3 to specify the name of a file
-----------------------------------------------------------------------------------------------*/
void UI_InitPrompts()
{
    char* fileProcLgSmCus;            //largest, smallest or custom (ui defined) 
    char fileProcSelction;            //L =1, S = 2,    3 = UI defined
    int fileProcOption;
    do {
        printf("____________________________________________\n| Which file do you want to process?         |\n|  -   Enter 1 to pick the largest file      |\n|  --  Enter 2 to pick the smallest file     |\n|  --- Enter 3 to specfiy the name of a file |");
        printf("\n| Select from 1 to 3 ONLY:             ");
      //  scanf("%i", &fileProcOption);
      scanf("%*[^0-9]%d", &fileProcOption);
        switch (fileProcOption) {
        case 1:                         //Process Largest
         fileProcSelction = 'L';
         fileProcLgSmCus = getFileMovies_csvLgSm(fileProcSelction);
        break;
        
        case 2:                         //Process Largest
         fileProcSelction = 'S';
         fileProcLgSmCus = getFileMovies_csvLgSm(fileProcSelction);
        break;
        
        case 3:                         //Process UI if valid, if not prompt user for better input
         fileProcLgSmCus = promptUserFile();
          if (UI_fileNameChk(fileProcLgSmCus) == 0)
            {
            	printf(" ____________________________________________________\n");
                printf("|\n %s Directory Doesnt Exist. MAKE BETTER CHOICES:  |\n", fileProcLgSmCus);
                free(fileProcLgSmCus);
                fileProcOption = 0;
            }
        break;
        
	default:
	printf("\n\n _______________________________________________________________\n |                                                               |\n |                           PLAY AGAIN?:                        |\n |_______________________________________________________________|\n\n");
	
	  /*  printf(" _______________________________________________________________\n");
            printf(" _______________________________________________________________\n");
            printf("|                                                               |\n");
            printf("|                           PLAY AGAIN?:                        |\n");
            printf("|_______________________________________________________________|\n\n");*/
        }

    } 
    while (fileProcOption < 1 || fileProcOption > 3);
        struct movie* List = processMoviesFileNameExtension(fileProcLgSmCus);
        char* newDir = makeDirectory();
        buildNewFilesInNewDir(newDir, List);
    // whipe memory
    clearMemory(List);
    free(fileProcLgSmCus);
    free(newDir);
}

//** ADD COMMENTS
int homePrompts()
{

    int homePrompt_UI;
  printf("| -----                                                                      -----| \n");
  printf("| 1. Select A File To process:                                                ----| ");
  printf("\n| 2. Exit from the program:                                                    ---| ");
  printf("\n  Enter a choice 1 or 2:    ");
    scanf("%i", &homePrompt_UI);
    return homePrompt_UI;
}


//
int main()
{
    printf(" _________________________________________________________________________________  \n");
    printf("| -      -  Name: Brandon Schultz                                            ---- | \n");
    printf("| --     -  Date: 1-12-22                                                     --- | \n");
    printf("| ---    -  Assignment 2  -  Files and Directories                             -- | \n");
    printf("| ----       This program can read directory entries,                           - | \n");
    printf("| ----       Find a file in current directory based on UI,                      - | \n");
    printf("| ----       Read + Process data in file specified by User                      - | \n");
    printf("| ----       Create a directory filled with files containing processed data     - | \n");
    printf("| ----       written to them                                                    - | \n");
    printf("|_________________________________________________________________________________| \n");
    srandom(time(NULL));
    int UI_SelectionHome;
    do {
     UI_SelectionHome = homePrompts();
     if (UI_SelectionHome == 1)
      UI_InitPrompts();
     else if (UI_SelectionHome == 2)
      printf("| ------                                                  I LOVE YOU!     ------|  \n");
      //  printf("| -----                                                                      -----| \n");
        else
            printf("|-----ERROR: Valid Input =1 or 2 ONLY |\n\n");
            
         // printf("|ERROR:|________________________________________________________ \n");
 	//  printf("|_______________________________________________________________| \n");
  	//  printf("|   - PLay Again?            |    - **Numeric Input Only        | \n");
   	//  printf("|____________________________|    - **Unless you choose 3| \n");
   	//  printf("|____________________________|__________________________________| \n");
   	 // printf("|---------------------------------------------------------------| \n");
   	  //printf("|                                                               |  \n");
  	  printf(" _________________________________________________________________________________  \n");
   	  printf("|                                - Play Again?                                    | \n");
   	  printf("|_________________________________________________________________________________| \n");
     
     
           // printf("_________________________________________________________________\n");
            //printf("|                                                               |\n");
            //printf("|                           PLAY AGAIN?:                        |\n");
          //  printf("|_______________________________________________________________|\n\n");

    } while (UI_SelectionHome != 2);
    return EXIT_SUCCESS;
}
