<%@page contentType="text/html" import="uk.ac.clrc.dataportal.facility.facilityAdmin.*, ac.dl.xml.*"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>

<title>CCLRC Facility Manager</title>
<link rel="stylesheet" href="../style/mainmenu.css" type="text/css">


<script>
//chaecks that all fields are not nothing
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
<!--&nbsp;&nbsp;<a class="menu" href="create.jsp">Create Facility</a><br/>-->
&nbsp;&nbsp;<a class="menu" href="edit.jsp">Edit </a><br />
&nbsp;&nbsp;<a class="menu" href="edit.jsp">Change password</a><br />
&nbsp;</td></tr>



</table>

</td>
<td valign="top">
<p id="topheader">Create new Facility</p>
<br /><br /><br /><br />
<hr />
<br />
<form name="createFacility" action="../servlet/createFacilityServlet" method="post" onsubmit="return check();" />
<table>
<tr><td width="200"><b>Facility (ID)</b></td><td width="20">&nbsp;</td><td><input type="text" name="fac_id" value="type_name_here"/></td></tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3" align="left"><b>Services</b></td></tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td><font size="2">XML Wrapper</font></td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td><font size="2">XMLW</font></td></tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><input type="text" name="xmlw" size="70" value="http://localhost:8080/wrappers/services/xml_wrapperISIS" /></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><input type="text" name="xmlw_wsdl" size="70" value="http://localhost:8080/wrappers/services/xml_wrapperISIS?wsdl" /></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><font size="2">Returns XML data from remote database</font></td></tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td><font size="2">Access and Control Module</font></td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td><font size="2">ACM</font></td></tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><input type="text" name="acm" size="70" value="http://localhost:8080/acmisis/services/acm" /></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><input type="text" name="acm_wsdl" size="70" value="http://localhost:8080/acmisis/services/acm?wsdl" /></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><font size="2">Controls access to the metadata</font></td></tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3"><input type="submit" value="Create Facilty"/></td></tr>

</table>
</form>
<br />


<br />
<hr />
<%@ include file="../html/footer.html"%>
</td>
</table>

</body>
</html>