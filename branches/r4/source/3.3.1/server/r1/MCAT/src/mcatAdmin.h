/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* mcatAdmin.h - Definition and prototypes for mcatAdmin.c 
 */

#ifndef MCAT_ADMIN_H
#define MCAT_ADMIN_H

#define STR_LEN		256

/* The attribute input struct */

typedef struct attriInput {
    char header[STR_LEN];	/* The header description for the input */
    char inpStr[STR_LEN];	/* The input string */
};
    
/* The user input */

#define N_USER_ATTRI	7	/* number of user attribute */
#define USER_NAME	0	/* The user name */
#define USER_PASSWD	1	/* The user passwd */
#define USER_DOMAIN	2	/* The user domain */
#define USER_TYPE	3	/* The user type */
#endif	/* MCAT_ADMIN_H */
