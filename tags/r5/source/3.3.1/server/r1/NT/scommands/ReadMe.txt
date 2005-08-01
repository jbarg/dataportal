1. Currently only one session is allowed in a Windows machine for a user
   to use Windows s-commands.

2. Similarly to using S-commands in UNIX, two files are needed to run Windows 
   S-commands. They are 'MdasEnv' and 'MdasAuth' with no file extensions. 

   S-commands look for these two files in the following directories.

      (1) %USERPROFILE%\.srb
      (2) %HOMEDRIVE%\srb

      Note: (1) is new in SRB 3.1 and is introduced to support multiple users 
                in one Windows machine.
            (2) is used in SRB 2.x.x and all previous versions. This is still
                supported in 3.1 but has lower priority than (1).

3. Launch the 'srbcommands.html' in the S-command zip file to view on-line manuals
   of S-commands.

4. User can use environment variables to override those SRB parameters defined
   in both 'MdasEnv' and 'MdasAuth'.

   e.g. to change the TCP/IP port number to 5717, set the env variable 'srbPort' by:
      C:> set srbPort=5617

5. Contact SRB team at srb@sdsc.edu should you have any question.

