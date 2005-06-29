/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbDirent.h - The standard dirent struct used by all SRB client.
 *
 */
#ifndef SRB_DIRENT_H
#define SRB_DIRENT_H

#define MAX_DIR_LEN 255 

struct  srbDirent {
#if defined(PORTNAME_c90)
        unsigned int    d_offset:32;       /* real off after this entry */
        unsigned int    d_ino:32;          /* inode number of entry */
        unsigned short  d_reclen:16;       /* length of this record */
        unsigned short  d_namlen:16;       /* length of string in d_name */
	unsigned int	d_dummy:32;	   /* pat to 64 bit boundary */
        char            d_name[MAX_DIR_LEN+1]; 
#else
        unsigned int    d_offset;       /* real off after this entry */
        unsigned int    d_ino;          /* inode number of entry */
        unsigned short  d_reclen;       /* length of this record */
        unsigned short  d_namlen;       /* length of string in d_name */
	unsigned int	d_dummy;	   /* pat to 64 bit boundary */
        char            d_name[MAX_DIR_LEN+1]; 
#endif
};

#endif	/* SRB_DIRENT_H */
