#!/bin/sh

#Make the webpage for it as well
echo '<HTML>
<HEAD>
   <TITLE>SRB S-Command Man Pages</TITLE>
</HEAD>
<BODY>

<CENTER>
<H1>
<FONT FACE="Arial,Helvetica">SRB S-Command Man Pages</FONT></H1></CENTER>

<UL>' > srbcommands.html

#Go to the man pages
cd man1

# convert all the man pages to html
for manpage in S* ; do
  man2html $manpage > ../html/$manpage.html
  echo "     <LI><A HREF='http://www.npaci.edu/DICE/srb/srbcommands/$manpage.html'>$manpage</A></LI>" >> ../srbcommands.html
done

echo '</UL>

</BODY>
</HTML>' >> ../srbcommands.html

exit 0
