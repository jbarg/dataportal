these are the jar files from:

Axis 1.1 

Cog v1.1 (but this changes without the version changing which is a bit confusing)

these files should be copied to $CATALINA_HOME/shared/lib - due to a bug (?) in tomcat 4.1.29 when running 2 instances the second base cannot
read the shared/lib folder so the way around this is to put them into $CATALINA_HOME/common/lib which works but is not a preferred solution.
