#include <string>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <strstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <stdlib.h>

using namespace std;

void testStringRedirect(){
  strstream *mystr = new strstream();
  string a, b;
  *mystr << "crystalrsh.wait.1" << endl;
  *mystr >> a;
  *mystr << "This is some more text in the buffer\n";
  *mystr >> b;
  cout << "a = " << a << endl;
  cout << "b = " << b << endl;
  cout << ( (*mystr).str() );
}

string* addEnding( string *s ){
  string *sn = new string( *s + "/" );
  return sn;
}

string* testRoutine( string &cs ){
  cs = cs.substr( 3 );
  string *rs = new string( cs ); 
  (*rs) += " with a bit added";
  return rs;
}

void testLoadNothing(){
  ifstream *ifs;
  ifs = new ifstream( "doesntExist" );
  cout << ( ( !ifs ) ? "ifs == 0" : "ifs != 0" );
  cout << endl;
  cout << ( ( *ifs == NULL ) ? "*ifs == NULL" : "*ifs != NULL" ); 
  cout << endl;
  cout.flush();
}

void testStringCmp(){
  string *sa, *sb;
  sa = 0;
  cout << "before first new; sa == 0 is ";
  cout << ( ( sa == 0 ) ? "true" : "false" ) << " sa = " << sa << endl;
  sa = new string( "aString" );
  cout << "sa = \"aString\", sb = null\n";
  cout << "*sa == *sb " << ( (*sa == *sb) ? "true" : "false" ) << endl;
  sb = new string( "bString" );
  cout << "sa = \"aString\", sb = \"bString\"\n";
  cout << "*sa == *sb " << ( (*sa == *sb) ? "true" : "false" ) << endl;
  delete sa;
  sa = new string( "bString" );
  cout << "sa = \"bString\", sb = \"bString\"\n";
  cout << "*sa == *sb " << ( (*sa == *sb) ? "true" : "false" ) << endl;
  cout << "*sa == *(new string( *sa )) " << ( (*sa == *(new string( *sa )) ) ? "true" : "false" ) << endl;
  delete sa;
  delete sb;
  cout << "delete sa; sa == NULL is ";
  cout << ( ( sa == NULL ) ? "true" : "false" );
  cout << endl;
  cout.flush();
}

main(){
  string userName( "rshtest" );
  string pid( "12345" );
  string dt( "2000:14:01:19:29" );
  string ck;
  ck = "ps -u " + userName + " | awk '$1==" + 
    pid + "{ print \"Running\" > \"./isRunning" + dt + "\" }'";
  cout << ck << endl;
  //
  string *inFiles = new string[ 2 ];
  inFiles[0] = "/home/daniel/dir/tester";
  inFiles[1] = "testfile";
  //
  int first, last;
  for ( int i=0; i<2; i++ ){
    first = inFiles[i].find( "/" );
    last = inFiles[i].find_last_of( "/" );
    cout << "First=" << first << "\nLast=" << last << "\n";
    if ( ( first != 0 ) && ( last > 1 ) ){
      cout << "Changing filename: " << inFiles[i] << " to ";
      inFiles[i] = inFiles[i].substr( ++last );
      cout << inFiles[i] << endl;
    }
  }
  //
  string trial( "ThisIsATrialString" );
  cout << "Original string = " << trial << endl;
  string result( trial );
  string *testResult;
  testResult = testRoutine( result );
  cout << "Final strings:\n";
  cout << "Original string = " << trial << endl;
  cout << "Processed string = " << result << endl;
  cout << "Processed string via pointer= " << *testResult << endl;
  delete testResult;
  cout << "Processed string via pointer after delete= " << *testResult << endl;
  cout.flush();

  string *lerr, lout;
  lout = "Hello (statically)";
  lerr = new string( lout + "This is dynamically allocated" );
  cout << lout << endl << *lerr << endl;
  delete lerr;
  lerr = new string( "" );
  if ( (*lerr).empty() ) cout << "lerr is now empty\n";
  cout.flush();
  testLoadNothing();
  testStringCmp();
  testStringRedirect();
  //void testParamPassing(){
  string a,b;
  a = "This is a string";
  b = *( addEnding( addEnding( &a ) ) );
  cout << "After two addEndings:\n" << b << endl;

  b = *( addEnding( addEnding( new string( "I am making " + a + " this string up here\n" ) ) ) );
  cout << "After two addEndings and made in situ\n" << b << endl;

  //Testing temp file generation
  cout << "Testing temp file generation..." << endl;
  
  string charstring = "crs_XXXXXX";
  int stringlength = strlen( charstring.c_str() );
  char *charname = new char[ stringlength ];
  
  cout << "Template filename = ";
  cout << charstring.c_str() << endl;
  cout << charname << endl;
  int mkstemp_integer;
  for ( int i=0; i<10; i++ ){
    strcpy( charname, charstring.c_str() );
    mkstemp_integer = mkstemp( charname );
    if ( mkstemp_integer ){
      cout << "Error...\n";
    }
    cout << "Temp filename = " << charname << endl;
  }   
  cout << "The end....\n";
  cout.flush();
}
