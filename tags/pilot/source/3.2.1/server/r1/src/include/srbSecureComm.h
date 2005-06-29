/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


extern int   sscEstablishRsaKeypair(int opt);
extern char *sscGetPublicKeyString();
extern int   sscInitPublicKey(char *keyString);
extern char *sscSetupSession();
extern int   sscEncrypt(char *inBuf, int len);
extern char *sscEncryptString(char *inBuf);
extern int   sscDecrypt(char *inBuf, int len);
extern char *sscDecryptString(char *inBuf);
