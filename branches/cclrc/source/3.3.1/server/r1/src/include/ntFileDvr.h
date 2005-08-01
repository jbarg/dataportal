/* Copyright   1999   The Regents of the University of California
 * All Rights Reserved
 *
 * Permission to use, copy, modify and distribute any part of this Storage
 * Resource Broker (SRB) software package, for educational, research and
 * non-profit purposes, without fee, and without a written agreement is
 * hereby granted, provided that the above copyright notice, this paragraph
 * and the following three paragraphs appear in all copies.
 * Those desiring to incorporate this SRB software into commercial products
 * or use for commercial purposes should contact the Technology Transfer
 * Office, University of California, San Diego, 9500 Gilman Drive, La Jolla,
 * CA 92093-0910, Ph: (619) 534-5815, FAX: (619) 534-7345.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SRB SOFTWARE, EVEN IF THE
 * UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE SRB SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND THE UNIVERSITY
 * OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.  THE UNIVERSITY OF CALIFORNIA MAKES NO
 * REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER IMPLIED OR
 * EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, OR THAT THE USE OF THE
 * SRB SOFTWARE WILL NOT INFRINGE ANY PATENT, TRADEMARK OR OTHER RIGHTS.
 */

/* ntFileDvr.h - Definitions and prototypes for ntFileDvr.c
 */
 
#ifndef _NT_SRB_FILE_DVR_H__
#define _NT_SRB_FILE_DVR_H__

#ifdef _WIN32

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>  
#include <dirent.h> 
#include "c.h"
#include "srb.h"
#include "elogExtern.h"


#endif  /* _WIN32 */

#endif	/* UNIX_FILE_DVR_H */