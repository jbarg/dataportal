<%@ page  isErrorPage="true" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%
    //invalidate session at this point
    //id invalidate and then sendRediect another session is the created!!!! 
    if(session != null){ 
        session.invalidate();
    }
%>
<html>
<head>
<title>CCLRC Data Portal</title>
<title>CCLRC DataPortal - Logged Out</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta name="robots" content="index, nofollow" />
<meta name="description" content="This site is the dataportal" />
<meta name="keywords" content="Dataportal, metadata, clrc, dl, facilities,Estedi, data, search, Glen Drinkwater, Synchrotron Radiation,British Atmospheric Data Centre, shopping basket" />
<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" />
<style type="text/css">
@import url(../style/mainmenu.css); /*IE and NN6x styles*/
</style>
</head>
<body>
<table width="100%" cellspacing="0" border="0"><tr>
<td valign="top" width="180" align="left">
<!--<a href="http://www.clrc.ac.uk"><img src="../img/clrc.gif"  alt="E-minerals" border="0" /></a>-->
<!--<form name="top-search_form" action="">

<input class="button" onclick="location='../index.html'" type="button"
value="Home page" />

</form>-->
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

<br />&nbsp;<br /><br />
<br />&nbsp;<br /><br />
</td>
<td valign="top">
<p id="topheader">Session Timed Out</p>
<br />
<br />
<br />
<br />
<hr />
<br />
<br />
<br />
<br />
<table>
<tr><td>Your certificate has expired, and you have been logged off the Data Portal.</td></tr>
<tr><td>Thank you for using the <a href="../">dataportal.</a>
</td></tr>
</table>
<br />
<br />

<br />
<br />

<hr />
<br />
<%@ include file="../html/footer.html"%>
</td>
</tr>
</table>
</body>

</html>



