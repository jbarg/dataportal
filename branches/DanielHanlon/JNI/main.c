/** File main.c **/

#include <stdlib.h>
#include <jni.h>

int main (int argc, char *argv[])
{
  JNIEnv *env;
  JavaVM *jvm;
  JavaVMInitArgs vm_args;
  JavaVMOption options[3];
  int nbOptions;
  jint res;
  jclass myJavaClass;
  jobject javaGUI;
  jmethodID constructorID;
  char *classPath; /*, *libraryPath;*/
  
  /** Code to be added here **/
  
  fprintf(stderr, "Begin main C code\n");
  /** Start the Java Virtual Machine **/
  
  classPath = malloc(256);
  strcpy(classPath, "-Djava.class.path=.:./lib/cog.jar:./lib/cryptix.jar:./lib/gnu-regexp-1.0.8.jar:./lib/providerutil.jar:./lib/xerces.jar:./lib/xml4j.jar:./lib/iaik_jce_full.jar:./lib/iaik_ssl.jar:./lib/jndi.jar:./lib/ldap.jar"); 
  /**strcpy(classPath, "-Djava.class.path=.:./src_lib" );*/	
  
  /*strdup(libraryPath, "-Djava.library.path=/myLibDir"); */
  
  fprintf(stderr, "Set options\n");
  nbOptions=0;
  options[0].optionString = classPath; nbOptions++;
  /*options[1].optionString = libraryPath; nbOptions++;*/
  vm_args.version = JNI_VERSION_1_2; /* Specifies the JNI version used */
  vm_args.options = options;
  vm_args.nOptions = nbOptions;
  vm_args.ignoreUnrecognized = JNI_TRUE; /* JNI won't complain about unrecognized options */
  
  fprintf(stderr, "Invoke JVM\n");
  res = JNI_CreateJavaVM(&jvm,(void **)&env,&vm_args);
  
  /*free(classPath);*/
  /*free(libraryPath);*/
  fprintf(stderr, "JVM started\n");
  
  /*
   * Create an instance of the Class SwingMenu.
   * Before calling NewObject we must get the constructor
   * of the Java class. This constructor must de passed
   * as an argument to NewObject.
   */
  myJavaClass = (*env)->FindClass(env, "MyMain");
  constructorID = (*env)->GetMethodID(env,myJavaClass,"<init>", "()V");
  javaGUI = (*env)->NewObject(env,myJavaClass,constructorID);
  /* Invoke main */
  
  
  /* Finish */
  (*jvm)->DestroyJavaVM(jvm);
}

