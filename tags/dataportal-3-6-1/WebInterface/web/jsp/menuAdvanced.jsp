

<%@ page import="java.util.*,java.io.*" %>
<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store");
 %>
 <%@ taglib uri="/tldweb" prefix="xtags" %>
 <%@ taglib uri="/tld/taglibs-xtags.tld" prefix="xtags1" %>
 

  <html>
<head>
<script type="text/javascript">
<!--

function openwindow(page){
   // alert("../help/Help.html#"+page);
   window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");


    //window.open("style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=0,resizable=1, scrollbars=yes");
}
//-->
</script>


<title>CLRC Data Portal/Estedi - Basic database search</title>
  <link rel="stylesheet" href="../style/mainNN4frame.css" type="text/css" /> 
<style type="text/css"> 
@import url(../style/main.css); /*IE and NN6x styles*/
</style>
<base target="_top"/>


</head>
<body background="../img/redbg.gif" >

<table width="140" cellspacing="3" border="0"><tr>
<td valign="top" width="140" align="left">
<a href="http://www.clrc.ac.uk"><img border="0" src="../img/clrc.gif" alt="CLRC logo" /></a>

<br />&nbsp;<br /><br />

<xtags:netscape netscapefile="html/netscape.html">

<form name="norestrict-search_form" target="_top" action="">

<input type="button"  class="button" onclick="parent.location.href='BasicSearch.jsp'" value="Basic 
search" />
</form>
<br />
<form name="logoff"  method="post" action="../servlet/SaveCartServlet">

<input class="button" type="submit" value="Log off" />
</form>

<br />

<form name="see_cart" action="Checkout.jsp" method="post">
<input type="submit" class="button" value="Shopping 
Cart" />
</form>
<br />
<form name="help" action="" method="">
<input type="button" class="button" value="Help" onclick="openwindow('_Toc13298566');return false" />
</form>
</xtags:netscape>
<xtags:HPC /> 
<br />

		</td>
</tr>
</table>
  </body>

</html>
