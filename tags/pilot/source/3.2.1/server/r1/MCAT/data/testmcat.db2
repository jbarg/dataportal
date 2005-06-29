#!/bin/csh
rm data/mytest.results.db2
echo "testing started - this may take a few minutes"
cd bin
test.catalog >& ../data/mytest.results.db2
cd ..
diff data/mytest.results data/test.results.db2
wc data/*test.results.db2
echo "testing completed. Successful if diff and wc results above are identical."
echo "installation and testing completed."


