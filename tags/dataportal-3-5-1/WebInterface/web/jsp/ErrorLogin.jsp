<%@ page import="java.util.*,java.io.*" %>

<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store");
 %>
<html><head>
<title>CCLRC Data Portal - Error</title>
<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> 
<style type="text/css"> 
@import url(../style/main.css); /*IE and NN6x styles*/
</style>
</head>
<body> 
<table width="100%" cellspacing="3" border="0"><tr>
<td valign="top" width="20%" align="left">
<a href="http://www.clrc.ac.uk" /><img border="0" src="../img/clrc.gif" alt="CLRC logo"></a>
<br />&nbsp;<br /><br />
</td>
<td valign="top">
<h1>Error</h1>
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
