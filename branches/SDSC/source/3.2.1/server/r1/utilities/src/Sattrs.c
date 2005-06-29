/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include <stdio.h>


main(argc, argv)
{
  int i, nbytes;
  FILE *fd;
  char rdline[400];
  char *rdl, *tmpStr, *tmpStr1;
  printf("The following parameters can be specified in  Scommands that allow the -A option or that can take conditions as part of srbObject Names:\n");
  printf("DATA            name of the object\n");
  printf("COPY            replica number of the object\n");
  printf("GUID            GUID value of the object. This is a string \n");
  printf("DTYPE           data type of the object\n");
  printf("COLLECTION      collection where the object can be found\n");
  printf("RESOURCE        resource name where the object resides\n");
  printf("RTYPE           resource type where the object resides\n");
  printf("NETADDR         net address of the resource where the object resides \n");
  printf("PATH            physical path name of the object\n");
  printf("DCOMMENTS       comments on the object\n");
  printf("DTIME           time of last modification of the object\n");
  printf("USER            user name\n");
  printf("OWNER           owner of the object\n");
  printf("TICKET          ticket issued on the object\n");
  printf("SIZE            size of the object\n");
  printf("CTIME           creation timestamp of the object\n");
  printf("DIRTY           dirty-flag  of the object\n");
  printf("VNUM            version number of the object\n");
  printf("ATTRCONDD       user-defined attribute-condition on the object\n");
  printf("ATTRCONDC       user-defined attribute-condition on the collection\n");
  printf("ATTRCONDU       user-defined attribute-condition on users\n");
  printf("ATTRCONDR       user-defined attribute-condition on the resource\n\n");

  printf("Sample Usage:\n");
  printf("  SgetD -A \"SIZE > 15000\" '*' \n");
  printf("  SgetD -A \"DTYPE = 'C code'\" '*' \n");
  printf("  SgetD -A \"RESOURCE like '*raja*' \" '*' \n");
  printf("  SgetD -A \"SIZE > 15000 &RESOURCE like 'du*'\" '*' \n");
  printf("  SgetD -A \"ATTRCONDD name = val\" '*' \n");
  printf("       this queries for data with userdefined attribute-value pairs\n");
  printf("       the form is <AttrName> <Operation> <Value> with spaces between them\n");
  printf("  SgetD -A \"ATTRCONDD name = val &ATTRCONDD name2 = val2\" '*'  \n");
  printf("       upto 5 ATTRCONDD can be used in the query, they will be treated as AND queries \n");
  printf("       upto 5 ATTRCONDC can be used in the query \n");
  printf("       upto 4 ATTRCONDR can be used in the query \n");
  printf("       upto 4 ATTRCONDU can be used in the query \n");
  printf("       for OR use the || as shown below \n");
  printf("  SgetD -A \"SIZE <15000 && > 10000\" '*' \n");
  printf("       this queries for data sizes between 10000 and 15000\n");
  printf("  SgetD -A \"RESOURCE like '*raja*' || like '*du*'\" '*' \n");
  printf("       this queries for data located in resources having names with substrings raja or du  \n");
  printf("       note that the keyword RESOURCE is not used twice \n");
  printf("  SgetD -A \"RESOURCE like '*raja*' || like '*du*'  &SIZE>10000\" '*' \n");
  printf("       this queries for data whose size is greater than 100000 and \n");
  printf("         which are located in resources having names with substrings raja or du  \n");
  exit (0);
}
