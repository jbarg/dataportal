We no longer recommend the use of these scripts (but are leaving them
here as examples).  Instead, we recommend you do this in two separate
steps and run the db2 commands by hand.  See README.MCAT.INSTALL.

#!/bin/csh
#source $HOME/.cshrc
#rm data/myinstall.results.db2 data/mytest.results.db2
#RUN  UnInstall or Clean if required. No clean up is automatically done.
#echo "connecting to mcat database"
#db2 connect to mcat
#echo "connected."
#echo "installation started"
#db2 -tvf  data/catalog.install.db2 >& data/myinstall.results.db2
#FOR DB2 V8.1, you can use db2sql92 , with user/passwd 
#db2sql92 -d srbtest -a user/passwd -f  data/catalog.install.db2 >& data/myinstall.results.db2
#diff data/myinstall.results.db2 data/install.results.db2
#wc data/*install.results.db2
#echo "installation completed. Successful if diff and wc results above are identical."
#echo "testing started - this may take a few minutes"
#cd bin
#test.catalog >& ../data/mytest.results.db2
#cd ..
#diff data/mytest.results.db2 data/test.results.db2
#wc data/*test.results.db2
#echo "testing completed. Successful if diff and wc results above are identical."
#db2 connect reset
#db2 terminate
#echo "installation and testing completed."





