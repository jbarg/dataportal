/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* obfExtern.h - The include file required for the obf library routines
 */

#ifndef __OBF_H__
#define __OBF_H__

#ifdef c_plus_plus
extern "C" {
#endif

extern int obfSavePw(int promptOpt, int fileOpt, int printOpt, char *pw);
extern int obfGetPw(char *pw);
extern int obfSetDebug(int opt);

#ifdef c_plus_plus
};
#endif

#endif /* __OBF_H__ */
