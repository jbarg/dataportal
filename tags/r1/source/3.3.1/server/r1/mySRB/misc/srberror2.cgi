#!/bin/csh
echo "Content-type: text/html"
echo  ""
echo  ""
echo "<html>"
echo "<body bgcolor=#EAE9FF>"
echo "<h3> SRB Error "
echo $QUERY_STRING
echo "</h3>"
./Serror $QUERY_STRING
echo '<form><input type="button" value="Back"  onclick="history.go(-2)"></form>'
echo "</body>"
echo "</html>"
