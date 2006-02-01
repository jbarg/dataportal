<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ page errorPage="error.jsp"    %>

<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store");

 %>

<%@ include file="loggedin.jsp" %>
<%@ taglib uri="/tldweb" prefix="xtags" %>
<html>

<head>
<script type="text/javascript">

<!--

function openwindow(page){

   // window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");



    //window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");

}

//-->

</script>
<title>CCLRC Data Portal - Transfer</title>

<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->

<style type="text/css"> 

@import url(../style/mainmenu.css); /*IE and NN6x styles*/

</style>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<script type="text/javascript">

</script>

</head>

<body>

<table width="100%" cellspacing="0" border="0"><tr>

<td valign="top" width="180" align="left">

<!--<a href="http://www.clrc.ac.uk"><img border="0" src="../img/clrc.gif" alt="CLRC logo" /></a>-->

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

<br />&nbsp;<br /><br />
<br />&nbsp;<br /><br />
<!-- out puts the content if the browser is not netsacpe 4.  If so out puts

html/netscape.html file from the content root.-->
<xtags:menu netscapefile="html/netscape.html" iefile="html/ie.html">

</xtags:menu>


</td>

<td valign="top">

<p id="topheader">Transfer File</p>
<br />
<br />
<br />
&nbsp;Home&nbsp;&nbsp;&nbsp;>&nbsp;<a href="BasicSearch.jsp">Basic Search</a>
&nbsp;&nbsp;>&nbsp;<a href="SimpleSearch.jsp">Studies</a>
&nbsp;&nbsp;>&nbsp;<a href="Expand.jsp">Expanded Studies</a>
&nbsp;&nbsp;>&nbsp;<a href="Explore.jsp">Data</a>
&nbsp;&nbsp;>&nbsp;<a href="Checkout.jsp">Shopping Cart</a>
&nbsp;&nbsp;>&nbsp;<a href="#" onclick="return false;">File transfer</a>
<hr />
<br />
<br />
<%  

         String url = request.getParameter("url");
            
%>
<form name="filetransfer" method="post" action="../servlet/FileTransfer" >
<table>
<tr><td>Name of file to transfer : </td><td><%=url%><input type="hidden" name="url" size="30" value="<%=url%>" /></td></tr>
<tr><td>Url of destination :</td><td> <input type="text" name="urlTo" size="40" /></td></tr>
<!--<tr><td>File to transfer : </td><td> <input type="text" size="30" /></td></tr>-->
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td><input type="submit" value="Transfer" /></td><td>&nbsp;</td></tr>
</table>
</form>

<!-- delete this once it is working -->
<form name="filetransfer" method="post" action="../servlet/FileDownload" >
<table>
<tr><td><!--Name of file to download : --></td><td><input type="hidden" name="dir" size="30" value="<%=url%>" /></td></tr>
<!--<tr><td>File to transfer : </td><td> <input type="text" size="30" /></td></tr>-->
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td><input type="submit" value="Download" /></td><td>&nbsp;</td></tr>
</table>
</form>
<br />
<hr />

<%@ include file="../html/footer.html"%>
</td>

</tr>

</table>

</body>

</html>

