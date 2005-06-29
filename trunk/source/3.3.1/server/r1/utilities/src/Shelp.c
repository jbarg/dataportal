/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "srbClient.h"
#include "scommands.h"






int main(int argc, char **argv)
{
    printf(" Sannotate  - Annotate SRB Objects and display annotations \n");
    printf(" Sappend    - Appends a local file, standard in, or a SRB object to a target SRB object \n");
    printf(" Sattrs     - Display meta attributes used in conditions \n");
    printf(" Sauth      - Create an SRB password file with scrambled contents \n");
    printf(" Sbkupsrb   - Backup/synchronize srb data/collection  to  a target resource \n");
    printf(" Sbload     - Imports in bulk one or more local files and/or directories into SRB space \n");
    printf(" Sbregister - Registers in  bulk  one  or  more  local  files into SRB space \n");
    printf(" Sbunload   - Exports in bulk a collection or a  SRB  container to local file system \n");
    printf(" Scat       - Streams SRB Objects to standard output \n");
    printf(" Scd        - Changes working SRB collection \n");
    printf(" Schdefres  - Changes default resource for the current session.\n");
    printf(" Schhost    - Changes connection host for the current session.\n");
    printf(" Schksum    - Performs checksum on existing SRB objects and stores them in  the MCAT \n");
    printf(" Schmod     - Modify access permissions for SRB objects and collections \n");
    printf(" Sconvert   - convert an image or sequence of images. Needs ImageMagick at server-side \n");
    printf(" Scp        - Copies an object to a new collection in SRB space \n");
    printf(" SdelValue  - Admin command to delete SRB tokens such as users, location, etc\n");
    printf(" Senv       - Displays environmental file content \n");
    printf(" Serror     - Display Error Information from Error Number \n");
    printf(" Sexit      - Clears environmental files created during SRB operation \n");
    printf(" Sget       - Exports SRB objects into local file system \n");
    printf(" SgetColl   - Displays information about SRB collections \n");
    printf(" SgetD      - Displays information about SRB files (objects) \n");
    printf(" SgetR      - Displays information about SRB resources \n");
    printf(" SgetT      - Displays information about SRB tickets \n");
    printf(" SgetU      - Displays information about SRB users \n");
    printf(" Shelp      - Displays one-line help messages for all Scommands \n");
    printf(" Singestgroup - Adds a  group user  account \n");
    printf(" Singesttoken - Adds SRB native metadata types (reserved keywords) \n");
    printf(" Singestuser- Register a new user into SRB space \n");
    printf(" Sinit      - Initializes environmental files for SRB operation \n");
    printf(" Sln        - Soft link SRB files in other collections \n");
    printf(" Sls        - Lists SRB objects and collections \n");
    printf(" Slscont    - Lists SRB containers \n");
    printf(" Smeta      - Displays and ingests meta data values \n");
    printf(" Smkcont    - Creates a new SRB container \n");
    printf(" Smkdir     - Creates a new SRB collection \n");
    printf(" SmodColl   - Modifies some system metadata information on SRB collections \n");
    printf(" SmodD      - Modifies some system metadata information on SRB objects \n");
    printf(" SmodR      - Modifies some system metadata information on SRB resorces \n");
    printf(" SmodifyUser- Modifies some system metadata information on SRB users \n");
    printf(" Smv        - Changes the collection for objects in SRB space \n");
    printf(" Spasswd    - Change SRB (Text) Password \n");
    printf(" Spcommand  - Execute proxy command in SRB \n");
    printf(" Sphymove   - Moves a SRB object to a new SRB resource \n");
    printf(" Spullmeta  - Accesses metadata in bulk from MCAT \n");
    printf(" Spushmeta  - Ingests metadata in bulk into MCAT \n");
    printf(" Sput       - Imports local files into SRB space \n");
    printf(" Spwd       - Displays current working SRB collection \n");
    printf(" Squery     - query metadata to find objects in SRB \n");
    printf(" Sregister  - Registers an object as a SRB object in MCAT \n");
    printf(" Sregisterlocation - Register a Resource location in MCAT \n");
    printf(" Sregisterresource - Register a Physical Resource in MCAT \n");
    printf(" Sreplcont  - Replicates an existing SRB Container \n");
    printf(" Sreplicate - Replicates an existing SRB object in a (possibly) new resource \n");
    printf(" Srm        - Removes SRB objects \n");
    printf(" Srmcont    - Removes SRB container \n");
    printf(" Srmdir     - Removes an existing SRB collection \n");
    printf(" Srmticket  - Remove a ticket \n");
    printf(" Srmtrash   - Purge SRB objects and sub-collections in the \"trash\" collection \n");
    printf(" Srsync     - Synchronize files in the local file system and the copy stored in SRB  \n");
    printf(" Ssh        - executes  Scommands in a shell type environment\n"); 
    printf(" Sstage     - Stage an SRB object  stored in Sun's SamFs-type resources from offline to online. not applicable to other storage systems  \n");
    printf(" Ssyncd     - Synchronizes copies of an SRB object \n");
    printf(" Ssyncont   - Synchronize SRB containers \n");
    printf(" Stcat      - Streams ticketed SRB Objects to standard output \n");
    printf(" Sticket    - Issue tickets for SRB objects and collections \n");
    printf(" Stls       - Lists ticketed SRB objects and collections \n");
    printf(" Stoken     - Displays information about SRB-aware types. \n");
    printf(" Sufmeta    - Query,insert,delete user-defined metadata for SRB data/collection/user,resources\n");
    printf(" Szone      - Insert, delete and modify Zone and Zone information  \n");
    printf(" srbpget    - Exports SRB objects into local file system with client driven threads \n");
    printf(" srbpput    - Imports local files into SRB space with client driven threads \n");
exit (0);
  
}






