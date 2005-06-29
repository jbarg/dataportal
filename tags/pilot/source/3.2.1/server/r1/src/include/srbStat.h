/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbStat.h - This header file defines the platform independent 
 * stat structure to be returned to the client. This is basically the
 * POSIX stat definition.
 */
#ifndef SRB_STAT_H
#define SRB_STAT_H

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <sys/utime.h>

#else
#include <utime.h>
#endif /* WIN32 */

/*
#include <sys/mode.h>
*/

/* definition for isDir in struct srbFd or myType in srbObjStat call */
#define IS_UNKNOWN      -1      /* don't know if it is file or dir */
#define IS_FILE         0       /* this is a file */
#define IS_DIR_1        1       /* is a collection. new desc */
#define IS_DIR_2        2       /* is a collection. listing data */
#define IS_DIR_3        3       /* is a collection. done listing data */
#define IS_DIR_4        4       /* is a collection. listing collection */

/* not a user on this machine */
#define NOT_LOCAL_USER          999999
#define FAKE_ROOT_USER          777777
#define PUBLIC_USER             "public"
#define MY_BLKSIZE              8192

/* Use fake inode for stat and gendents */
#define FAKE_INODE              10000
#define FAKE_DEV                10

typedef unsigned int srb_dev_t;
typedef unsigned int srb_ino_t;
typedef unsigned int srb_mode_t;
typedef unsigned int srb_uid_t;
typedef unsigned int srb_gid_t;
typedef unsigned int srb_int_t;
typedef srb_long_t srb_size_t;
typedef unsigned int srb_time_t;
typedef unsigned short	srb_ushort_t;

#if defined(PORTNAME_solaris)
#ifndef FSTYPSZ
#define FSTYPSZ		16
#endif
#endif

struct  srbStat
{
#if defined(PORTNAME_c90)
	srb_size_t	st_size:64;	/* File size in bytes */
	srb_dev_t 	st_dev:32;	
	srb_ino_t	st_ino:32;	
	srb_mode_t 	st_mode:32;	/* File mode */
	short		st_nlink:32;	/* Number of links */
	srb_uid_t	st_uid:32;	/* User ID of the file's owner */
	srb_gid_t	st_gid:32;	/* Group ID of the file's group */
	srb_dev_t	st_rdev:32;	/* ID of device */
					/*   This entry is defined only for */
					/*   character or block special files */
	srb_time_t	st_atim:32;	/* Time of last access */
					/* Use st_atim instead of st_atime */
					/* because in solaris, st_atime is */
					/* defined to be somthing else */
	int		st_spare1:32;
	srb_time_t	st_mtim:32;	/* Time of last data modification */
	int		st_spare2:32;
	srb_time_t	st_ctim:32;	/* Time of last file status change */
	int		st_spare3:32;
					/* Time measured in seconds since */
					/*   00:00:00 GMT, Jan. 1, 1970 */
	srb_size_t	st_blksize:32;	/* Optimal blocksize for file system
					   i/o ops */
	srb_size_t	st_blocks:32;	/* Actual number of blocks allocated
					   in DEV_BSIZE blocks */
	int		st_vfstype:32;	/* Type of fs (see vnode.h) */
	srb_size_t	st_vfs:32;		/* Vfs number */
	srb_size_t	st_type:32;	/* Vnode type */
	srb_size_t	st_gen:32;		/* Inode generation number */
	srb_size_t	st_flag:32;	/* Flag word */

	srb_uid_t	Reserved1:32;	/* Reserved */
	srb_gid_t	Reserved2:32;	/* Reserved */
	srb_ushort_t	st_access:16;	/* Process' access to file */
	srb_ushort_t	st_dummy:16;	/* pat to 32 bit boundary */
	srb_size_t	st_spare4:32;	/* Reserved. Can't do array */
	srb_size_t	st_spare4_1:32;	/* Reserved */
	srb_size_t	st_spare4_2:32;	/* Reserved */
	srb_size_t	st_spare4_3:32;	/* Reserved */
	srb_size_t	st_spare4_4:32;	/* Reserved */
#else	/* PORTNAME_c90 */
	srb_size_t	st_size;	/* File size in bytes */
	srb_dev_t 	st_dev;	
	srb_ino_t	st_ino;	
	srb_mode_t 	st_mode;	/* File mode */
	int		st_nlink;	/* Number of links */
	srb_uid_t	st_uid;		/* User ID of the file's owner */
	srb_gid_t	st_gid;		/* Group ID of the file's group */
	srb_dev_t	st_rdev;	/* ID of device */
					/*   This entry is defined only for */
					/*   character or block special files */
	srb_time_t	st_atim;	/* Time of last access */
					/* Use st_atim instead of st_atime */
					/* because in solaris, st_atime is */
					/* defined to be somthing else */
	int		st_spare1;
	srb_time_t	st_mtim;	/* Time of last data modification */
	int		st_spare2;
	srb_time_t	st_ctim;	/* Time of last file status change */
	int		st_spare3;
					/* Time measured in seconds since */
					/*   00:00:00 GMT, Jan. 1, 1970 */
	srb_int_t	st_blksize;	/* Optimal blocksize for file system
					   i/o ops */
	srb_int_t	st_blocks;	/* Actual number of blocks allocated
					   in DEV_BSIZE blocks */
	srb_int_t	st_vfstype;	/* Type of fs (see vnode.h) */
	srb_int_t	st_vfs;		/* Vfs number */
	srb_int_t	st_type;	/* Vnode type */
	srb_int_t	st_gen;		/* Inode generation number */
	srb_int_t	st_flag;	/* Flag word */

#ifdef PORTNAME_aix
	srb_uid_t	AIX_Reserved1;	/* Reserved */
	srb_gid_t	AIX_Reserved2;	/* Reserved */
	srb_ushort_t	st_Access;	/* Process' access to file. Rename in AIX because st_access is defined to something else in stat.h */
	srb_ushort_t	st_dummy;	/* pat to 32 bit boundary */
	srb_int_t	st_Spare4[5];	/* Reserved */
#else
	srb_uid_t	Reserved1;	/* Reserved */
	srb_gid_t	Reserved2;	/* Reserved */
        srb_ushort_t    st_access;      /* Process' access to file */
        srb_ushort_t    st_dummy;       /* pat to 32 bit boundary */
#ifdef PORTNAME_solaris
	char            st_fstype[FSTYPSZ];
        srb_int_t      st_spare4[1];   /* Reserved */
#else
        srb_int_t      st_spare4[5];   /* Reserved */
#endif  /* PORTNAME_solaris */
#endif	/* PORTNAME_aix */
#endif	/* PORTNAME_c90 */
};
#if defined(PORTNAME_solaris) 
/* Take care of a problem of hpss uses "struct stat" while the platform may
 * use "struct stat64" with "#define stat stat64".
 */

#if defined(__sparcv9)

struct stat32 {
        dev_t           st_dev;
        ino_t           st_ino;
        mode_t          st_mode;
        nlink_t         st_nlink;
        uid_t           st_uid;
        gid_t           st_gid;
        dev_t           st_rdev;
        off_t           st_size;
        timestruc_t     st_atim;
        timestruc_t     st_mtim;
        timestruc_t     st_ctim;
        blksize_t       st_blksize;
        blkcnt_t        st_blocks;
        char            st_fstype[_ST_FSTYPSZ];
};

#else   /* __sparcv9 */

struct stat32 {
        dev_t           st_dev;
        long            st_pad1[3];     /* reserve for dev expansion, */
                                        /* sysid definition */
        ino_t           st_ino;
        mode_t          st_mode;
        nlink_t         st_nlink;
        uid_t           st_uid;
        gid_t           st_gid;
        dev_t           st_rdev;
        long            st_pad2[2];
        off_t           st_size;
        long            st_pad3;        /* pad for future off_t expansion */
        timestruc_t     st_atim;
        timestruc_t     st_mtim;
        timestruc_t     st_ctim;
        long       	st_blksize;
        long        	st_blocks;
/*
        blksize_t       st_blksize;
        blkcnt_t        st_blocks;
*/
        char            st_fstype[_ST_FSTYPSZ];
        long            st_pad4[8];     /* expansion area */
};
#endif   /* __sparcv9 */

/* take care of situation where dirent64 and stat64 are not defined */
#if !defined(_LARGEFILE64_SOURCE)
typedef u_longlong_t ino64_t;
typedef srb_size_t off64_t;
typedef long            blksize_t;      /* used for block sizes */
typedef long            blkcnt_t;       /* count of file blocks */
typedef blkcnt_t        blkcnt64_t;     /* count of file blocks */

typedef struct dirent64 {
        ino64_t         d_ino;          /* "inode number" of entry */
        off64_t         d_off;          /* offset of disk directory entry */
        unsigned short  d_reclen;       /* length of this record */
        char            d_name[1];      /* name of file */
} dirent64_t;

#if defined(__sparcv9)
struct stat64 {
        dev_t           st_dev;
        ino_t           st_ino;
        mode_t          st_mode;
        nlink_t         st_nlink;
        uid_t           st_uid;
        gid_t           st_gid;
        dev_t           st_rdev;
        off_t           st_size;
        timestruc_t     st_atim;
        timestruc_t     st_mtim;
        timestruc_t     st_ctim;
        blksize_t       st_blksize;
        blkcnt_t        st_blocks;
        char            st_fstype[_ST_FSTYPSZ];
};

#else   /* __sparcv9 */
struct  stat64 {
        dev_t   st_dev;
        long    st_pad1[3];     /* reserve for dev expansion, */
                                /* sysid definition */
        ino64_t st_ino;
        mode_t  st_mode;
        nlink_t st_nlink;
        uid_t   st_uid;
        gid_t   st_gid;
        dev_t   st_rdev;
        long    st_pad2[2];
        off64_t st_size;        /* large file support */
        timestruc_t     st_atim;
        timestruc_t     st_mtim;
        timestruc_t     st_ctim;
        blksize_t       st_blksize;
        blkcnt64_t st_blocks;   /* large file support */
        char    st_fstype[_ST_FSTYPSZ];
        long    st_pad4[8];     /* expansion area */
};
#endif  /* __sparcv9 */
#endif  /* _LARGEFILE64_SOURCE */
#endif  /* PORTNAME_solaris */

#if defined(PORTNAME_linux)
#define stat32 stat	/* For HPSS */
/* in linux/dirent.h, the definition is somewhat different */
typedef struct myDirent {
        long            d_ino;          /* "inode number" of entry */
        int         	d_off;          /* offset of disk directory entry */
        unsigned short  d_reclen;       /* length of this record */
        char            d_name[1];      /* name of file */
} dirent_t;
#endif   /* PORTNAME_linux */

#if defined(PORTNAME_linux)
#if !defined(__USE_LARGEFILE64)
typedef srb_size_t ino64_t;
typedef srb_size_t off64_t;
typedef srb_size_t mydev_t;
typedef long            blksize_t;      /* used for block sizes */
typedef blkcnt_t        blkcnt64_t;     /* count of file blocks */

struct stat64
  {
    mydev_t st_dev;                     /* Device.  */
    unsigned int __pad1;

    __ino_t __st_ino;                   /* 32bit file serial number.    */
    __mode_t st_mode;                   /* File mode.  */
    __nlink_t st_nlink;                 /* Link count.  */
    __uid_t st_uid;                     /* User ID of the file's owner. */
    __gid_t st_gid;                     /* Group ID of the file's group.*/
    __dev_t st_rdev;                    /* Device number, if device.  */
    unsigned int __pad2;
    __off64_t st_size;                  /* Size of file, in bytes.  */
    __blksize_t st_blksize;             /* Optimal block size for I/O.  */

    __blkcnt64_t st_blocks;             /* Number 512-byte blocks allocated. */
#ifdef st_atime
    struct timespec st_atim;            /* Time of last access.  */
    struct timespec st_mtim;            /* Time of last modification.  */
    struct timespec st_ctim;            /* Time of last status change.  */
#else
    __time_t st_atime;                  /* Time of last access.  */
    unsigned long int __unused1;
    __time_t st_mtime;                  /* Time of last modification.  */
    unsigned long int __unused2;
    __time_t st_ctime;                  /* Time of last status change.  */
    unsigned long int __unused3;
#endif
    __ino64_t st_ino;                   /* File serial number. */
  };
#endif  /* __USE_LARGEFILE64 */
typedef struct myDirent64 {
        ino64_t         d_ino;          /* "inode number" of entry */
        off64_t         d_off;          /* offset of disk directory entry */
        unsigned short  d_reclen;       /* length of this record */
        char            d_name[1];      /* name of file */
} dirent64_t;
#endif   /* PORTNAME_linux */

#if defined(_WIN32)
typedef struct myDirent64 {
        ino_t     		d_ino;          /* "inode number" of entry */
        off_t         	d_off;          /* offset of disk directory entry */
        unsigned short  d_reclen:16;    /* length of this record */
        char            d_name[1];      /* name of file */
} dirent64_t;

typedef struct myDirent {
        long            d_ino;          /* "inode number" of entry */
        int         	d_off;          /* offset of disk directory entry */
        unsigned short  d_reclen;       /* length of this record */
        char            d_name[1];      /* name of file */
} dirent_t;
#define stat64 stat
#endif  /* WIN32 */

#if defined(PORTNAME_c90)
typedef struct myDirent64 {
        ino_t     	d_ino;          /* "inode number" of entry */
        off_t         	d_off;          /* offset of disk directory entry */
        unsigned short  d_reclen:16;    /* length of this record */
        char            d_name[1];      /* name of file */
} dirent64_t;

typedef struct myDirent {
        ino_t  		d_ino:32;       /* "inode number" of entry */
        off_t         	d_off:32;          /* offset of disk directory entry */
        unsigned short  d_reclen:16;    /* length of this record */
        char            d_name[1];      /* name of file */
} dirent_t;
/*
typedef struct dirent dirent_t;
#define dirent64_t dirent_t
*/
#define stat64 stat
#endif  /* PORTNAME_c90 */

#if defined(PORTNAME_osx) || defined(PORTNAME_sunos) || defined(PORTNAME_alpha)  /* True64 Alpha PSC port */ 
typedef struct myDirent {
        long            d_ino;          /* "inode number" of entry */
        int             d_off;          /* offset of disk directory entry */
        unsigned short  d_reclen;       /* length of this record */
        char            d_name[1];      /* name of file */
} dirent_t;

typedef srb_size_t ino64_t;
typedef srb_size_t off64_t;

typedef struct myDirent64 {
        ino64_t         d_ino;          /* "inode number" of entry */
        off64_t         d_off;          /* offset of disk directory entry */
        unsigned short  d_reclen;       /* length of this record */
        char            d_name[1];      /* name of file */
} dirent64_t;

#define stat64 stat	/* uses 64 bit stat native */
#endif	/* PORTNAME_osx */

#ifdef PORTNAME_aix

/* Take care of a problem of hpss uses "struct stat" while the platform may
 * use "struct stat64" with "#define stat stat64".
 */

struct  stat32
{
        dev_t   st_dev;         /* ID of device containing a directory entry
                                 * for this file.  File serial no + device
                                 * ID uniquely identify the file within the
                                 * system
                                 */
        ino_t   st_ino;         /* File serial number */
#ifdef  _NONSTD_TYPES
        ushort  st_mode_ext;
        ushort  st_mode;
#else
        mode_t  st_mode;        /* File mode; see #define's below */
#endif
        nlink_t st_nlink;       /* Number of links */
        ushort_t st_flag;       /* Flag word */
#ifdef  _NONSTD_TYPES
        ushort  st_uid_ext;
        ushort  st_uid;
        ushort  st_gid_ext;
        ushort  st_gid;
#else
        uid_t   st_uid;         /* User ID of the file's owner  */
        gid_t   st_gid;         /* Group ID of the file's group */
#endif

        dev_t   st_rdev;        /* ID of device. This entry is defined only for
                                 * character or block special files
                                 */
        int   st_size;        	/* 32 bit file size     */

        time_t  st_atime;       /* Time of last access  */
        int     st_spare1;
        time_t  st_mtime;       /* Time of last data modification */
        int     st_spare2;
        time_t  st_ctime;       /* Time of last file status change */
        int     st_spare3;
/*      blksize_t st_blksize; */
        int 	st_blksize;   /* Optimal blocksize for file system i/o ops */
        blkcnt_t st_blocks;     /* Actual number of blocks allocated
                                 * in DEV_BSIZE blocks
                                 */
        int     st_vfstype;     /* Type of fs (see vnode.h) */
        uint_t  st_vfs;         /* Vfs number */
        uint_t  st_type;        /* Vnode type */
        uint_t  st_gen;         /* Inode generation number */

        /* end of information returned by non-large file enabled stat */
#ifndef S_STAT_RESERVED_SPACE
#define S_STAT_RESERVED_SPACE 9
#endif
        uint_t  st_reserved[9];
#ifdef _KERNEL
        uint_t  st_padto_ll;
        offset_t st_llsize;     /* 64 bit file size in bytes */
#endif /* _KERNEL */
};
#endif	/* PORTNAME_aix */

#endif /* SRB_STAT_H */
