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
<LINK REL=STYLESHEET HREF="../style/main.css" TYPE="text/css">
</head>
<body background="../img/redbg.gif">
<table width="100%" cellspacing=3 border=0><tr>
<td valign=top width="18%" align=left>
<a href="http://www.clrc.ac.uk"><img border=0 src="../img/clrc.gif" alt="CLRC logo"></a>
<br />&nbsp;<br /><br />

</td>
<td valign=top>
<h1>Log off</h1>
<hr />
<br />
<br />
<br />
<table cellspacing=0 border=0 cellpadding=3>
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
