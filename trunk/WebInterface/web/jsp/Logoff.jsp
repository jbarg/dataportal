<%@ page import="java.io.*, java.util.*" isErrorPage="true" %>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<%
    //invalidate session at this point
    //id invalidate and then sendRediect another session is the created!!!! 
    session.invalidate();
    
   
%>
<title>CCLRC Data Portal</title>
<LINK REL=STYLESHEET HREF="../style/mainmenu.css" TYPE="text/css">
</head>
<body>
<table width="100%" cellspacing="3" border="0"><tr>
<td valign=top width="180" align="left">
<!--<a href="http://www.clrc.ac.uk"><img border=0 src="../img/clrc.gif" alt="CLRC logo"></a>-->
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

<br />&nbsp;<br /><br />
<br />&nbsp;<br /><br />


</td>
<td valign="top">
<p id="topheader">Log Off</p>
<br />
<br />
<br />
<br />
<hr />
<br />
<br />
<br />
<table cellspacing="2" border="0" cellpadding="3">
</TABLE>
<table>
<tr><td>Thank you for using the <a href="../">dataportal.</a>
</td></tr>

</table>
<br />
<br />
<br />
<br />
<hr />
<%@ include file="../html/footer.html"%>
</td>
</table>
</body>
</html>
