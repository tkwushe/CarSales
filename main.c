// needed to allow Visual Studio to work with scanf()
#define _CRT_SECURE_NO_WARNINGS

// required to enable use of scanf() and printf()
#include <stdio.h>
// required to runn system("cls") in VSstudioCode
#include <stdlib.h>
// required to run tolower()
#include <ctype.h>
#include <string.h>
#include <errno.h>

extern int errno;

/* Constants */
#define DISCOUNT_MIN_AGE 18
#define DISCOUNT_MAX_AGE 30
#define MAX_SALES 60
#define DISCOUNT_PERCENTAGE 0.15f
#define MENU_OPTION_BUY_CAR 'a'
#define MENU_OPTION_VIEW_SALES 'b'
#define MENU_OPTION_EXIT 'x'
#define MENU_OPTION_ZERO 0
#define MENU_OPTION_ONE 1
#define MENU_OPTION_TWO 2
#define MENU_OPTION_THREE 3
#define MENU_OPTION_FOUR 4
#define TRUE 1
#define FALSE 0
#define CSV_FILE "CarSalesRecord.csv"
//defining all combinations to be used to make an apealing menu to the user 
#define BOLD "\x1B[1m"      //Bold Text Formula...
#define RED   "\x1B[31m"
#define RESET "\x1B[0m"
#define GRN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define YEL   "\x1B[33m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
// min value: 0     max value: 255
#define bool unsigned char

/* Variables */
unsigned short carsAvailable = 65 ;
float totalSales[MAX_SALES] ;
// this variable will gradually build up as the number of sales increases 
unsigned short numberOfSales = 0;
char customerNames[MAX_SALES][201] ;
//this array will hold all the car Prices available
float carPrices [] = { 7000.0f , 10000.0f, 40000.0f , 50000.0f , 55000.0f } ;
// this array will hold the car Models in the program
char carTypes[] [20] = {"Ford Ranger" , "Toyota Corolla","Mercedes Benz C200","Nissan Patrol" , "Volkswagen Amarok" } ;
unsigned short carYears [] = { 2010, 2013 ,2015 , 2020 , 2022 };
unsigned short carsAvailablePerType [] = {8 , 10 , 12 , 20 , 15 } ;
// this array will hold the number of cars sold each sale
unsigned short carsPerSale[MAX_SALES];
// this array will hold the type of the cars sold each sale; this relates to the car price
unsigned short typeOfCarPerSale[MAX_SALES];
bool discountGivenPerSale[MAX_SALES];
unsigned short carYearPerSale[MAX_SALES]; 
// this struct will hold car related variables that will be displayed in a menu
struct carInventory
{
    float carPrices  ;
    unsigned short carsAvailablePerType ;
    char carTypes  [20];

} ;


// Files Variables & Constants
#define FILE_OPENED 0
#define FILE_CLOSED 1
#define FILE_ERROR 2
// will hold the file stream once opened
FILE* file;
unsigned char fileStatus = FILE_CLOSED;
// making this a function will make it easier to call the function as it will be used quite often and overall make the code easier to read
void clearScreen() {
    system ("cls") ;

}

char getCharFromConsole(char message[201]) {
    char userInput;
    // prompt user using the value of the given message
    printf("%s" ,message);
    // capture the character
    scanf("\n%c", &userInput);
    // return the captured value
    return userInput;
}

void getStringFromConsole(char message[201], char* str) {
    // prompt user
    printf("%s" , message);
    // as we're now working with the pointer str, we don't need & here
    scanf("\n%[^\n]s", str);
}

unsigned short getUnsignedShortFromConsole(char message[201]) {
    // will be used to store the captured number
    unsigned short userInput;

    // prompt user using the value of the given message
    printf("%s" , message);
    scanf("%hd", &userInput);

    // finally, return/export the value so it can be used

    return userInput;
}
void swapBool(bool* a, bool* b) {
    bool temp;
    // copy the value found at the pointer a's address into the newly created temp variable
    temp = *a;
    // copy the value found at the pointer a's address into the address represented by the pointer b
    *a = *b;
    // copy the value of the temp variable to the pointer b's address
    *b = temp;

}

/* START FILES FUNCTIONS */


void pauseProgram(char userChoice) {
    // give the user a chance to read the previous output, allow them to continue when ready
    // customise the output depending on the user's choice
    if (userChoice == MENU_OPTION_EXIT) {
        printf("\n\nPress Enter to Exit...");
    }
    else {
        printf("\n\nPress Enter to return to the Menu...");
    }

    // two of these to skip the newline character that's likely floating around the console window
   if (userChoice != '_') {
		getchar();
	}

	getchar();
}
// returns something that is not NULL if successful
FILE* createFile(char fileName[201]) {
    // open the file for writing
    file = fopen(fileName, "w");
    // then close it immediately if successful
    if (file != NULL) {
        fclose(file);
    }
    // return the pointer so that the success may be checked
    return file;
}

// mode is one of: "w" for writing, "r" for reading, "a" for appending
// appending means writing starting from the end of the file, will not touch existing contents
// there are also other modes available, I encourage you to look these up
void openFile(char fileName[201], char mode[4]) {
    // attempt to open the file
    file = fopen(fileName, mode);

    // if file's value is NULL, then something happened...
    if (file == NULL) {

        // attempt to create it first and then check if it's still NULL, meaning
        //		that something still failed
        if (createFile(fileName) == NULL) {
            fileStatus = FILE_ERROR;

            // will produce the output: Error opening D:\carSalesRecord: No such file of directory
            printf("Error opening %s: %s", fileName, strerror(errno));
        }
        else {
            // we managed to create the file ... so now ...
            // execute this function again with the same parameters
            //		because it should now succeed
            openFile(fileName, mode);
        }

    }
        // otherwise, all OK
    else {
        fileStatus = FILE_OPENED;
    }
}

// should be called once the operations on
//		the currently opened file have concluded
void closeFile() {
    // only attempt to close the file if it's already open
    if (fileStatus == FILE_OPENED) {
        fclose(file);
        fileStatus = FILE_CLOSED;
    }
}

// will return an array of strings representing the
//		customer names stored in the file
void readDataFromFile() {
    // will keep track of how many lines were read from the file
    int lineCounter = 0;

    // this is an infinite loop, we'll manually stop it once we reach the end of the file
    while (1) {
// these VARIABLES  will store what we are reading from the existing file 
        unsigned short carsPerSaleValue = 0, typeOfCarPerSaleValue = 0, discountGivenPerSaleValue = 0,  carYearPerSaleValue = 0 ;
        char customerNameValue[201] = "";

        int scanResult = fscanf(
                file, // the file stream
                "%hd,%hd,%hd,%hd,%[^\n]s", // the format of the line
                &carsPerSaleValue, // the variables, one for each placeholder in the format above
                &typeOfCarPerSaleValue,
                &carYearPerSaleValue ,
                &discountGivenPerSaleValue,
                &customerNameValue
        );

        // if we reached the end of the file
        if (scanResult == EOF) {
            // then, stop the loop
            break;
        }

        // add the bits of data that were read above into the correct arrays
        carsPerSale[lineCounter] = carsPerSaleValue;
        typeOfCarPerSale[lineCounter] = typeOfCarPerSaleValue;
        carYearPerSale[lineCounter] = carYearPerSaleValue;
        // hele i will cast  the discountGivenPerSaleValue from unsigned short to a bool type
        //		before putting it in the discountGivenPerSale array
        discountGivenPerSale[lineCounter] = (bool)discountGivenPerSaleValue;
        // strcpy will allow us to cast the read value as we are dealing with strings 
        strcpy(customerNames[lineCounter], customerNameValue);

        // increment the lineCounter, ready for next line that might be read
        lineCounter++;
        // subtract cars that were sold that had been stored in the csv file 
         carsAvailable -= carsPerSaleValue;
    
    // the line of code below is to update the total sales for each car and to store them in their respective variables 
// as well as to subtract the cars sold for each car Model . This will also update the carsAvailableperType when seeing the menu
// this is done this way to update the carsAvailablePerType by subrating from the values stored in the file
    if (typeOfCarPerSale[lineCounter] == MENU_OPTION_ZERO)
    {
carsAvailablePerType[MENU_OPTION_ZERO] -= carsPerSale[lineCounter] ;
    }
    else if (typeOfCarPerSale[lineCounter] == MENU_OPTION_ONE)
    {
    carsAvailablePerType[MENU_OPTION_ONE] -= carsPerSale[lineCounter] ;
    }
    else if (typeOfCarPerSale[lineCounter] == MENU_OPTION_TWO)
    {
    carsAvailablePerType[MENU_OPTION_TWO] -= carsPerSale[lineCounter] ;
    }
    else if (typeOfCarPerSale[lineCounter] == MENU_OPTION_THREE)
    {
    carsAvailablePerType[MENU_OPTION_THREE] -= carsPerSale[lineCounter] ;
    }
    else if (typeOfCarPerSale[lineCounter] == MENU_OPTION_FOUR)
    {
     carsAvailablePerType[MENU_OPTION_FOUR] -= carsPerSale[lineCounter] ;
    } ;
    }
    // make sure the numberOfSales variable is also aware of how many sales are available after the above operation
    numberOfSales = lineCounter;
   
}

void getDataFromFile() {
    openFile(CSV_FILE, "r");

    if (fileStatus == FILE_OPENED) {
        readDataFromFile();
    }
    else if (fileStatus == FILE_ERROR) {
        printf("There was an error trying to read from the file %s.", CSV_FILE);
        // this function requires a char value, so we give it one
        //		that'll tell it we're using it because of a file error
        //			see the function body, it's been updated to check for
        //			this underscore character
        pauseProgram('_');
    }

    closeFile();
}

void writeDataToFile() {
    // loop through every sale
    for (int i = 0; i < numberOfSales; i++) {

        // this string will be built up bit by bit before being written to the opened file
        char line[201];
        // this string will contain the converted int value
        char data[50];

        // convert the unsigned short value into a string, put it in the data string variable;
        // as I was using MAC I had to find an alternative to _itoa which will be sprintf 
        // sprintf stands for “String print”. Instead of printing on console, it stores output on char buffer.
        // Gupta, M. (2022) Printing heart pattern in C, GeeksforGeeks. 
        // Available at: https://www.geeksforgeeks.org/printing-heart-pattern-c/ (Accessed: December 8, 2022). 
       sprintf(data, "%d",(int)carsPerSale[i]);
        // add the amount of cars to the line; first time we use strcpy,
        // then strcat which aids in building up our line and data 
        strcpy(line, data);
       // add a comma to separate this value as this is our delimiter for the file .csv
        strcat(line, ",");

        // convert the value into a string
        sprintf(data,"%d" ,(int)typeOfCarPerSale[i]);
        // add the type of car to the line
        strcat(line, data);
        // add a comma to separate this value as this is our delimiter for the file .csv
        strcat(line, ",");

         sprintf(data,"%d" ,(int)carYearPerSale[i]);
        // add the car Year to the line
        strcat(line, data);
// add a comma to separate this value as this is our delimiter for the file .csv
        strcat(line, ",");

        // convert the value into a string
        sprintf(data, "%d" ,(int)discountGivenPerSale[i]);
        // add the discount given to the line
        strcat(line, data);
       // add a comma to separate this value as this is our delimiter for the file .csv
        strcat(line, ",");

        // add the customer name to the line
        strcat(line, customerNames[i]);

        // write line to file
        fprintf(file, "%s", line);

        // only add a newline character if we're not yet writing the very last
        //		line in the file
        if (i < numberOfSales - 1) {
            fprintf(file, "\n");
        }
    }
}

void saveDataToFile() {
    // opening the file in 'w' mode means we are writing the user input to the file so that it can be saved and used later on 
    openFile(CSV_FILE, "w");

    if (fileStatus == FILE_OPENED) {
        writeDataToFile();
    }
    else if (fileStatus == FILE_ERROR) {
        printf("There was an error trying to write to the file %s.", CSV_FILE);
        // this function requires a char value, so we give it one
        //		that'll tell it we're using ibt because of a file error
        //			see the function body, it's been updated to check for
        //			this underscore character
        pauseProgram('_');
    }
// closing file when we are done.
    closeFile();
}/* START FILES FUNCTIONS */




/* Rest of the program */
void startOFCarProgram() {
    // here I am making the printf bold and red so that it is apealing to the user and catches the eye 
    printf(RED BOLD "\n\n                       Welcome to the Car sales!\n\n                               " RESET );
}



void swapUnsignedShort(unsigned short* a, unsigned short* b) {
    unsigned short temp;
    // copy the value found at the pointer a's address into the newly created temp variable
    temp = *a;
    // copy the value found at the pointer a's address into the address represented by the pointer b
    *a = *b;
    // copy the value of the temp variable to the pointer b's address
    *b = temp;
}

void menu_showMenu() {
    // present the various actions the user can choose from
    //Using defines very easily without a need for '%s' of '\x1B[...'
    // menu options are made different colours to make it appealing to user
    printf(RED"Menu:\n" RESET);
    printf(GRN "%c. Buy Cars\n" RESET, MENU_OPTION_BUY_CAR );
    printf(YEL "%c. View Sales Stats\n"RESET, MENU_OPTION_VIEW_SALES);
    printf(BLU "%c. Exit\n\n"RESET, MENU_OPTION_EXIT);
}


void menu_showCarTypes(struct carInventory str_arr[]) {


// below I am making the printf result bold to make it more aesthetically pleasing 
    printf( BOLD "\n            These are the cars we have to offer:\n              " RESET);

// this loop makes it easier to output struct members and reduces clatter withing the code 
// I had to use the variable carsAvailablePerType here because the struct member was not updating accordingly 
    for (int i = 0; i< 5; i++) {
        printf(CYN "%d - %s - Price: %.2f GBP - Cars In Stock: %hd \n" RESET, i, str_arr[i].carTypes, str_arr[i].carPrices,
               carsAvailablePerType[i]);
    };
}





void menu_showCarYears()
{ 
    printf( BOLD "\n                Car Years:\n                 " RESET);
    for (int i = 0; i < 5; i++) {
        printf(MAG "%d. %hd \n" RESET, i,carYears[i] );
    }
}
float menu_applyDiscount(float currentPrice) {

    return currentPrice * (1 - DISCOUNT_PERCENTAGE);
}
bool menu_checkIfDiscountIsNeeded(unsigned short userAge) {
    do {
    if (userAge >= DISCOUNT_MIN_AGE && userAge <= DISCOUNT_MAX_AGE) {
        return TRUE;
    }
    else   {
        char nusCardResponse = getCharFromConsole("\nDo you have an NUS card ? Answer 'y' or 'n' : ");
        // This is to ensure that the correct value is stored even if the user has inputted either an uppercase 'Y' or 'N'
        nusCardResponse = tolower(nusCardResponse) ;

        if (nusCardResponse == 'y' )
        {

            return TRUE;
        }
        else if (nusCardResponse == 'n')
        {
            printf("\n\nSorry, we are unable to offer you a discount at this moment in time\n\n");
            return FALSE;
        }
        else {
        printf("\n\n Please enter either 'y' or 'n' \n\n") ;
        return 0;
        }
    
        getchar();
        getchar();
        }
} 
while (getchar() != '\n');
}
void menu_printDiscountOutcome(bool giveDiscount) {
    switch (giveDiscount) {
        case TRUE :
            printf("\n\n You get a discount!");
            break;
        case FALSE  :
            printf("\n\n No discount given. ");
            break;
    }
}


void menu_buyCars()  {
                                   
     struct carInventory carsInfo [5] = {
                                            {7000.0f,8 ,"Ford Ranger"} ,
                                            { 10000.0f,  10 , "Toyota Corolla"} ,
                                            {40000.0f, 12, "Mercedes Benz C200"} ,
                                            { 50000.0f, 20, "Nissan Patrol"} ,
                                            {55000.0f, 15, "Volkswagen Amarok"} ,
                                        } ;
        ; 
     // I put spaces within priintf to centralize the 
    printf( BOLD "                         Buy your dream Car:                      \n\n" RESET);
    // This is to check if there's at least 1 car available for sale
    if (carsAvailable > 0) {
        printf("There are %hd cars available. We have a range of cars to choose from.\n\n", carsAvailable);
    } else {
        printf("Sorry, there are no more cars available .\n\n");
        return;
    }




    unsigned short carsNeeded = 0, userAge = 0, carType ,carYearChoice;
    bool giveDiscount = FALSE;
    float totalPrice = 0;

    getStringFromConsole ("What is your name? Name: ", &customerNames[numberOfSales]);

// This to ensure that no cars are registered as bought
    userAge = getUnsignedShortFromConsole("How old are you? Age: ");
    if (userAge< DISCOUNT_MIN_AGE)
    {printf("Sorry we are not able to offer you any of our cars as you are too young to acquire a driving license\n\n");
        return;
        
    }
    else {
         giveDiscount = menu_checkIfDiscountIsNeeded(userAge);
    }
   // do while loop here ensures that the program will keep on prompting user until a valid input is done     
do {
    menu_showCarTypes(carsInfo) ;
        carType = getUnsignedShortFromConsole("\nWhat type of car do you need? \n Choose from our wide range above: ");
    

       // The IF statement has been done this way to improve readability
      if (carType == MENU_OPTION_ZERO || carType == MENU_OPTION_ONE || carType == MENU_OPTION_TWO
      || carType == MENU_OPTION_THREE || carType == MENU_OPTION_FOUR) {
       break;
        } else {
          //validation to ensure that customer inputs valid choices 
          printf( BOLD" \n\nPlease enter one of our options which are either 0 , 1 , 2 , 3 or 4\n\n" RESET);
                 
        }
          }
          // WHILE statement broken up like this to improve readability
           while (carType != MENU_OPTION_ZERO ||
           carType != MENU_OPTION_ONE ||
           carType != MENU_OPTION_TWO ||
           carType != MENU_OPTION_THREE ||
           carType != MENU_OPTION_FOUR);

        carsNeeded = getUnsignedShortFromConsole("Great! \n How many cars would you like to buy? Amount:  ");
        // this is to check whether  we have enough cars for the user
        if (carsAvailable < carsNeeded ||  carsAvailablePerType[carType] < carsNeeded) {
            printf("Sorry , there are fewer cars remaining than you require.\n\n");
            // this is to exit the program early to avoid errors being inputted when there are no cars available 
            return;
        }
        
        carsPerSale[numberOfSales] = carsNeeded;   
// do while  and if statement are used to ensure that user enters valid information before moving on  
    do {
        menu_showCarYears() ;
        carYearChoice = getUnsignedShortFromConsole("Good choice! What Year do you want " ) ;
        //validation to ensure that customer inputs valid choices 
        if (carYearChoice >= 0 && carYearChoice < 5 )
        { break;
        }
        else {
            // error message made to be bold to ensure that user sees that there is an invalid input
         printf(BOLD"Please enter a correct option from our menu\n\n" RESET) ;
    } ;

    } while (carYearChoice != MENU_OPTION_ZERO ||
             carYearChoice != MENU_OPTION_ONE ||
            carYearChoice != MENU_OPTION_TWO ||
            carYearChoice != MENU_OPTION_THREE ||
            carYearChoice != MENU_OPTION_FOUR ) ;

    // Next, we are storing the user inputs in arrays that will be used in other functions 
    carYearPerSale[numberOfSales] = carYearChoice;
        typeOfCarPerSale[numberOfSales] = carType;

        // calculate totalprice
        totalPrice = carsNeeded * carsInfo[carType].carPrices ;

                            


// update number of cars available per Type  by subtracting from it the amount of cars needed . 
// update carsAvailable by subtracting from the cars Bought.
carsAvailablePerType[carType]  -= carsNeeded;
carsAvailable -= carsNeeded ;
    // actually apply the discount if needed
    if (giveDiscount == TRUE) {
        totalPrice = menu_applyDiscount(totalPrice);
        discountGivenPerSale[numberOfSales] = giveDiscount;
    }

    menu_printDiscountOutcome(giveDiscount) ;
    // present the outcome
    printf("\nThank you.\n");
    printf("You have bought %hd cars.\n", carsNeeded );
    printf("Total cost is %.2f GBP.\n", totalPrice);

    printf("\n\nThere are %hd cars remaining in total and the updated stock for %s is %hd", carsAvailable, carsInfo[carType].carTypes, carsAvailablePerType[carType]);

    // finally, add 1 to the numberOfSales counter
    numberOfSales++;
}

void sortArraysByNumberOfCarsSoldPerSale() {

    // set up a loop that gives us an index "i" for accessing
    //		between the (first) and (second to last) positions that contain values
    // Source : Dinita, D.R.-I.  MOD003212  Introduction to  Programming Week  10. Available at: https://canvas.anglia.ac.uk/courses/27030/pages/week-10-slides?module_item_id=1549049 (Accessed: November 30, 2022). 
    for (int i = 0; i < numberOfSales - 1; i++) {

        // set up a loop the gives us an index "j" for accessing
        //		between the (first immediately after i) and (last) positions that contain values
        // this loop will execute fully, from start to finish,
        //		every time the above for loop begins a new iteration
        for (int j = i + 1; j < numberOfSales; j++) {

            // check if the value at position i is greater than the value at position j
            if (carsPerSale[i] < carsPerSale[j]) {

                // if so, swap those two values in the carsPerSale array
                swapUnsignedShort(&carsPerSale[i], &carsPerSale[j]);

                // also swap the two values at those same positions in the typeOfCarPerSale array
                swapUnsignedShort(&typeOfCarPerSale[i], &typeOfCarPerSale[j]);

               // also swap the two values at those same positions in the carYearPerSale array
                swapUnsignedShort(&carYearPerSale[i], &carYearPerSale[j]) ;
               
                // and in the discountGivenPerSale array
                swapBool(&discountGivenPerSale[i], &discountGivenPerSale[j]);

                // and lastly, do the same in the customerNames array

                // using a function to perform this swap would complicate this program a bit too much, but we
                //		can do the swap directly here quite easily
                char temp[201];
                // copy string from position i to the newly created temp variable
                strcpy(temp, customerNames[i]);
                // copy string from position j into position i
                strcpy(customerNames[i], customerNames[j]);
                // copy string from temp into position j
                strcpy(customerNames[j], temp);

            } // end of "if" statement

        } // end of second "for" loop

    } // end of first "for" loop

}


// specialised function that will display a row representing the sales data at a given array position
void printSalesDataAtPosition( unsigned short position) {

    // here, we're using the value found at position i in the typeOfCarPerSale array
    //	as a position for the carPrices array to find out the price of the type of car
    //	the customer purchased this sale
    int typeOfCar = typeOfCarPerSale[position];
    // calculate the current sale's price
    float price = carsPerSale[position] * carPrices[typeOfCar];

    int carYear = carYearPerSale[position] ;

    // calculate the display text for the discount; this is for the user's benefit
    char discountGivenText[4];
    // if a discount was given, then...
    if (discountGivenPerSale[position] == TRUE) {
        // set the display text to Yes
        strcpy(discountGivenText, "Yes");
        // and also modify the price
        price *= (1 - DISCOUNT_PERCENTAGE);
    }
    else {
        // set the display text to No
        strcpy(discountGivenText, "No");
    }

    // show the user the information related to each sale
    // printf  statement has been put into several lines, so we
    //	can read it more easily, but it's still just one statement
    printf("Sale Index: %d | Sale Amount: %.2f GBP | Cars Bought and Year: %s , %hd| \n" 
           "Car Price: %.2f GBP   | Number of Cars: %hd |"
           "Discount Given: %s | Customer Name: %s\n\n",
            // please note that the above are three separate strings that will be glued together by the program
           position, price, carTypes[typeOfCar], carYears[carYear],
           carPrices[typeOfCar], carsPerSale[position],
           discountGivenText, customerNames[position]);
}

void menu_viewSales() {
    // sort the arrays before showing the sales data
    sortArraysByNumberOfCarsSoldPerSale();

    /* Variables - these are only used inside menu_viewSales() Function and nowhere else */
    // these two will contain the sum the total sales price and total number of cars sold for all sales
    float totalSalesValue = 0;
    unsigned short carsSold = 0;
    
    // this is to store total prices for each car model 
    float totalSalesforCarZero = 0  , totalSalesforCarOne = 0 , totalSalesforCarTwo = 0 , totalSalesforCarThree = 0 , totalSalesforCarFour = 0 ;
// done to make program more aesthetically pleasing.
    printf( BOLD "       All Sales Data:\n\n      " RESET);

    // set up a for loop that will execute the block of code as many times as
    // indicated by the numberOfSales variable's value
    for (int i = 0; i < numberOfSales; i++) {

        // here, we're using the value found at position i in the typeOfCarPerSale array
        //	as a position for the carPrices array to find out the price of the type of car 
        //	the customer purchased this sale
        int typeOfCar = typeOfCarPerSale[i];
       
        // calculate the current sale's price
        float price = carsPerSale[i] * carPrices[typeOfCar];

        // use this function to print the row of sales data at position i
        printSalesDataAtPosition(i);

        // here we are updating carssold , totalsalesvalue to output accurate values
        totalSalesValue += price;
        carsSold += carsPerSale[i] ;
       
        
        
// the line of code below is to update the total sales for each car and to store them in their respective variables 
// as well as to subtract the cars sold for each car Model . This will also update the carsAvailableperType when seeing the menu
    if (typeOfCarPerSale[i] == MENU_OPTION_ZERO)
    {
totalSalesforCarZero += price ;
carsAvailablePerType[MENU_OPTION_ZERO] -= carsPerSale[i] ;
    }
    else if (typeOfCarPerSale[i] == MENU_OPTION_ONE)
    {
    totalSalesforCarOne += price ;
    carsAvailablePerType[MENU_OPTION_ONE] -= carsPerSale[i] ;
    }
    else if (typeOfCarPerSale[i] == MENU_OPTION_TWO)
    {
    totalSalesforCarTwo += price ;
    carsAvailablePerType[MENU_OPTION_TWO] -= carsPerSale[i] ;
    }
    else if (typeOfCarPerSale[i] == MENU_OPTION_THREE)
    {
    totalSalesforCarThree += price ;
    carsAvailablePerType[MENU_OPTION_THREE] -= carsPerSale[i] ;
    }
    else if (typeOfCarPerSale[i] == MENU_OPTION_FOUR)
    {
    totalSalesforCarFour += price ;
     carsAvailablePerType[MENU_OPTION_FOUR] -= carsPerSale[i] ;
    } ;
 


    }
clearScreen() ;
    // also display some overall statistics
    printf( BOLD "\n%hd cars have been sold with a total gross value of %.2f GBP. There are %hd cars in our inventory.\n\n" RESET,
           carsSold, totalSalesValue, carsAvailable);

printf( BOLD "                 Please see below for more sales statistics:\n\n                 " RESET) ;

// this is to display the the total sales for each car model
printf( "The total  gross sales for %s is %.2f GBP \n", carTypes[MENU_OPTION_ZERO], totalSalesforCarZero);
printf( "The total  gross sales for %s is %.2f GBP\n", carTypes[MENU_OPTION_ONE], totalSalesforCarOne);
printf( "The total  gross sales for %s is %.2f GBP\n", carTypes[MENU_OPTION_TWO], totalSalesforCarTwo);
printf( "The total  gross sales for %s is %.2f GBP\n", carTypes[MENU_OPTION_THREE], totalSalesforCarThree);
printf( "The total  gross sales for %s is %.2f GBP\n", carTypes[MENU_OPTION_FOUR], totalSalesforCarFour);


}



void menu_exit() {
    printf( BLU BOLD "\n\nThank you for using our program.\nWe hope that next time you will buy from us as we give the best offers on our Cars. \n Take care!\n\n" RESET);
    saveDataToFile();
    //Here we have an output of A's and B's that will make up a heart this will leave the customer with a heartwarming 
    //output and could persuade them to using the program again.
    //Gupta, M. (2022) Printing heart pattern in C, GeeksforGeeks. Available at: https://www.geeksforgeeks.org/printing-heart-pattern-c/ (Accessed: December 8, 2022). 
    int a, b, size = 10;
 
    /* FOR THE APEX OF HEART */
    for (a = size/2; a <= size; a = a+2)
    {
        // FOR SPACE BEFORE PEAK-1 : PART 1
        for (b = 1; b < size-a; b = b+2)       
            printf(RED " ");
 
        // FOR PRINTING PEAK-1 : PART 2
        for (b = 1; b <= a; b++)
            printf("A");
  
        // FOR SPACE B/W PEAK-1 AND PEAK-2 : PART 3
        for (b = 1; b <= size-a; b++)
            printf(" ");
         
        // FOR PRINTING PEAK-2 : PART 4
        for (b = 1; b <= a-1; b++)
            printf("A");
 
        printf("\n");
    }
 
    /*FOR THE BASE OF HEART ie. THE INVERTED TRIANGLE */
 
    for (a = size; a >= 0; a--)
    {
        // FOR SPACE BEFORE THE INVERTED TRIANGLE : PART 5
        for (b = a; b < size; b++)
            printf(" ");
 
        // FOR PRINTING THE BASE OF TRIANGLE : PART 6
        for (b = 1; b <= ((a * 2) - 1); b++)
            printf(RED "B");
 
        printf("\n" RESET ); 
    }

}


int main() {
    // see if there's any data in the file
    getDataFromFile();
    char userChoice;
  // first do while loop is for the  entire program . This keep the program until the user Exits the program
     do {
   // second  do while loop is for the menu which will keep on prompting user to input the correct value from the menu of 3 options 
         do
        {

        clearScreen();

        startOFCarProgram();
        menu_showMenu();

        // the return value supplied by the Function once it finishes will be stored in userChoice
        userChoice = getCharFromConsole("Please choose one: ");
        // this is validation for the menu options so that it will only accept a , b or x and their respective uppercase letters 
        //as the customer might have their caps lock on by mistake 
         userChoice=tolower(userChoice) ;
         // if statement broken up like this to aid in readability of code 
         if (userChoice == MENU_OPTION_BUY_CAR ||
             userChoice == MENU_OPTION_VIEW_SALES ||
             userChoice == MENU_OPTION_EXIT) {
           
         } else { printf("Please enter either a ,b or  x "); 
         return 0;
         }

        }
           while (getchar() != '\n');
        clearScreen();

        // next, we check the user's choice and make a decision based on that
        switch (userChoice) {
            case MENU_OPTION_BUY_CAR:
                menu_buyCars();
                break;

            case MENU_OPTION_VIEW_SALES:
                menu_viewSales();
                break;

            case MENU_OPTION_EXIT:
                menu_exit();
                break;

            

        }
        pauseProgram(userChoice);

    } while (userChoice != MENU_OPTION_EXIT);
    clearScreen();

    printf( BOLD "                \n\nHave a good day!\n\n                              " RESET);
    return 0 ;
}


