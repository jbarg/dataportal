<%@page contentType="text/html" %>
<%@page import="uk.ac.clrc.dataportal.facility.*"%>
<% session.invalidate();%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>

<title>CCLRC Facility Manager</title>
<link rel="stylesheet" href="../style/mainmenu.css" type="text/css">


<script>
//checks that all fields are not nothing
function check(){


return true;

}

</script>
</head>
<body>
<table width="100%" cellspacing="3" border="0"><tr>
<td valign="top" width="180" align="left">
<!--<a href="http://www.clrc.ac.uk"><img border=0 src="../img/clrc.gif" alt="CLRC logo"></a>-->
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

<br />&nbsp;<br /><br />
<br />&nbsp;<br /><br />
<table width="125" cellspacing="0" border="0">

<tr><td id="headmenu">Facility</td><tr><td>
<hr class="menuhr" />
<!--&nbsp;&nbsp;<a class="menu" href="create.jsp">Create Facility</a><br/>
&nbsp;&nbsp;<a class="menu" href="deleteFacility.jsp">List </a><br />-->
&nbsp;&nbsp;<a class="menu" href="../">Log In</a><br />

&nbsp;</td></tr>



</table>

</td>
<td valign="top">
<p id="topheader">Logged Off</p>
<br /><br /><br /><br />
<hr />
<br />
Tbank you for using the Facility Manager Tool.
<br />


<br />
<hr />
<%@ include file="../html/footer.html"%>
</td>
</table>

</body>
</html>