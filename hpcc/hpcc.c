/* -*- mode: C; tab-width: 2; indent-tabs-mode: nil; fill-column: 79; coding: iso-latin-1-unix -*- */
/*
  hpcc.c
*/

#include <hpcc.h>
#include <ctype.h>

int
main(int argc, char *argv[]) {
  int myRank, commSize;
  char *outFname;
  FILE *outputFile;
  HPCC_Params params;
  time_t currentTime;

  MPI_Init( &argc, &argv );

  if (HPCC_Init( &params ))
    goto hpcc_end;

  MPI_Comm_size( MPI_COMM_WORLD, &commSize );
  MPI_Comm_rank( MPI_COMM_WORLD, &myRank );

  outFname = params.outFname;

  /* -------------------------------------------------- */
  /*                 MPI RandomAccess                   */
  /* -------------------------------------------------- */

  MPI_Barrier( MPI_COMM_WORLD );

  BEGIN_IO( myRank, outFname, outputFile);
  fprintf( outputFile, "Begin of MPIRandomAccess section.\n" );
  END_IO( myRank, outputFile );

  if (params.RunMPIRandomAccess) HPCC_MPIRandomAccess( &params );

  time( &currentTime );
  BEGIN_IO(myRank, outFname, outputFile);
  fprintf( outputFile,"Current time (%ld) is %s\n",(long)currentTime,ctime(&currentTime));
  fprintf( outputFile, "End of MPIRandomAccess section.\n" );
  END_IO( myRank, outputFile );

  hpcc_end:

  HPCC_Finalize( &params );

  MPI_Finalize();
  return 0;
}
