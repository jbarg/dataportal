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
<p id="footer" align="right"> <a href="http://validator.w3.org/check/referer"><img
         src="../img/vxhtml.gif"
        alt="Valid XHTML 1.0!"   border="0" height="31" width="88" /></a>
<br /> 
<br />
<a href="http://www.clrc.ac.uk/Activity/ACTIVITY=Copyright;" target="_top">&copy;  2003 CLRC DataPortal</a></p>
</td>
</tr>
</table>
</body>
</html>
