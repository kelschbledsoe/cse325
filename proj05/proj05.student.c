// Gabriella Kelsch Bledsoe
// Project 5 - Interpret the contents of a text file as command line arguments
// CSE 325

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
using namespace std;

extern char **environ;

// TA on 2/17 said to use this function as described on a stack overflow post
void ThreadFunc(char *command){
  int ret = 0;
  if (command == NULL){
    printf("Error, input pointer argument is NULL\n");
    return;
  }
  if (command[0] == '\0'){
    printf("Error, input command string is empty\n");
    return;
  }
  ret = system(command);
  if (ret != 0){
    cout << "Error, system " << command << " failed. errno = " << errno << endl;
  }
  else{
    cout << "Success, system " << command << " succeeded\n" << endl;
  }
  pthread_exit(NULL);
}

int main(int argc, char* argv[]){
  char *file;
  FILE * currentFile;
  int currentChar = 0, ret = 0;
  string currentLine, thisLine, newDir, command, verboseCommand, currentCommand;
  vector<string> commandList, verboseList, exList;
  time_t t;
  char strC;
  bool vflag = true;
  int vCount = 1, commandPerLineCount = 0;
  bool changeDir = false;
  pthread_t mythread;

  // Loop over arguments
  for (int i = 1; i < argc; i++){
    if (argv[i][0] == '-') {
      if(strlen(argv[i]) != 2){
        cout << "Invalid flag " << argv[i] << endl;
        continue;
      }
      // Verbose you print the line number and command
      else if(argv[i][1] == 'v'){
        vflag = true;
      }
      // silent you dont print that
      else if(argv[i][1] == 's'){
        vflag = false;
      }
      // If invalid flag, error message and check next argument
      else{
        cout << "Invalid flag " << argv[i] << endl;
        continue;
      }
    }
    // File Name?
    else{
      // Check it's an existing file
      file = argv[i];
      currentFile = fopen(file,"r");
      if(currentFile == NULL){
        cout << "File " << file << " does not exist" << endl;
        continue;
      }
      currentChar = getc(currentFile);
      // Read this file
      while(currentChar != EOF){
        // Handle empty lines
        while(currentChar == '\n'){
          if(currentLine.length() == 0)
          {
            verboseList.push_back("empty");
          }
          currentChar = getc(currentFile);
        }
        // Now get a line in the file
        while((char)currentChar != '\n'){
          currentLine += (char)currentChar;
          currentChar = getc(currentFile);
        }  
        commandPerLineCount = 0;
        // Grab arguments from line
        for(auto j : currentLine){
          // Verbose mode wants to print the white space
          verboseCommand += j;
          if(commandPerLineCount == 1){
            continue;
          }
          // Commands should not have the whitespace
          if(j == ' ' || j == '\t'){
            // Known directive that does not take other tokens 
            if((command.length() != 0) && 
                ((command[0]=='e' && command[1]=='x' && command[2]=='i' && command[3]=='t')||
                 (command[0] == 'd' && command[1]=='a' && command[2] == 't' && command[3] == 'e')||
                 (command[0] == 'e' && command[1] == 'n' && command[2] == 'v')||
                 (command[0] == 'p' && command[1] == 'a' && command[2] == 't' && command[3] == 'h')||
                 (command[0] == 'c' && command[1] == 'w' && command[2] == 'd'))){
              // First command of line, add to vector
              commandList.push_back(command);
              commandPerLineCount++;
              command.clear();
              continue;
            }
            else if(command.length() == 0){
              continue;
            }
            // cd, set, and external commands all take multiple tokens, just
            // take the whole line and worry about number of tokens in later
            // processing
            else{
              command += j;
              continue;
            }
          }
          else{
            command += j;
          }
        }
        if(commandPerLineCount == 1){
          verboseList.push_back(verboseCommand);
          verboseCommand.clear();
        }
        else if(commandPerLineCount == 0){
          commandList.push_back(command);
          verboseList.push_back(verboseCommand);
        }
        command.clear();
        currentLine.clear();
        verboseCommand.clear();
        currentChar = getc(currentFile);
      } // This curly brace is the end of the while not EOF block
      fclose(currentFile);
      // Count needed to index verbose list
      vCount = 1;        
      for (auto c : commandList){
        if(vflag){
          // Handle printing empty lines without throwing off commandList loop
          while(verboseList[vCount-1]=="empty"){
            cout << '<' << vCount << '>' << endl;
            vCount++;
          }
          cout << '<' << vCount << '>' << verboseList[vCount-1] << endl;
          vCount++;
        }
        currentCommand = c;
        // exit means stop processing this file
        if(currentCommand=="exit"){
          break;
        }
        // display date
        else if(currentCommand == "date"){
          time(&t);
          cout << ctime(&t);
        }
        // display environment like setenv
        else if(currentCommand == "env"){
          int i = 0;
          while (environ[i] != NULL){
            printf("%s\n", environ[i]);
            i++;
          }
        }
        // display pathname of current working directory
        else if(currentCommand == "cwd"){
          char cDir[PATH_MAX];
          getcwd(cDir, sizeof(cDir));
          cout << cDir << endl;
        }
        // display current search path in readable format
        else if(currentCommand == "path"){
          char *currentPath = getenv("PATH");
          for(unsigned int c = 0; c < strlen(currentPath); c++){
            strC = currentPath[c];
            // Instead of printing :, printing a new line
            if(strC != ':'){
              cout << strC;
            }
            else{
              printf("\n");
            }
          }
          printf("\n");
        }
        else if(currentCommand[0] == 'c' && currentCommand[1] == 'd'){
          // just cd, go to user home directory
          if (currentCommand.length() == 2){
            chdir(getenv("HOME"));
            changeDir = true;
            cout << "Current directory is now your home directory."<< endl;
          }
          // cd ~USER
          else if (currentCommand[3] == '~'){
            newDir = "/user/";
            for (unsigned int s = 4; s < currentCommand.length(); s++){
              if (currentCommand[s] == ' '){
                cout << "cd only accepts one argument, processing that one." << endl;
                break;
              }
              newDir += currentCommand[s];
            }
            // special case of cd ~
            if (newDir == "/user/"){
              chdir(getenv("HOME"));
              changeDir = true;
              cout << "Current directory is now your home directory."<< endl;
            }
            // normal cd ~USER
            else if(chdir(newDir.c_str())==0){
              changeDir = true;
              cout << "Directory successfully changed to " << newDir << endl;
            }
            else{
              cout << "Invalid directory " << newDir << endl;
            }
            newDir = "";
          }
          // cd DIR
          else{
            // This is a relative pathname
            if(currentCommand[3] != '/'){
              newDir += getenv("PWD");
              newDir += '/';
            }
            for (unsigned int s = 3; s < currentCommand.length(); s++){
              if (currentCommand[s] == ' '){
                cout << "cd only accepts one argument, processing that one." << endl;
                break;
              }
              newDir += currentCommand[s];
            }
            if (chdir(newDir.c_str())==0){
              changeDir = true;
              cout << "Directory successfully changed to " << newDir << endl;
            }
            else{
              cout << "Invalid directory " << newDir << endl;
            }
            newDir = "";
          }
          // If changed directory, need to update PWD
          if (changeDir){
            char cDir[PATH_MAX];
            getcwd(cDir, sizeof(cDir));
            setenv("PWD",cDir,1);
            changeDir = false;
          }  
        }
        // SET
        else if(currentCommand[0] == 's' && currentCommand[1] == 'e' && currentCommand[2] == 't'){
          string var = "", value = "";
          int space = 0;
          for (unsigned int c = 4; c < currentCommand.length(); c++){
            if (currentCommand[c] == ' '){
              space++;
              continue;
            }
            if (!space){
              var += currentCommand[c];
            }
            else if (space==1){
              value += currentCommand[c];
            }
            else{
              cout << "Set only takes two arguments. Processing those two." << endl;
              break;
            }
          }
          if (var == ""){
            cout << "No environment variable provided." << endl;
          }
          // Remove environment variable VAR
          else if (value == ""){
            if(unsetenv(var.c_str()) == 0){
              cout << "Removed environment variable " << var << endl;
            }
            else{
              cout << "Cannot remove environment variable " << var << endl;
            }
          }
          else{
            if(setenv(var.c_str(), value.c_str(), 1) == 0){
              cout << "Set environment variable " << var << " to " << value << endl;
            }
            else{
              cout << "Cannot set environment variable " << var << " to " << value << endl;
            }
          }
        }
        else{
          ret =
            pthread_create(&mythread,NULL,(void*(*)(void*))ThreadFunc,const_cast<char*>(currentCommand.c_str()));
          if (ret != 0){
            cout << "pthread_create() failed for command " << currentCommand << " errno = " << errno << endl;
          }
          else{
            cout << "pthread_create() successful for command " << currentCommand << endl;
          }
          if(pthread_join(mythread,NULL)){
            printf("Error joining thread\n");
          }
        }
      }
      // Clear vectors for next file
      commandList.clear();
      verboseList.clear();
    } // This curly brace is the end of the else block
  }// End of for loop
}
