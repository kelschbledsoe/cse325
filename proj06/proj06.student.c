// Gabriella Kelsch Bledsoe
// Project 6 - producer/consumer problem that uses multi-threading to stimulate a banking system
// CSE 325

#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <string>
#include <typeinfo>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <vector>

using namespace std;

struct Account{
  int number;
  double balance;
};
// initialized like
// struct Account a1 = {1111, 1278};

struct Transaction{
  int number, type;
  // 0 for deposit. 1 for withdraw. 2 is the special one used to signal.
  double amount;
  long tid;
};

Transaction buff[20];
vector<Account> accounts;
int in = 0, out = 0, numB = 5, numP = 1, numProd = 0, threadNum = 0;
sem_t S, N, E;

// Name: modifyAccount
// Purpose: Helper function used to modify global vector
// Receive: Transaction information
// Return: None. Modifies global vector
void modifyAccount(int type, int index, double amount){
  if(type == 0){
    accounts[index].balance += amount;
  }
  else{
    accounts[index].balance -= amount;
  }
}


void *producer(void *arg){
  string filename = "trans";
  int argu = *((int *) arg);
  filename += to_string(argu);
  FILE * tfile = fopen(filename.c_str(), "r");
  int thisChar, space = 0;
  string numb, typ, amoun;
  char *p;

  // Read the entire file
  thisChar = getc(tfile);
  while(thisChar != EOF){
    while((char)thisChar != '\n'){
      if((char)thisChar == ' '){
        space++;
      }
      else if(space == 0){
        numb+= (char)thisChar;
      }
      else if(space == 1){
        typ += (char)thisChar;
      }
      else{
        amoun += (char)thisChar;
      }
      thisChar = getc(tfile);
    }
    struct Transaction t;
    t.number = strtol(numb.c_str(), &p, 10);
    if(strcmp(typ.c_str(),"deposit")==0){
      t.type = 0;
    }
    else{
      t.type = 1;
    }
    t.amount = strtod(amoun.c_str(),&p);
    t.tid = (long)pthread_self();
    // Add to the vector inside the critical section
    sem_wait(&E);
    sem_wait(&S);
    buff[in] = t;
    in = (in+1)%numB;
    sem_post(&S);
    sem_post(&N);
    numb.clear();
    typ.clear();
    amoun.clear();
    space = 0;
    thisChar = getc(tfile);
  }
  // Special transaction so know done
  sem_wait(&E);
  sem_wait(&S);
  struct Transaction t = {0,0,0,0};
  buff[in] = t;
  in = (in+1)%numB;
  sem_post(&S);
  sem_post(&N);
  pthread_exit(NULL);
}


void *consumer(void *arg){
  // Loop until no more transactions
  int num, typ;
  double amoun, camount, namount;
  FILE * lfile;
  long thid;
  string ttype;
  lfile = fopen("accounts.log","w+");
  while(true){
    sem_wait(&N);
    sem_wait(&S);
    struct Transaction t = buff[out];
    out = (out+1)%numB;
    sem_post(&S);
    sem_post(&E);
    num = t.number;
    typ = t.type;
    amoun = t.amount;
    thid = t.tid;
    // Process special transaction
    if(typ == 0 && num == 0 && amoun == 0 && thid == 0){
      numProd++;
      // Stop once processed all producers
      if(numProd == numP){
        break;
      }
      continue;
    }
    // Execute transaction
    for(unsigned int i = 0; i < accounts.size(); i++){
      if (accounts[i].number == num){
        camount = accounts[i].balance;
        modifyAccount(typ, i, amoun);  
        namount = camount + amoun;
      }
    }
    // write to accounts.log
    // Thread number where transaction originated
    // Account number
    // Transaction type
    // Current account balance
    // Transaction amount
    // Updated account balance
    if(typ == 0){
      ttype = "Deposit";
    }
    else{
      ttype = "Withdraw";
    }
    fprintf(lfile, "%ld %i %-9s $%-7.2f $%-7.2f $%-7.2f\n", thid, num, ttype.c_str(), camount, amoun, namount); 
  }
  pthread_exit(NULL);
}

int main(int argc, char* argv[]){
  int pflag = 0, bflag = 0, currentChar = 0, aiNum;
  double adBal;
  bool gotP = false, gotB = false, space = false;
  char *p;
  long conv;
  FILE * file, *ofile;
  string aNum, aBal;

  // Process command line
  for (int i = 1; i < argc; i++){
    if (argv[i][0] == '-'){
      if(strlen(argv[i]) != 2){
        cout << "Invalid flag " << argv[i] << endl;
      }
      else if(argv[i][1] == 'p'){
        pflag++;
        if(pflag > 1){
          cout << "Multiple p flags specified. Using last producer number." << endl;
          continue;
        }
        errno = 0;
        if (i+1 == argc){
          continue;
        }
        conv = strtol(argv[i+1], &p, 10);
        if (errno != 0 || *p != '\0' || conv > INT_MAX){
          cout << "Invalid input. p flag detected but not followed by integer. Using default." << endl;
        }
        else{
          if (conv < 11 && conv > 0){
            numP = conv;
            gotP = true;
          }
          else{
            cout << "Invalid producer number " << conv << endl;
            continue;
          }
        }
      }
      else if(argv[i][1] == 'b'){
        bflag++;
        if(bflag > 1){
          cout << "Multiple b flags specified. Using last producer number." << endl;
          continue;
        }
        errno = 0;
        if (i+1 == argc){
          continue;
        }
        conv = strtol(argv[i+1], &p, 10);
        if (errno != 0 || *p != '\0' || conv > INT_MAX){
          cout << "Invalid input. p flag detected but not followed by integer. Using default." << endl;
        }
        else{
          if (conv < 21 && conv > 0){
            numB = conv;
            gotB = true;
          }
          else{
            cout << "Invalid record number " << conv << endl;
            continue;
          }
        }  
      }
      else{
        cout << "Invalid flag " << argv[i] << endl;
      }
    }
    // Skipping numbers since process when first find flag
    else if((pflag > 0 && gotP == true) || (bflag > 0 && gotB == true)){
      continue;
    }    
  
  } // end of for loop
  // opening accounts.old
  file = fopen("accounts.old","r");
  if(file == NULL){
    cout << "File accounts.old does not exist." << endl;
    exit(0);
  }
  currentChar = getc(file);
  while(currentChar != EOF){
    // Read line in file
    while((char)currentChar != '\n'){
      if ((char)currentChar == ' '){
        space = true;
      }
      else if (!space){
        aNum += (char)currentChar;
      }
      else{
        aBal += (char)currentChar;
      }
      currentChar = getc(file);
    }
    // Make the account and prep for next line in file
    aiNum = strtol(aNum.c_str(), &p,10);
    adBal = strtod(aBal.c_str(), &p);
    struct Account a1 = {aiNum, adBal};
    accounts.push_back(a1);
    aNum.clear();
    aBal.clear();
    space = false;
    currentChar = getc(file);
  }
  fclose(file);

  // Initialize semaphores
  sem_init(&S, 0, 1);
  sem_init(&N, 0, 0);
  sem_init(&E, 0, numB);

  pthread_t pThreads[numP];
  pthread_t cThreads[1];

  int returnCode;
  // create threads
  for(int i = 0; i < numP; i++){
    int *ar = (int *)malloc(sizeof(*ar));
    *ar = threadNum;
    returnCode = pthread_create(&pThreads[i], NULL, producer, ar);
    threadNum++;
    if(returnCode){
      cout << "Error, return code from pthread_create() is " << returnCode << endl;
      exit(0);
    }
  }
  returnCode = pthread_create(&cThreads[0], NULL, consumer, NULL);
  if(returnCode){
    cout << "Error, return code from pthread_create() is " << returnCode << endl;
    exit(0);
  }
  // Join threads
  for(int i = 0; i < numP; i++){
    if(pthread_join(pThreads[i], NULL)){
      cout << "Error joining thread." << endl;
      exit(0);
    }
  }
  if(pthread_join(cThreads[0],NULL)){
    cout << "Error joining thread." << endl;
    exit(0);
  }
  // Destroy semaphores
  sem_destroy(&S);
  sem_destroy(&N);
  sem_destroy(&E);

  // Write to accounts.new
  ofile = fopen("accounts.new", "w");
  if (ofile == NULL){
    cout << "Could not write to accounts.new." << endl;
    exit(0);
  }
  for(auto a : accounts){
    fprintf(ofile, "%i %-7.2f \n", a.number, a.balance); 
  }
  fclose(ofile);
}

