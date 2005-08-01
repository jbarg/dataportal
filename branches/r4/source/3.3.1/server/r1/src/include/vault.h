/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* vault.h - Header file for vault configuration.
 */

#ifndef VAULT_H
#define VAULT_H

/* The following are for the storage vault configuration */
 
#define VAULT_CONFIG    "vaultConfig"   /* The vault config file */
#define MAX_LINE_SZ     512
#define VAULT_KEY       "storSysType"  /* The vault key word */
#define RS_BEGIN        0
#define RS_GOT_KEYWD    1       /* read in the keyword */
#define RS_GOT_TYPE       2       /* read in the access method */
 
struct vaultQueElement {
    int storSysType;
    char *vaultPath;
    struct vaultQueElement *next;
};
 
#endif	/* VAULT_H */
