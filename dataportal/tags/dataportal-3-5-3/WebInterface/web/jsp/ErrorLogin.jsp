<%@ page import="java.util.*,java.io.*" %>

<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store");
 %>
<html><head>
<title>CCLRC Data Portal - Error</title>
<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> 
<style type="text/css"> 
@import url(../style/mainmenu.css); /*IE and NN6x styles*/
</style>
</head>
<body> 
<table width="100%" cellspacing="0" border="0"><tr>
<td valign="top" width="180" align="left">
<!--<a href="http://www.clrc.ac.uk" /><img border="0" src="../img/clrc.gif" alt="CLRC logo"></a>-->
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

<br />&nbsp;<br /><br />
<br />&nbsp;<br /><br />
</td>
<td valign="top">
<p id="topheader">Error</p>
<br />
<br />
<br />
<br />
<hr /><br />

<br />
<h4>Your username and/or password is incorrect.</h4>
<br />
<br />
<p><a href="#" onclick="history.back(-1);">Please try again</a></p>
<br />
<br />
<hr />
<%@ include file="../html/footer.html"%></td>
</tr>
</table>
</body>
</html>
