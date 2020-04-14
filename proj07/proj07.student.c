// Gabriella Kelsch Bledsoe
// Project 7 - Cache Simulation Part 1
// CSE 325

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <iomanip>
using namespace std;

struct CacheLine{
  bool V = 0;
  bool M = 0;
  char data[32];
  int tag = 0;
  int index;
  
  // Print the data block of the cache
  string printData(){
    string output;
    for(int i = 0; i < 31; i+=2){
      output += data[i];
      output += data[i+1];
      output += " ";
    }
    return output;
  }

  // Print the cache line
  void printLine(){
    // Items within a line will be separated by exactly one space
    cout << "[" << hex << index << "]: " << V << " " << M << " ";
    cout << setfill ('0') << setw (4);
    cout << tag << " " << printData() << endl;
    // Each line will terminate with a newline
  }
  // constructor. Initialize everything to zero
  CacheLine(){
    int i = 0;
    while(i < 32){
      data[i] = '0';
      i++;
    }
  }

};

struct MemoryReference{
  unsigned int address, tag, index, offset;
  string operation;
};

vector<MemoryReference> references;

// Name: printCache
// Purpose: Print the data cache
// Receive: Data cache
// Return: None
void printCache(CacheLine * line){
  cout << endl;
  cout << "Data Cache Contents: " << endl;
  cout << endl;
  cout << "     V M Tag  Block Contents" << endl;
  // Need appropriate column headers
  for(int i = 0; i < 8; i++){
    line[i].printLine();
  }
  cout << endl;
  // Data cache display will begin and end with a blank line
}


int main(int argc, char* argv[]){
  char *file;
  FILE * currentFile;
  bool debug = false, space = false, gotRefs = false;
  int currentChar = 0;
  string address, operation;
  unsigned int a, t, ind, o;
  CacheLine CACHE[8];

  for(int i = 0; i < 8; i++){
    CACHE[i].index = i;
  }

  for (int i = 1; i < argc; i++){
    if(argv[i][0] == '-'){
      if(strlen(argv[i]) != 5 && strlen(argv[i]) != 6){
        cout << "Invalid flag " << argv[i] << endl;
      }
      else if(strcmp(argv[i], "-debug")==0){
        debug = true;
        // Double check don't need to reset this
      }
      else if(strcmp(argv[i],"-refs")==0){
        if(gotRefs == true){
          cout << "Only specify refs once!" << endl;
          i++;
          continue;
        }
        gotRefs = true;
        if(i == argc-1){
          cout << "No file specified!" << endl;
          continue;
        }
        i++;
        file = argv[i];
        currentFile = fopen(file,"r");
        if(currentFile == NULL){
          cout << "File " << file << " does not exist" << endl;
          continue;
        }
        currentChar = getc(currentFile);
        while(currentChar != EOF){
          while((char)currentChar != '\n'){
            if(operation == "W"){
              currentChar = getc(currentFile);
              // Currently don't care about what value to write, so skipping the
              // rest of the line. 
            }
            else if((char)currentChar == ' '){
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
          // convert address to unsigned int
          a = stoul(address,nullptr,16);
          // Shift number of bits to left where want to start grabbbing
          // & with number of bits want, count 1s in binary rep of HEX
          t = (a>>7)&0x1FFF;
          o = (a&0xF);
          ind = (a>>4)&0x7;
          // create struct and push back into vector
          struct MemoryReference ref = {a, t, ind, o, operation};
          references.push_back(ref);
          address.clear();
          operation.clear();
          space = false;
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
  } // End of for loop
  if(!gotRefs){
    cout << "No file specified." << endl;
  }
  // Loop over memory references
  for(auto r : references){
    if(debug == true){
      printCache(CACHE);
    }
    // display at start and after each memory reference
    printf("%05x %s %04x %01x %01x\n",r.address,r.operation.c_str(),r.tag,r.index,r.offset);
  }
  // display contents of data cache at the end no matter what
  printCache(CACHE);
} // End of main
