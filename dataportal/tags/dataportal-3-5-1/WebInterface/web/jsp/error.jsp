<%@ page isErrorPage="true" import="java.io.*, java.util.*,org.apache.log4j.*" %>
<%@ include file="loggedin.jsp" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>CCLRC Data Portal - Error</title>
<style type="text/css"> 

@import url(../style/mainmenu.css); /*IE and NN6x styles*/

</style>
</head>
<body background="../img/redbg.gif">
<table width="100%" cellspacing=3 border=0><tr>
<td valign=top width="20%" align=left>
<a href="http://www.clrc.ac.uk"><img border=0 src="../img/clrc.gif" alt="CLRC logo"></a>
<br />&nbsp;<br /><br />

<xtags:menu netscapefile="html/netscape.html" iefile="html/ie.html">

</xtags:menu>
</td>
<td valign=top>
<h1>Error Page</h1>
<hr />
<br />

<table cellspacing=0 border=0 cellpadding=3>
</TABLE>
<table>
<tr><td>An exception has occured, please <a href="#" onclick="history.back(-1);return false;">try again.</a></td></tr>
<tr><td>&nbsp;<br /></td></tr>
<tr><td>If the problem persists please report the problem</td></tr>
</table>

<%
       // Logger logger = Logger.getLogger(this.getClass().getName());
       // logger.error(exception);

%>
<br />
<br />
<hr />
<%@ include file="../html/footer.html"%>
</td>
</table>
</body>
</html>



