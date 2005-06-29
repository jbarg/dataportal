/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* crypt1.c */

#include "auth.h"
#include <math.h>

struct stRandomStruct {
  unsigned long ulSeed, ulSeed2, ulMaxValue;
  double ulMaxValue_dbl;
};

double dRandonmValue(struct stRandomStruct *rand)
{
  rand->ulSeed=(rand->ulSeed*3+rand->ulSeed2) % rand->ulMaxValue;
  rand->ulSeed2=(rand->ulSeed+rand->ulSeed2+33) % rand->ulMaxValue;
  return (((double) rand->ulSeed)/rand->ulMaxValue_dbl);
}

void vInitRandomSeeds(struct stRandomStruct *rand, ulong ulSeed, ulong ulSeed2)
{
  rand->ulMaxValue= 0x3FFFFFFFL;
  rand->ulMaxValue_dbl=(double) rand->ulMaxValue;
  rand->ulSeed=ulSeed%rand->ulMaxValue ;
  rand->ulSeed2=ulSeed2%rand->ulMaxValue;
}

static void vHashString(ulong *ulResult, const char *cpPassword)
{
  register ulong nr=1345345333L, add=7, nr2=0x12345671L;
  ulong tmp;
  for (; *cpPassword ; cpPassword++)
    {
      tmp= (ulong) (unsigned char) *cpPassword;
      nr^= (((nr & 63)+add)*tmp)+ (nr << 8);
      nr2+=(nr2 << 8) ^ nr;
      add+=tmp;
    }
  ulResult[0]=nr & (((ulong) 1L << 31) -1L); /* Don't use sign bit (str2int) */;
  ulResult[1]=nr2 & (((ulong) 1L << 31) -1L);
  return;
}

void vCrypt1(char *cpEncryptedMsg, const char *cpClearMsg, const char *cpPassword)
{
  struct stRandomStruct rand;
  ulong hash_pass[2],hash_cpClearMsg[2];
  if (cpPassword && cpPassword[0])
    {
      vHashString(hash_pass,cpPassword);
      vHashString(hash_cpClearMsg,cpClearMsg);
      vInitRandomSeeds(&rand,hash_pass[0] ^ hash_cpClearMsg[0],
		       hash_pass[1] ^ hash_cpClearMsg[1]);
      while (*cpClearMsg++)
	*cpEncryptedMsg++= (char) (floor(dRandonmValue(&rand)*31)+64);
    }
  *cpEncryptedMsg=0;
}

