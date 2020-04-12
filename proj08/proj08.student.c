// Gabriella Kelsch Bledsoe
// Project 8 - Cache Simulation Part 2
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
  unsigned int data[16];
  unsigned int tag = 0;
  unsigned int index;
  
  // Print the cache line
  void printLine(){
    // Items within a line will be separated by exactly one space
    cout << "[" << hex << index << "]: " << V << " " << M << " " << hex << setfill('0') << setw (4) << tag << " ";
    for(int i  = 0; i < 16; i++){
      cout << hex << setfill('0') << setw(2) << data[i];
      cout << " ";
    }
    cout << endl;
    // Each line will terminate with a newline
  }
  // constructor. Initialize everything to zero
  CacheLine(){
    int i = 0;
    while(i < 16){
      data[i] = 0;
      i++;
    }
  }

};

struct MemoryReference{
  unsigned int address, tag, index, offset;
  string operation;
  unsigned int value[4];
  string result;
};

struct RamLine{
  unsigned int address;
  unsigned int data[16];

  // Print the cache line
  void printLine(){
    // Items within a line will be separated by exactly one space
    cout << hex << setfill('0') << setw (5) << address << " ";
    for(int i  = 0; i < 16; i++){
      cout << hex << setfill('0') << setw(2) << data[i];
      cout << " ";
    }
    // Each line will terminate with a newline
  }
  // constructor. Initialize everything to zero
  RamLine(){
    int i = 0;
    while(i < 16){
      data[i] = 0;
      i++;
    }
  }
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
  cout << "     V M Tag   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F" << endl;
  // Need appropriate column headers
  for(int i = 0; i < 8; i++){
    line[i].printLine();
  }
  cout << endl;
  // Data cache display will begin and end with a blank line
}

// Name: printRam
// Purpose: Print the RAM contents
// Receive: RAM
// Return: None
void printRam(RamLine * line){
  cout << endl;
  cout << "RAM Contents: " << endl;
  cout << endl;
//  for(int i = 8192; i < 65536; i++){

  for(int i = 8192; i < 8200; i++){
    line[i].printLine();
    cout << endl;
  }
  cout << endl;
}

int main(int argc, char* argv[]){
  char *file;
  FILE * currentFile;
  bool debug = false, space=false, gotRefs = false;
  int currentChar = 0, valCount = 0;
  string address, operation, valZero, valOne, valTwo, valThree;
  unsigned int a, t, ind, o;
  CacheLine CACHE[8];
  
  RamLine RAM[65536];
  
  unsigned int ramAddress = 0;
  for(unsigned int i = 0; i < 65536; i++){
    RAM[i].address = ramAddress;
    ramAddress += 16;
  }

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
              if((char)currentChar == ' '){
                if(valCount==0){
                  currentChar = getc(currentFile);
                  valZero += (char)currentChar;
                  currentChar = getc(currentFile);
                  valZero += (char)currentChar;
                  valCount++;
                }
                else if(valCount==1){
                  currentChar = getc(currentFile);
                  valOne += (char)currentChar;
                  currentChar = getc(currentFile);
                  valOne += (char)currentChar;
                  valCount++;
                }
                else if(valCount==2){
                  currentChar = getc(currentFile);
                  valTwo += (char)currentChar;
                  currentChar = getc(currentFile);
                  valTwo += (char)currentChar;
                  valCount++;
                }
                else if(valCount==3){
                  currentChar = getc(currentFile);
                  valThree += (char)currentChar;
                  currentChar = getc(currentFile);
                  valThree += (char)currentChar;
                }
              }
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
          if(operation=="W"){
            ref.value[0] = stoul(valZero, nullptr, 16);
            ref.value[1] = stoul(valOne, nullptr, 16);
            ref.value[2] = stoul(valTwo, nullptr, 16);
            ref.value[3] = stoul(valThree, nullptr, 16);
          }
          references.push_back(ref);
          address.clear();
          operation.clear();
          space = false;
          valCount = 0;
          valZero.clear();
          valOne.clear();
          valTwo.clear();
          valThree.clear();
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
  unsigned int tempData[16];
  string hitmiss;
  unsigned int ramIndex = 0; 
  bool td = false;
  // Loop over memory references
  for(auto r : references){
    if(debug == true){
      printCache(CACHE);
    }
    hitmiss = " H ";
    td = false;
    if(!(CACHE[r.index].V==1 && CACHE[r.index].tag==r.tag)){
      hitmiss = " M ";
      if(CACHE[r.index].V==1 && CACHE[r.index].M == 1){
        // Write from cache to RAM
        td = true;
        int i = 0;
        ramIndex = ((CACHE[r.index].tag<<7)|r.index<<4)/16;
        
        while(i<16){
          tempData[i] = RAM[ramIndex].data[i];
          i++;
        }
        i = 0;
        while(i < 16){
          RAM[ramIndex].data[i] = CACHE[r.index].data[i];
          i++;
        }
      }
      // Copy block from RAM to cache
      int i = 0;
      ramIndex = ((r.tag<<7)|r.index<<4)/16;
      while(i < 16){
        if(td){
          CACHE[r.index].data[i] = tempData[i];
        }
        else{
          CACHE[r.index].data[i] = RAM[ramIndex].data[i];
        }
        i++;
      }
      CACHE[r.index].M = 0;
      CACHE[r.index].tag = r.tag;
      CACHE[r.index].V = 1;
    }
    if(r.operation=="R"){
      // Copy from cache to CPU - not actually modeling CPU so outputting instead
      r.value[0] = CACHE[r.index].data[r.offset];
      r.value[1] = CACHE[r.index].data[r.offset+1];
      r.value[2] = CACHE[r.index].data[r.offset+2];
      r.value[3] = CACHE[r.index].data[r.offset+3];
    }
    else if (r.operation=="W"){
      // Copy from CPU to cache - not actually modeling CPU so outputting instead
      CACHE[r.index].data[r.offset] = r.value[0];
      CACHE[r.index].data[r.offset+1] = r.value[1];
      CACHE[r.index].data[r.offset+2] = r.value[2];
      CACHE[r.index].data[r.offset+3] = r.value[3];
      CACHE[r.index].M = 1;
    }

    // display memory reference
    cout << hex << setfill('0') << setw(5) << r.address << " " << r.operation
        << " " << hex << setfill('0') << setw(4) << r.tag << " " << r.index << " " <<
        r.offset << hitmiss << hex << setfill('0') << setw(2) << r.value[0] << " "
        << hex << setfill('0') << setw(2) << r.value[1] << " " << hex <<
        setfill('0') << setw(2) << r.value[2] << " " << hex << setfill('0') <<
        setw(2) << r.value[3] << endl;
  }  // display contents of data cache at the end no matter what
  printCache(CACHE);
  printRam(RAM);
} // End of main
