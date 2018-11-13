#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include "hw.h"


// This is the main driver file for the bank simulation.
// The `main` function takes one argument, the name of a
// trace file to use.  The test directory contains a
// short sample trace that exercises at least one interesting
// case.

int main (int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: %s trace_file\n", argv[0]);
    exit(1);
  }

  int result        = 0;
  int atm_count     = 0;
  int account_count = 0;

  // open the trace file
  result = trace_open(argv[1]);
  if (result == -1) {
    printf("%s: could not open file %s\n", argv[0], argv[1]);
    exit (1);
  }

  // Get the number of ATMs and accounts:
  atm_count     = trace_atm_count();
  account_count = trace_account_count();
  trace_close();

  // This is a table of atm_out file descriptors. It will be used by
  // the bank process to communicate to each of the ATM processes.
  int atm_out_fd[atm_count];

  // This is a table of bank_in file descriptors. It will be used by
  // the bank process to receive communication from each of the ATM processes.
  int bank_in_fd[atm_count];

  // TODO: ATM PROCESS FORKING
  // START YOUR IMPLEMENTATION
  for (int i=0; i< atm_count; i++){
    int to_atmfd[2];
    pipe(to_atmfd);
    atm_out_fd[i]=to_atmfd[1];

    int to_bankfd[2];
    pipe(to_bankfd);
    bank_in_fd[i]=to_bankfd[0];

    if (fork()==0){
      close(to_atmfd[1]);
      close (to_bankfd[0]);

      if(atm_run(argv[1],to_bankfd[1],to_atmfd[0],i ) != SUCCESS){
        error_print();
      }
      exit(0);
    }
      else {
        close(to_atmfd[0]);
        close (to_bankfd[1]);
      }
      
  }

  // END YOUR IMPLEMENTATION


  // TODO: BANK PROCESS FORKING
  // START YOUR IMPLEMENTATION
  if (fork()==0);{
  bank_open(atm_count, account_count);
  int result =run_bank(bank_in_fd,atm_out_fd);
  if (result!=SUCCESS){
    error_print();
  }
  bank_dump();
  bank_close();
  exit(0);

}
  // END YOUR IMPLEMENTATION

  // Wait for each of the child processes to complete. We include
  // atm_count to include the bank process (i.e., this is not a
  // fence post error!)
  for (int i = 0; i <= atm_count; i++) {
    wait(NULL);
  }

  return 0;
}
