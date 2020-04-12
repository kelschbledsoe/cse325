// Gabriella Kelsch Bledsoe
// Project 4 - Interpret the contents of a text file as command line arguments
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
using namespace std;

int main(int argc, char* argv[]){
  char *file;
  FILE * currentFile;
  int currentChar = 0;
  string currentLine;
  string command;
  vector<string> commandList;
  string currentCommand;
  time_t t;
  extern char **environ;
  char strC;
  bool vflag = true;
  string verboseCommand;
  vector<string> verboseList;
  int vCount = 1;
  int commandPerLineCount = 0;

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
            // Have a command
            if(command.length() != 0){
              // First command of line, add to vector
              commandList.push_back(command);
              commandPerLineCount++;
              command.clear();
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
        if(commandPerLineCount == 1){
          cout << "Only one token per line. For each invalid line, processing the first token." << endl;
        }        
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
        // Make command lowercase
        currentCommand = c;
        for(int k=0; c[k]; k++){
          currentCommand[k] = tolower(c[k]);
        }
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
        else{
          cout << "Invalid command " << c << " continuing with next valid command." << endl;
        }
      }
      // Clear vectors for next file
      commandList.clear();
      verboseList.clear();
    } // This curly brace is the end of the else block
  }// End of for loop
}
