#ifndef RANDOM_ACCESS_DATATYPE
#define RANDOM_ACCESS_DATATYPE

#include "pup.h"

struct raDtype {
public:
  CmiUInt8 elems[2];
  CmiUInt8 blah;
  CmiUInt8 blah2;

  void pup(PUP::er &p) {
    p | elems[0];
    p | elems[1];
    p | blah;
    p | blah2; 

  }

};
//PUPBytes(raDtype)

#endif
