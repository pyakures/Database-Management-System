
/* 
Sahil Pyakurel
December 6, 2020
Project 4

This program is a basic form of Datbase Management System coded in C++. It 
will do tasks such as Database creation, deletion, Table creation, delection, 
update, query, and join.
*/


#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <string>
#include <algorithm>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>


using namespace std;


//FUNCTION DECLARATION--------------------------------------------------------------------------
inline bool fexists(const string& name); //checks if a file already exists
void removeR(string& input); // removes '/r' from the input
int positionCounter(string topArguments, string word); // this function counts the position of the column name by looking for the passed strings
string dataAtPosition(string tempCopier, int position); //returns a string, which is the word on an exact position on a tuple
string removeSpace(string input); //removes empty space from table name during table creation. This is to make it compatible with PA1 and PA2

//main-------------------------------------------------------------------------------------
int main(){

    //variables decleration and intitialization
    string input = "empty"; //for storing input
    string inputSmallized ="empty"; //for storing the lowered case input
    string database_name = "empty"; //name of the database
    string table_name = "empty"; //stroing the name of the table

    //using boolean instead of global vaiable to keep track of whether youre inside a database
    bool insideDatabase= 0;
    
    bool lockOn= false; //boolean to keep track of transactions and lock implementaion
    bool error22= false;//to return an error if the table is locked
    
    //looping the ending util .EXIT is recieved
    while (input!=".exit"){
        
        getline(cin, input); // getting input from the customer
        removeR(input); // removing '\r' from my input which was causing issues while auto running the attached .sql file
        inputSmallized = input; //This line is creating a copy of the input string for checking valid statements purposes.
        transform(inputSmallized.begin(), inputSmallized.end(), inputSmallized.begin(), ::tolower); // this function is taking the copy of the input and making all the characters lowercase so my program can recognize either uppercase or lowercase query and inputs from the user

        //if the input is blank do nothing
        if(input=="" || input.find("--")!=-1){ 
            //do nothing
        }
        //creating database/ directory
        else if(inputSmallized.find("create database")!=-1 && input.find(";")!=-1){ //checking if the passed input has create database string and ; to see if its a valid statement
            database_name= input.substr(16, input.length()-17); //parsing database name from the input.

            if(insideDatabase){ //if the program is already inside a database
                chdir("..");        //coming out of that folder
                insideDatabase=0;   //changing the boolean value to false- meaning its outside of a database
            }

            if(mkdir(database_name.c_str(),0777)==0){ // creating a database with the passed database_name value
                cout<< "-- Database " << database_name << " created.\n"; //confirmation
            }
            else{
                cout<< "-- !Failed to create database " << database_name << " because it already exists.\n"; // prompt if the creation of database is failed. Mainly because one folder with that name already exists
            }
        }
        //deleting database/ directory
        else if(inputSmallized.find("drop database")!=-1 && input.find(";")!=-1){//checking if the passed input has drop database string and ; to see if its a valid statement
            database_name= input.substr(14, input.length()-15); // parsing database_name from input

            if(insideDatabase){ //if the program is already inside a database   
                chdir("..");    //coming out of database
                insideDatabase=0; //setting insideDatabase  
            }

            if(rmdir(database_name.c_str())!=-1){ //deleting the folder  with the database_name passed
                cout<< "-- Database " << database_name << " deleted.\n"; //prompts if sucess
            }
            else
            {
                cout<< "-- !Failed to delete " << database_name <<  " because it does not exist.\n"; //prompts if it cannot delete folder with that name
            }
        }
        //going inside a database/directory
        else if(inputSmallized.find("use")!=-1 && input.find(";")!=-1){//checking if the passed input has use string and ; to see if its a valid statement
            if(insideDatabase){ // using boolean variable to track if we are already inside of another database
                chdir(".."); //if yes, coming out of database
                insideDatabase=0; //setting inside database to false
            }
            database_name= input.substr(4, input.length()-5);  // parsing the new database name from the input
            
            if(chdir(database_name.c_str())!=-1){ //goes inside directory
                cout << "-- Using database " << database_name << ".\n"; // prompts if sucess
                insideDatabase=1; //changes the insideDatabase to true, which means the program is inside of a directory
            }
            else{
                cout<< "-- !Failed to use database " << database_name << " because it does not exist.\n"; //prompts if fails to go inside the database
            }
        }
        //creating a table/file inside a database/directory
        else if(inputSmallized.find("create table")!=-1 && input.find(";")!=-1){ //checking if the passed input has create table string and ; to see if its a valid statement
            table_name= input.substr(13, input.find("(")-13); //parsing the table_name from input
            transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower); 
            table_name= removeSpace(table_name); //to remove space from table name for PA1 and PA2 compatibility

            if(!insideDatabase){ //checking if any database is being used
                cout << "-- !Failed to create a table because you are not using a database.\n"; //prompts if not inside a directory
            }
            else if(fexists(table_name)){ //checking if the file with that filename exixts
                cout << "-- !Failed to create table " << table_name << " because it already exists.\n"; //prompts if table already exists
            }
            else{
                string argument; //stores the argument from input
                ofstream Table(table_name.c_str()); //output file
                
                //extracts the name and type of the attributes from input
                argument = input.substr(input.find("(")+1,input.find(";")-(input.find("(")+2));

               for(int i=0; i<argument.length(); i++){ // goes through each index of argument strings and seperates them into different arguments based on space and commas
                    if(argument[i]==','){ // if it finds a comma
                        Table << " |"; //add | to the file
                    }
                    else if(argument[i]=='\t'){ // if it finds tab
                        //do nothing
                    }
                    else{
                        Table << argument[i]; //else add everthing to the output file
                    }
                }
                Table<< "\n"; //adding a new line

                Table.close();
                if(fexists(table_name)){ //checking if the table you just created was sucessful
                    cout << "-- Table " << table_name << " created.\n"; // if it was, prompts the confirmation
                }
            }
     
        }
        //deleting table
        else if(inputSmallized.find("drop table")!=-1 && input.find(";")!=-1){ //checking if the passed input has drop table string and ; to see if its a valid statement
            table_name= input.substr(11, input.length()-12); // parsing the table_name from input
            transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower); 

            if(!fexists(table_name)){ //checks if table exists
                cout << "-- !Failed to delete " << table_name << " because it does not exist.\n"; //prompts if it does
            }
            else{
                remove(table_name.c_str()); //delecting table
                if(!fexists(table_name)){ //checking if the table deletion was sucessful 
                    cout << "-- Table " << table_name << " deleted.\n"; //prompts if it was sucessful
                }
            }
        }
        //selecting and printing from table
        else if(inputSmallized.find("select * from")!=-1 && input.find(";")!=-1){ //checking if the passed input has select * from string and ; to see if its a valid statement
            table_name= input.substr(14, input.length()-15); //parsing table_name from input
            transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower); 

            if(!fexists(table_name)){ //checking if table with that name exixts
                cout << "-- !Failed to query table " << table_name << " because it does not exist.\n"; //prompts if it doesnt exixts
            }
            else{ //if table exixts
                ifstream readTable(table_name.c_str()); //reading from the table
                string temp; //temporary string to read table line by line
                
                //lopping through the file line by line and printing that in terminal.
                while(getline(readTable,temp)){
                    cout << "-- ";
                    for(string::size_type i=0; i<temp.size();i++){ 
                            cout << temp[i];
                    }
                    cout << endl; //new line
                }


            }
    
        }
        //Altering / adding to the table
        else if(inputSmallized.find("alter table")!=-1 && input.find(";")!=-1){//checking if the passed input has alter table string and ; to see if its a valid statement
            table_name= input.substr(12, input.find("ADD")-12-1); //parsing table_name from input
            transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower); 
            if(!fexists(table_name)){ //checks if the table exixts
                cout << "-- !Failed to query table " << table_name << " because it does not exist.\n"; //prompts if the table doesnot exists
            }
            else{ //if the table exixts

                string newAttribute, tempCopier;
                //name of the new attribute
                newAttribute = input.substr(input.find("ADD")+4,input.find(";")-(input.find("ADD")+4)); //parsing the new additional attribute from the argument

                ofstream writeTable("output.txt"); //opening a new output file with temporary name
                ifstream readTable(table_name.c_str()); //opening the table file for reading

                while(getline(readTable, tempCopier)){ //reading from old file and writing to new file
                    if(!tempCopier.empty()){ //if the line is not empty
                        writeTable << tempCopier; //write the already existing attributes to the new file
                        writeTable << "|" << newAttribute; // add | and the new attribute name
                    }
                    writeTable << "\n";
                }

                readTable.close(); //close the reading file
                writeTable.close(); //close the writing file

                remove(table_name.c_str()); //deleting the old file
                if(rename("output.txt",table_name.c_str())!=-1){ //renaming the newfile to the name of oldfile
                cout << "-- Table " << table_name << " modified.\n"; //confirmation
                }
            }
        }
        //inserting new data to a table
        else if(inputSmallized.find("insert into")!=-1 && input.find(";")!=-1){//checking if the passed input has insert into string and ; to see if its a valid statement
            table_name= input.substr(12, input.find("values")-12-1); //extracting the table name from the input
            transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower);
            
            if(!fexists(table_name)){// checking to see if the table with that name exists
                cout << "-- !Failed to query table " << table_name << " because it does not exist.\n"; //printing if it table doesnot exist
            }
            else{
                
                string argument; //string to store the data 
                string tempCopier; //string to help copy the data from old file to new file
                ofstream writeTable("output.txt"); //output file
                ifstream readTable(table_name.c_str()); //reading the old file
                
                argument = input.substr(input.find("(")+1,input.find(";")-(input.find("(")+2)); //extracts the data from the input

                 while(getline(readTable, tempCopier)){ //reading from old file and writing to new file
                    if(!tempCopier.empty()){
                        writeTable << tempCopier;
                    }
                    writeTable << "\n";
                }

                //printing the data to the outfile file/ table
                for(int i=0; i<argument.length(); i++){
                    if(argument[i]==','){ // checking if the data contains , 
                        writeTable << "|";
                    }
                    else if(argument[i]==' '||argument[i]=='\t'||argument[i]=='\''){ // checking if the data contains space or tb
                        //do nothing
                    }
                    else{
                        writeTable << argument[i];
                    }
                }

                writeTable<< "\n";
                readTable.close();
                writeTable.close();

                remove(table_name.c_str()); //deleting the old file
                if(rename("output.txt",table_name.c_str())!=-1){ //renaming the newfile to the name of oldfile
                cout << "-- 1 new record inserted.\n"; //confirmation
                }
                
                
            }

        } 
        //updating the data of the table
        else if(inputSmallized.find("update")!=-1){//checking if the passed input has update string to see if its a valid statement
            string temporaryInput;// to store string until ; is found 
            while(input.find(";")!=-1){
                getline(cin, temporaryInput); //getting line
                input = input + temporaryInput; //concatinating the input
            }

            table_name= input.substr(7, input.find("set")-7); //extracting the table name from the input
            transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower); //making the string lower case
            table_name.erase(remove(table_name.begin(), table_name.end(), ' '), table_name.end());//erasing any white spaces from the input. This is because I was getting an error while auto running the sql because of extra space at the end of tablename
            
            if(!fexists(table_name)){// checking to see if the table with that name exists
                cout << "-- !Failed to update table " << table_name << " because it does not exist.\n"; //printing if it table doesnot exist
            }
            else// if the table exists
            {
                string lockfile= table_name + "_lock.txt"; //creating a lock file name using the passed table name
                if(lockOn && fexists(lockfile)){ //checks weather transaction has being and if a lock file already exists

                    cout << "-- Error: Table " << table_name << " is locked!\n"; // returns error if a lock file already exists
                    error22= true; //sets the error to true meaning their is a locked file for commit
                }
                else{// if the lock is on but theres no lock file yet

                    if(lockOn){  
                        ofstream lockFile1(lockfile);//creating the lock file
                        lockFile1 << "locked\n";
                    }
                    

                    string setArgument ="empty", set[3]; //strings for the parsed set arguments
                    string whereArgument = "empty", where[3]; //strings for the parsed where arguments
                    string tempCopier,tempWord, tempWord2; //extra temporary strings
                    int whereCounter=0, setCounter=0, insideWhereCounter=0,insideSetCounter=0,index=0, jindex=0, modificationCounter=0;// all of the counter required for this part of the program

                        removeR(input);     //again removing '\r' from the commands
                        inputSmallized = input; //lowercased input
                        transform(inputSmallized.begin(), inputSmallized.end(), inputSmallized.begin(), ::tolower); // this function is taking the copy of the input and making all the characters lowercase so my program can recognize either uppercase or lowercase query and inputs from the user

                        
                        if(inputSmallized.find("set")!=-1){ //checking if the input has the word "set" in it

                            setArgument= input.substr(input.find("set")+4, input.find("where")-input.find("set")-5); //parsing the argument from set

                        
                            //this will save the arguments as seperate strings
                            for(int i=0, j=0; i< setArgument.length(); i++){
                                if(setArgument[i]==' '){
                                    j++;
                                } 
                                else if(setArgument[i]=='\''){
                                    //do nothing
                                }
                                else{
                                    set[j].push_back(setArgument[i]); //adding each char to string
                                }
                            }
                            
                        }
                        
                        if( inputSmallized.find("where")!=-1){ //checking if the input has the word "where" in it
                            ifstream readTable(table_name.c_str()); //reading file for the table_name
                            ofstream writeTable("output.txt");  //new temporary writing file

                            whereArgument= input.substr(input.find("where")+6, input.find(";")-input.find("where")); //parsing the argument from where

                            //this will save the arguments as seperate strings
                            for(int i=0, j=0; i< whereArgument.length()-1; i++){
                                if(whereArgument[i]==' '){
                                    j++;
                                }
                                else if(whereArgument[i]=='\''){
                                    //do nothing
                                }
                                else{
                                    where[j].push_back(whereArgument[i]);
                                }
                            }


                            if(where[1]=="="){ //cheking if the argument is =

                                
                                getline(readTable, tempCopier, '\n');//reading the first line from the file

                                //This is for where counter
                                whereCounter= positionCounter(tempCopier,where[0]); //look at the function setion to see what this function does
            
                                //This is for set counter
                                setCounter= positionCounter(tempCopier, set[0]);//look at the function setion to see what this function does

                                writeTable << tempCopier << endl; //writing the headline of the old file to the new file
                                tempWord = "";

                                while(getline(readTable, tempCopier)){ //reading from old file and writing to new file
                                    if(!tempCopier.empty()){
                                        
                                    for(; insideWhereCounter!=whereCounter;index++){ //incriment index until it finds the spot in the head line
                                        if(tempCopier[index]=='|'){
                                            insideWhereCounter++;
                                        }   
                                    }

                                    for(; !(tempCopier[index]=='|'|| index==tempCopier.length()); index++){ // take the word before | and save it in tempWord
                                        tempWord.push_back(tempCopier[index]);
                                    }

                                        if(tempWord == where[2]){ //check if the tempWord matches your argument
                                            modificationCounter++; //counts the number of modification made durring this execution
                                            for(; insideSetCounter!=setCounter;jindex++){ //increment until you find the postion where you will be inserting the new word
                                                if(tempCopier[jindex]=='|'){
                                                    insideSetCounter++;
                                                }
                                                writeTable << tempCopier[jindex]; //copy the rest of the char to the new file until the postion is found
                                            }
                                            writeTable << set[2]; //once postion is found, insert the new value you want to update
                                            jindex++;
                                            while(tempCopier[jindex]!='|'){
                                                jindex++;
                                            }
                                        
                                            for(; jindex<tempCopier.length(); jindex++){ //copy the rest of the char as it is after the one you want to change 
                                            writeTable << tempCopier[jindex];
                                            }
                                            writeTable<< endl;

                                        }
                                        else{
                                            writeTable<< tempCopier<<endl; //if you didnt find the tempWord match, just copy the entire line to the new file
                                        } 
                                    }
                                    index=0;//rest everything for looping purposes
                                    jindex=0;
                                    tempWord = "";
                                    insideWhereCounter=0;
                                    insideSetCounter=0;
                                }
                                
                                
                            }

                            readTable.close();
                            writeTable.close();
                            
                                if(modificationCounter==0||modificationCounter==1){
                                    cout << "-- " <<modificationCounter<< " record modified.\n"; //confirmation based on the modificationCounter(0,1)
                                }
                                else{
                                    cout << "-- " <<modificationCounter<< " records modified.\n";// confirmation for modificationCounter>=2
                                }                               
                

                        }
                        else{
                            cout << "-- Invalid update argument;\n"; //prints this if the argument for update doesnot have either where or set.
                        }
                      

                }
              
            }
                
        }
        //deleting data from a table
        else if(inputSmallized.find("delete from")!=-1)//checking if the passed input has delete from string to see if its a valid statement
        {
            
            table_name= input.substr(12, input.length()-12); //extracting the table name from the input
            transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower); 
            table_name.erase(remove(table_name.begin(), table_name.end(), ' '), table_name.end()); //erasing any space from the table_name; was causing an issue since there was an extra space in table_name

            if(!fexists(table_name)){// checking to see if the table with that name exists
                cout << "-- !Failed to delete from table " << table_name << " because it does not exist.\n"; //printing if it table doesnot exist
            }
            else// if table_name does exist
            {
                
                input= "empty"; // changing the input to default so it does not throw unwanted results

                while(input.find(";")==-1){ //getting input till the command is ended with ;
                
                    getline(cin, input); //getting new line of input
                    removeR(input);     //again removing '\r' from the commands
                    inputSmallized = input; //for lowercased input
                    transform(inputSmallized.begin(), inputSmallized.end(), inputSmallized.begin(), ::tolower); // this function is taking the copy of the input and making all the characters lowercase so my program can recognize either uppercase or lowercase query and inputs from the user

                    if(inputSmallized.find("where")!=-1){ //checking if it finds where in the string
                        
                        string argument, arguments[3], tempCopier, tempWord; // temporary strings need to perform execution
                        int whereCounter=0, insideWhereCounter=0,index=0, deletionCounter=0; // temporary counters

                        ifstream readTable(table_name.c_str()); //read file
                        ofstream writeTable("output.txt");//temporary write file

                        argument= input.substr(6, input.length()-7); //parsing the argument from where 
                    
                        //this will save the arguments as seperate strings
                        for(int i=0, j=0; i< argument.length(); i++){
                            if(argument[i]==' '){
                                j++;
                            } 
                            else if(argument[i]=='\''){
                                //do nothing
                            }
                            else{
                                arguments[j].push_back(argument[i]);
                            }
                        }

                        //does deletion based on the argument passed
                        if(arguments[1]=="="){ //only checks if the argument is a equal
                             
                            getline(readTable, tempCopier, '\n');//reading the first line from the file

                            whereCounter= positionCounter(tempCopier,arguments[0]);// counts the position of the argument in the headline

                            writeTable << tempCopier << endl; //writes the head line to the new file
                            tempWord = "";

                            while(getline(readTable, tempCopier)){ //reading from old file and writing to new file, looping until the end of file
                                if(!tempCopier.empty()){
                                    
                                   for(; insideWhereCounter!=whereCounter;index++){ //increment index until the postion of the where argument
                                       if(tempCopier[index]=='|'){
                                           insideWhereCounter++;
                                       }   
                                   }

                                   for(; tempCopier[index]!='|'; index++){ //saving the name of the argument in a temporary tempWord string
                                       tempWord.push_back(tempCopier[index]);
                                   }

                                    if(tempWord==arguments[2]){// if the tempWord is equal to the passed argument
                                        deletionCounter++; //increment the delection counter
                                    }
                                    else
                                    {
                                        writeTable<<tempCopier<<endl;// only write the values that is not euqal, which means the ones that are euqal will be deleted
                                    }
                                }
                                //resetting counter for looping purposes
                                index=0;
                                insideWhereCounter=0;
                                tempWord = "";
                            }


                        }
                        else if(arguments[1]==">"){ //only checks if the argument is greater than

                            getline(readTable, tempCopier, '\n');//reading the first line from the file

                            whereCounter= positionCounter(tempCopier,arguments[0]);//finding the position in headline

                            writeTable << tempCopier << endl; //writing the headline in new file
                            tempWord = "";

                            while(getline(readTable, tempCopier)){ //reading from old file and writing to new file
                                if(!tempCopier.empty()){
                                    
                                   for(; insideWhereCounter!=whereCounter;index++){ //incrementing until index is equal to the postion of the word
                                       if(tempCopier[index]=='|'){
                                           insideWhereCounter++;
                                       }   
                                   }

                                   for(; tempCopier[index]!='|'; index++){ //extract the value from the file
                                       tempWord.push_back(tempCopier[index]);
                                   }

                                    if(stof(tempWord)>stof(arguments[2])){ //if the value is greater than passed argument
                                        deletionCounter++;//increment the deletion counter, which means the line is deleted
                                    }
                                    else
                                    {
                                        writeTable<<tempCopier<<endl;// if not, write the line as it is in the new file
                                    }
                                }
                                //resetting values for looping purposes
                                index=0;
                                insideWhereCounter=0;
                                tempWord = "";
                            }



                        }
                        //closing the input and output files
                        readTable.close();
                        writeTable.close();
                        remove(table_name.c_str()); //deleting the old file
                        if(rename("output.txt",table_name.c_str())!=-1){ //renaming the newfile to the name of oldfile
                            if(deletionCounter==0||deletionCounter==1){
                                cout << "-- " <<deletionCounter<< " record deleted.\n"; // confimation for deletion (0,1)
                            }
                            else{
                                cout << "-- " <<deletionCounter<< " records deleted.\n";// confirmation for delection>=2
                            }                               
                        }
                    }
                }
            }
        }
         //joining tables
        else if(inputSmallized.find("select *")!=-1){ //checking if the passed input has select * from string
            //variable DECLARATION needed for join
            string tempInput= "";
            string table2 = "";
            string tableIdentifier[2], tableIdentifier2[2], tempCopier, tempCopier2, parsedargument, argument[5];
            int counter1=0, counter2=0;
            
            while(tempInput.find(";")==-1){ //gets input until ; is inserted
                cin >> tempInput;
                input = input + " " + tempInput; //concatnates the temporary input to the input to create a query

            }

            inputSmallized= input;
            transform(inputSmallized.begin(), inputSmallized.end(), inputSmallized.begin(), ::tolower); //setting all char to lower case for the query to work as both upper and lower case input

            if(inputSmallized.find("inner join")!=-1 || inputSmallized.find("where")!=-1){ //if the input contains the words inner join or where

                if(inputSmallized.find("inner join")!=-1){ //this will parse the table names based on if it contains inner join
                    table_name= input.substr(input.find("from")+5, input.find("inner")-input.find("from")-6); //parsing table_name from input
                    transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower);//lower case for compatibility reasons
                    table2= input.substr(input.find("join")+5, input.find("on")-input.find("join")-6);//parsing second table name
                    transform(table2.begin(), table2.end(), table2.begin(), ::tolower);//compatibility

                    parsedargument= inputSmallized.substr(inputSmallized.find("on")+3, inputSmallized.find(";")-(inputSmallized.find("on")+3));//extracting the on argument from the input

                }else{// if the query contains where
                    table_name= input.substr(input.find("from")+5, input.find(",")-input.find("from")-5); //parsing table_name from input
                    transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower); //compatibility
                    table2= input.substr(input.find(",")+2, input.find("where")-input.find(",")-3); //parsing name for second table
                    transform(table2.begin(), table2.end(), table2.begin(), ::tolower);//compatibility

                    parsedargument= inputSmallized.substr(inputSmallized.find("where")+6, inputSmallized.find(";")-(inputSmallized.find("where")+6));//parsing the where argument 
                }

                for(int i=0, j=0; i< table_name.length(); i++){ //extraacting the table name and table identifier
                    if(table_name[i]== ' '){ 
                        j++;
                    }
                    else{
                        tableIdentifier[j].push_back(table_name[i]); //storing it in an array of strings
                    }
                    
                }
                for(int i=0, j=0; i< table2.length(); i++){ //doing the same thing for second table
                    if(table2[i]== ' '){
                        j++;
                    }
                    else{
                        tableIdentifier2[j].push_back(table2[i]); //storing in an array of strings
                    }
                    
                }

                if(!(fexists(tableIdentifier[0])&&fexists(tableIdentifier2[0]))){ //checking whether both table name exists
                    cout << "-- Table doesnot exist!\n"; //returns if it doeesnt exist
                    
                }else{
                    
                    //cross multiplication

                    ofstream writeTable("temp.txt");//temporary write file
                    ofstream without("output.txt");
                    ifstream ReadTable1(tableIdentifier[0].c_str()); //reading the first table
                    ifstream tempread(tableIdentifier2[0].c_str()); //reading the second table

                    //reading the headline for both tables
                    getline(ReadTable1,tempCopier); 
                    getline(tempread,tempCopier2);
                    
                    writeTable << tempCopier << " | " << tempCopier2<<endl; // writing the headline for both tables to the new file

                    while(getline(tempread,tempCopier2)){
                        without<< tempCopier2<< endl; // making a another new file without the header 
                    }
                    without.close();//closing file
                    tempread.close();// closing file
                    ifstream ReadTable2("output.txt"); //opening the new made file to read

                    while(getline(ReadTable1,tempCopier)){// cross multiplication between all values of table1 and table2
                        ReadTable2.clear(); //resetting the cursor so that it will begin reading from the top of the file again
                        ReadTable2.seekg(0, ReadTable2.beg);
                        while(getline(ReadTable2, tempCopier2)){
                            writeTable<< tempCopier <<"|" << tempCopier2<<endl;// writing the data from the first time will all of the data from the second table
                        }
                    }
                    ReadTable2.close(); //closing the file
                    remove("output.txt"); //deleting the output.txt file without the header
                    writeTable.close(); //closing the file
                    ReadTable1.close();//closing the file


                    for(int i=0, j=0; i< parsedargument.length();i++){ //paarsing the arguments in an array
                        if(parsedargument[i]=='.'||parsedargument[i]==' '){//incrementing if the char is . or ' '
                            j++;
                        }else
                        {
                            argument[j].push_back(parsedargument[i]);
                        }
                    }
                    
                    //getting rid of unmatched results

                    ifstream tempoRead("temp.txt"); //reading the crossed multiplied file
                    
                    if(argument[2]=="="){ //checking if the parsed argument contains =

                        if(argument[0]==tableIdentifier[1]&& argument[3]==tableIdentifier2[1]){//if the identifier matches the argument

                            getline(tempoRead, tempCopier); //reading the header
                            counter1= positionCounter(tempCopier, argument[1]);//finding the position of the argument in the temp file header
                            counter2= positionCounter(tempCopier, argument[4]);//finding the position of the argument in the temp file header

                            cout << "-- " << tempCopier << endl; //printing THE HEADER to the console

                            while(getline(tempoRead, tempCopier)){
                                if(dataAtPosition(tempCopier, counter1)==dataAtPosition(tempCopier, counter2)){ //going through the file and printing if the arguments match
                                    cout << "-- "<< tempCopier;
                                    cout << endl;
                                }
                               
                            }

                        }


                    }
                    else{
                        cout << "-- Invalid Argument!\n";
                    }

                    tempoRead.close();
                    remove("temp.txt"); // deleting the temporary file

                }

            }
            else if(inputSmallized.find("left outer join")!=-1){ //if the query includes left outer join

                table_name= input.substr(input.find("from")+5, input.find("left")-input.find("from")-6); //parsing table_name from input
                transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower); // compatibility lower case
                table2= input.substr(input.find("join")+5, input.find("on")-input.find("join")-6);// parsing second table
                transform(table2.begin(), table2.end(), table2.begin(), ::tolower);// lower case compatibility

                parsedargument= inputSmallized.substr(inputSmallized.find("on")+3, inputSmallized.find(";")-(inputSmallized.find("on")+3)); // parsing the on arguement

                for(int i=0, j=0; i< table_name.length(); i++){ //parsing the table identifier for table1
                    if(table_name[i]== ' '){
                        j++;// incrementing if ' ' is found
                    }
                    else{
                        tableIdentifier[j].push_back(table_name[i]); 
                    }
                    
                }
                for(int i=0, j=0; i< table2.length(); i++){//parsing the table identifier for table 2
                    if(table2[i]== ' '){
                        j++;//incrementing if ' ' is found
                    }
                    else{
                        tableIdentifier2[j].push_back(table2[i]);
                    }
                    
                }

                if(!(fexists(tableIdentifier[0])&&fexists(tableIdentifier2[0]))){
                    cout << "-- Table doesnot exist!\n"; // returns if the either one of the table doesnt exists
                    
                }else{
                    ofstream writeTable("temp.txt");//temporary write file
                    ofstream without("output.txt");
                    ifstream ReadTable1(tableIdentifier[0].c_str()); //reading from table1
                    ifstream tempread(tableIdentifier2[0].c_str());// reading from table2

                    //reading the headline of tables
                    getline(ReadTable1,tempCopier); 
                    getline(tempread,tempCopier2);
                    
                    writeTable << tempCopier <<" | " << tempCopier2<<endl; //concatinating the two header in a new temporary file

                    while(getline(tempread,tempCopier2)){
                        without<< tempCopier2<< endl; //creating a new file with all data except the header file
                    }
                    without.close();
                    tempread.close();
                    ifstream ReadTable2("output.txt"); //reading from the created temporary file

                    while(getline(ReadTable1,tempCopier)){ 
                        ReadTable2.clear(); //resetting the reading cursor so it allows to reread a file
                        ReadTable2.seekg(0, ReadTable2.beg); 
                        while(getline(ReadTable2, tempCopier2)){
                            writeTable<< tempCopier <<"|" << tempCopier2<<endl; //cross multiplying 
                        }
                    }
                    ReadTable2.close();
                    remove("output.txt"); //deleting the temporary file
                    writeTable.close();

                    
                    //parsing the arguments from the main argument in an array
                    for(int i=0, j=0; i< parsedargument.length();i++){
                        if(parsedargument[i]=='.'||parsedargument[i]==' '){
                            j++;
                        }else
                        {
                            argument[j].push_back(parsedargument[i]);
                        }
                    }

                    //getting rid of unmatched results
                    
                    
                    if(argument[2]=="="){ //checking if the argument is =

                        if(argument[0]==tableIdentifier[1]&& argument[3]==tableIdentifier2[1]){ //making sure that the table identifier is the same as the passed identifier
                            
                            ifstream tempoRead("temp.txt");
                            ofstream tempoWrite("output.txt");

                            getline(tempoRead, tempCopier); //reading the header line
                            counter1= positionCounter(tempCopier, argument[1]); //finding the position of the argument in header line
                            counter2= positionCounter(tempCopier, argument[4]);// finding the position of the argument in header line

                            tempoWrite << tempCopier << endl; // writing the header file in a new file
        
                            while(getline(tempoRead, tempCopier)){
                                if(dataAtPosition(tempCopier, counter1)==dataAtPosition(tempCopier, counter2)){ //getting rid of the items that do not match
                                    tempoWrite << tempCopier;
                                    tempoWrite << endl;
                                }
                            }
                            tempoRead.close();
                            remove("temp.txt"); //deleting the temporary file
                            tempoWrite.close();

                            ifstream readingAgain("output.txt");
                            ReadTable1.clear(); //resetting the cursor back to the beginning of the file
                            ReadTable1.seekg(0, ReadTable1.beg);

                            getline(readingAgain, tempCopier);
                            cout << "-- "<< tempCopier <<endl; //printing the header file to the console
                            
                            bool found= false; //a bool variable to keep track if certain things are found
                            getline(ReadTable1, tempCopier2); 
                            

                            while(getline(ReadTable1,tempCopier2)){
                                readingAgain.clear();//resetting the cursor
                                readingAgain.seekg(0, readingAgain.beg);
                                while(getline(readingAgain, tempCopier)){ //nested loop to check if the things from the left table are there in the cross multiplied table
                                    if(dataAtPosition(tempCopier, 0)==dataAtPosition(tempCopier2, 0)){
                                        found= true;
                                        cout << "-- "<< tempCopier<<endl;// if yes, it will print the data from the cross multiplied table
                                    }
                                }
                                if(found == false){
                                    cout << "-- "<< tempCopier2 <<"||\n"; //if not, it will print the data from the original left table
                                }
                                found= false;//resets the found to false
                            }

                            readingAgain.close();
                            remove("output.txt");// deletes the temporary file,
                        }
                    }
                    else{
                        cout << "-- Invalid Argument!\n";
                    }
                }
            }
            else{
                cout << "-- Join Invalid!\n";
            }
        }
        //selecting couple of columm form a table
        else if(inputSmallized.find("select")!=-1){//checking if the passed input has select string to see if its a valid statement
            
            string input2="empty"; //a new string for the second input and the first input holds value for the select argument
            string Narguments[10], whereArguments[3];// currenlty setting the number of arguments to only 10
            string mainArgument, whereArgument, tempCopier, tempWord;//temporary stings for execution purposes
            int counterForAll[10]={}, whereCounter=0, insideCounter=0, insideWhereCounter=0, index=0, jindex=0;//temporary counters
            bool unknownArgument=0; //to check if the input colums names are found in the headline

            mainArgument= input.substr(7, input.length()-7); //parsing the main argument from input

            while(input2.find(";")==-1){ //getting input till the command is ended ;

                getline(cin, input2); //getting new line of input
                removeR(input2);     //again removing '\r' from the commands
                
                if(input2.find("from")!=-1){ //if the input has from in it
                    table_name= input2.substr(5, input.length()-4); //extracting the table name from the input
                    transform(table_name.begin(), table_name.end(), table_name.begin(), ::tolower); 
                    table_name.erase(remove(table_name.begin(), table_name.end(), ' '), table_name.end()); //removing any whitespaces like before

                    if(!fexists(table_name)){// checking to see if the table with that name exists
                        cout << "-- !Failed to select from table " << table_name << " because it does not exist.\n"; //printing if it table doesnot exist
                        break;// since the table doesnt exist there is no point of doing anything else, so break will end the loop
                    }

                }
                else if(input2.find("where")!=-1){ //if the input has where

                     //this will save the arguments as seperate strings
                    for(int i=0, j=0; i< mainArgument.length(); i++){
                        if(mainArgument[i]==','){
                            j++;
                        } 
                        else if(mainArgument[i]==' '){
                            //do nothing
                        }
                        else{
                            Narguments[j].push_back(mainArgument[i]);
                        }
        
                    }
                    
                    ifstream readTable(table_name.c_str()); //reading file
                    getline(readTable, tempCopier, '\n');//reading the first line from the file
                
                    for(int i=0; i<10; i++){
                        counterForAll[i]= positionCounter(tempCopier,Narguments[i]); // couting the position in the headline, in this case for 10 arguments
                    }

                    whereArgument= input2.substr(6, input2.length()-7); //parsing the main argument from where

                    //this will save the arguments as seperate strings
                    for(int i=0, j=0; i< whereArgument.length(); i++){
                        if(whereArgument[i]==' '){
                            j++;
                        } 
                        else if(whereArgument[i]=='\''){
                            //do nothing
                        }
                        else{
                            whereArguments[j].push_back(whereArgument[i]);
                        }
                    }
                    whereCounter= positionCounter(tempCopier, whereArguments[0]); //counting the postion of argument in the headline

                    for(int random=0; random< 10 ; random++){ //if one of the passed column name doesnt belong to the table
                        if(counterForAll[random]==-2){
                            unknownArgument= 1;//set true for unknown argument
                        }

                    }
                    if(!unknownArgument){// if there are no unknownArgument/ column name
                        cout << "-- ";
    
                        for(int i=0; i<10; i++){ //looping for all 10 arguments
                            if(counterForAll[i]!=-1){//making sure an arguement exists
                                for(; insideCounter!=counterForAll[i];index++){// incrementing index until you find the position of the value
                                    if(tempCopier[index]=='|'){
                                        insideCounter++;
                                    }   
                                }
                                for(;!(tempCopier[index]=='|'|| index==tempCopier.length()); index++){ //displaying the headline in the consule
                                    cout << tempCopier[index];
                                }
                                if(counterForAll[i+1]!=-1){ // adding | after each headname
                                    cout << "|";
                                }
                            }
                            //resetting for looping purposes
                            index=0;
                            jindex=0;
                            insideWhereCounter=0;
                            insideCounter=0;
                        }
                        cout << endl;

                        if(whereArguments[1]=="!="){ // if the given argument says !=

                            while(getline(readTable, tempCopier)){ //reading from old file and writing to new file
                                    
                                for(; insideWhereCounter!=whereCounter;index++){ //incrementing index until counter postion is found 
                                    if(tempCopier[index]=='|'){
                                        insideWhereCounter++;
                                    }   
                                }
                               
                                for(;!(tempCopier[index]=='|'|| index==tempCopier.length()); index++){ //extracting the word till |
                                    tempWord.push_back(tempCopier[index]); //saving in tempWord
                                }
                                
                                if(tempWord != whereArguments[2]){//checking if the tempWord is not equal to the given argument
                                    cout << "-- ";

                                    for(int i=0; i< 10; i++){ // looping for all 10 attributes

                                        if(counterForAll[i]!=-1){ 
                                            for(; insideCounter!=counterForAll[i];index++){//incrementing index till the postion
                                                if(tempCopier[index]=='|'){
                                                    insideCounter++;
                                                }   
                                            }
                                            for(;!(tempCopier[index]=='|'|| index==tempCopier.length()); index++){ //printing the value on screen
                                                cout << tempCopier[index];
                                            }
                                            if(counterForAll[i+1]!=-1){
                                                cout << "|";
                                            }
                                        }
                                        //resetting for looping purposes
                                        index=0;
                                        jindex=0;
                                        insideWhereCounter=0;
                                        insideCounter=0;


                                    }
                                    cout << endl; 

                                }
                                else{// if the tempword is equal to the given argument
                                    //Do nothing
                                }                  
                                tempWord="";
                                 

                            }




                        }

                    }
                    else{
                        cout << "-- Atleast one of your select argument cannot be found in " << table_name << ".\n"; //returns if the select arguemnt is not valid.
                    }
                        


                }
                else{
                    cout << "-- Invalid input for select!\n"; //returns if the input is not valid
                    break;

                }
                


            } 







        }
        //creating a lock
        else if(inputSmallized.find("begin transaction;")!=-1||inputSmallized.find("commit;")!=-1){ //searching for begin transaction or commit
            if(inputSmallized.find("begin transaction;")!=-1){ //if the input was begin transaaction
                lockOn= true; //turning the lock on
                cout << "-- Transaction starts.\n"; //outputting
            }
            else{
                if(error22){ // if there was an error, which means somebody else was using transaction
                    cout << "-- Transaction abort.\n"; //outputs this
                    error22= false;// sets the error to false
                }
                else{// if the passed value is commit
                    string lockfile= table_name + "_lock.txt"; //creating the lock file name
                    lockOn= false; //turning the lock off
                    remove(table_name.c_str()); //deleting the old file
                    rename("output.txt",table_name.c_str()); //renaming the temp file to be that of the old file
                    cout << "-- Transaction committed.\n";//outputting
                    remove(lockfile.c_str());//removing the lock file
                }
                
            }
            
        }
        else{
            transform(input.begin(), input.end(), input.begin(), ::tolower);
            if(input==".exit"){
               //do nothing
            }else{
                 cout << "-- Invalid Query!\n";
            }
        }
    }
    cout << "-- All done.\n";// done confirmation

    return 0;
}



//checking if a file exists
inline bool fexists(const string& name) {


    ifstream f(name.c_str());
    return f.good();
}

//removing '\r'
void removeR(string& input)
{
	string withoutR;

	if (!(input.find('\r') == string::npos))
	{
        //lopping through the string to copy all char except '\r'
		for(int i = 0; i < input.size(); i++)
		{
			if (input[i] == '\r')
			{
				continue;
			}
			else
			{
				withoutR.push_back(input[i]);
			}
		}
		input = withoutR;
	}
	
    return;
}


//counting the position of a word in a header file
int positionCounter(string topArguments, string word){

    transform(word.begin(), word.end(), word.begin(), ::tolower);
    transform(topArguments.begin(), topArguments.end(), topArguments.begin(), ::tolower);

 
    string tempWord;
    int whereCounter=0;

    for(int i=0; i< topArguments.length(); i++){  // basicallly finding the counter where the argument resides
        if(topArguments[i]=='|'){
            if(tempWord== word){ //break if you found the word youre looking for
                break;
            }
            tempWord= "";//resetting the word back to nul
            whereCounter++;//incrementing the position counter

        }
        else if(topArguments[i]==' '){// if its a space
            if(tempWord== word){//checking and breaking if you have the word
                break;
            }
            tempWord= ""; //of not resetting tempWord

        }
        else if(word == "" || word == " "){//return -1 if the word is empty
            whereCounter=-1;
        }
        else{
            tempWord.push_back(topArguments[i]);//creating word
        }
    }
    if(tempWord != word){//if you looped through the entire thing but you didnt find the word, return -2
        return -2;
    }

    return whereCounter;

}

//returning a string based on an exact location 
string dataAtPosition(string tempCopier, int position){

    int counter=0;
    string tempword;
    int i=0;

    for(; counter!= position; i++){
        if(tempCopier[i]=='|'){ //going to the place where the counter is equal to the paassed postion
            counter++;
        }
    }
    while(!(tempCopier[i]=='|'|| i==tempCopier.length())){
        tempword.push_back(tempCopier[i]); //saving the string from there
        i++;
    }
    return tempword;
}

//removing any ' ' from the input table name
string removeSpace(string input){
    string temp;

    for(int i=0; i< input.length(); i++){
        if(input[i]==' '){
            //do nothing
        }else{
            temp.push_back(input[i]); 
        }
    }
    return temp;
}