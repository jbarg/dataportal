<%@ page  isErrorPage="true" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%
    //invalidate session at this point
    //id invalidate and then sendRediect another session is the created!!!! 
    session.invalidate();
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

@import url(../style/main.css); /*IE and NN6x styles*/

</style>

</head>
<body background="../img/redbg.gif" >
<table width="100%" cellspacing="3" border="0"><tr>
<td valign="top" width="20%" align="left">
<a href="http://www.clrc.ac.uk"><img src="../img/clrc.gif"  alt="E-minerals" border="0" /></a>


<!--<form name="top-search_form" action="">

<input class="button" onclick="location='../index.html'" type="button"
value="Home page" />

</form>-->
</td>
<td valign="top">
<h1>Session Timed Out</h1>

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

<div id="footer" align="right"> <a href="http://validator.w3.org/check/referer"><img

         src="../img/vxhtml.gif"

        alt="Valid XHTML 1.0!" border="0" height="31" width="88" /></a>


<br />
<br />
<a href="http://www.clrc.ac.uk/Activity/ACTIVITY=Copyright;" target="_top">&copy; 2003 CLRC DataPortal</a></div>
</td>
</tr>
</table>
</body>

</html>



