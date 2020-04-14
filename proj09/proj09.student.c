// Gabriella Kelsch Bledsoe
// Project 9 - Primary Storage with Paging Part 1
// CSE 325

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <iomanip>
using namespace std;

struct PageLine{
  bool V = 0, P = 0, R = 0, M = 0;
  unsigned int index, frameNumber = 0;
  
  // Print the page line
  void printLine(){
    // Items within a line will be separated by exactly one space
    // Each line will terminate with a new line
    cout << "[" << hex << index << "]: " << V << " " << P << " " << R << " " <<
      M << " " << hex << setfill('0') << setw(2) << frameNumber << endl;
  }
};

struct MemoryReference{
  // number is page number
  // offset is page offset
  unsigned int address, number, offset;
  string operation;
};

vector<MemoryReference> references;

// Name: printPageTable
// Purpose: print the page table
// Receive: line in table
// Return: None
void printPageTable(PageLine * line){
  cout << endl;
  cout << "Page Table contents: " << endl;
  cout << endl;
  cout << "     V P R M Frame" << endl;
  cout << "     ------- -----" << endl;
  for(int i = 0; i < 16; i++){
    line[i].printLine();
  }
  cout << endl;
}

int main(int argc, char* argv[]){
  char *file;
  FILE * currentFile;
  bool debug = false, space = false, gotRefs = false;
  int currentChar = 0, numR = 0, numW = 0;
  string address, num, offset, operation;
  unsigned int a, n, o;

  PageLine PageTable[16];
  for(int i = 0; i < 16; i++){
    PageTable[i].index = i;
  }

  for(int i = 1; i < argc; i++){
    if(argv[i][0] == '-'){
      if(strlen(argv[i]) != 5 && strlen(argv[i]) != 6){
        cout << "Invalid flag " << argv[i] << endl;
      }
      else if(strcmp(argv[i], "-debug")==0) {
        debug = true;
      }
      else if(strcmp(argv[i],"-refs")==0){
        if(gotRefs == true){
          cout << "Only specify refs once!" << endl;
          i++;
          continue;
        }
        gotRefs=true;
        if(i==argc-1){
          cout << "No file specified!" << endl;
          continue;
        }
        i++;
        file=argv[i];
        currentFile = fopen(file,"r");
        if(currentFile == NULL){
          cout << "File " << file << " does not exist." << endl;
          continue;
        }
        currentChar = getc(currentFile);
        while(currentChar != EOF){
          while((char)currentChar != '\n'){
            if((char)currentChar == ' '){
              space = true;
            }
            else if (!space){
              address += (char)currentChar;
            }
            else{
              operation = (char)currentChar;
            }
            currentChar = getc(currentFile);
          }
          // Process lines to create struct
          a = stoul(address,nullptr,16);
          n = (a>>12)&0xF;
          o = a&0xFFF;
          struct MemoryReference ref = {a,n,o,operation};
          references.push_back(ref);
          address.clear();
          operation.clear();
          space=false;
          currentChar = getc(currentFile);
        }
        fclose(currentFile);
      }
      else{
        cout << "Invalid flag " << argv[i] << endl;
      }
    }
    else{
      cout << "Invalid flag " << argv[i] << endl;
    }
  } // end of argument for loop
  if(!gotRefs){
    cout << "No file specified." << endl;
  }

  // Loop over references
  for(auto r: references){
    if(debug){
      printPageTable(PageTable);
    }
    if(r.operation == "R"){
      numR++;
    }
    else if(r.operation == "W"){
      numW++;
    }
    cout << hex << setfill('0') << setw(4) << r.address << " " << r.operation
      << " " << hex << r.number << " " << hex << setfill('0') << setw(3) <<
      r.offset << endl;
  }
  printPageTable(PageTable);
  cout << "Total number of memory references: " << references.size() << endl;
  cout << "Total number of read operations:   " << numR << endl;
  cout << "Total number of write operations:  " << numW << endl;
} // end of main
