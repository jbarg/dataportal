<%@page contentType="text/html" import="uk.ac.clrc.dataportal.facility.facilityAdmin.*, ac.dl.xml.*"%>
<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store"); %>
<%@ include file="loggedin.jsp" %>

<%@ include file="logger.jsp" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>

<title>CCLRC Admin Manager</title>
<link rel="stylesheet" href="../style/mainmenu.css" type="text/css">
</head>
<body>
<a name="top" />
<table width="100%" cellspacing="3" border="0"><tr>
<td valign="top" width="180" align="left">
<!--<a href="http://www.clrc.ac.uk"><img border=0 src="../img/clrc.gif" alt="CLRC logo"></a>-->
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

<br />&nbsp;<br /><br />
<br />&nbsp;<br /><br />
<table width="125" cellspacing="0" border="0">

<tr><td id="headmenu">Admin</td><tr><td>
<hr class="menuhr" />
<!--&nbsp;&nbsp;<a class="menu" href="create.jsp">Create Facility</a><br/>-->
&nbsp;&nbsp;<a class="menu" href="deleteFacility.jsp">List </a><br />
&nbsp;&nbsp;<a class="menu" href="logoff.jsp">Log Off</a><br />

&nbsp;</td></tr>



</table>

</td>
<td valign="top">
<p id="topheader">Admin Manager</p>
<br /><br /><br /><br />
<hr />
<div align="right"><font size="-1"><a align="right" href="#bottom">bottom</a></font></div>

<br />
<form name="dataportal_creation" action="../servlet/createDataportalServlet" method="post">
<table>
<tr><td width="200"><b>DataPortal (ID)</b></td><td width="20">&nbsp;</td><td><input name="dataportal_id" disabled type="text" size="70" value="DataPortal"></input></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><input name="dataportal_desc" type="text" size="70" value="Data Portal instance"></input></td></tr>

<tr><td rows="3">&nbsp;</td></tr>

<tr><td rows="3" align="left"><b>Services</b></td></tr>


<tr><td width="200">Name </td><td width="20">&nbsp;</td><td>Query and Reply</td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td>QNR</td></tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><input name="qnr" type="text" size="70" value="http://localhost:8080/queryandreply/services/QueryAndReply"></input></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><input name="qnr_wsdl" type="text" size="70" value="http://localhost:8080/queryandreply/services/QueryAndReply?wsdl"></input></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><input name="qnr_desc" type="text" size="70" value="Query and Reply"></input></td></tr>

<tr><td rows="3">&nbsp;</td></tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td>Data Transfer</td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td>DTM</td></tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><input name="dts" type="text" size="70" value="http://localhost:8080/datatransfer/services/DataTransferService"></input></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><input name="dts_wsdl" type="text" size="70" value="http://localhost:8080/datatransfer/services/DataTransferService?wsdl"></input></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><input name="dts_desc" type="text" size="70" value="Data Transfer"></input></td></tr>

<tr><td rows="3">&nbsp;</td></tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td>Session Manager</td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td>SESSION</td></tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><input name="ses" type="text" size="70" value="http://localhost:8080/sessionmanager/services/SessionManager"></input></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><input name="ses_wsdl" type="text" size="70" value="http://localhost:8080/sessionmanager/services/SessionManager?wsdl"></input></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><input name="ses_desc" type="text" size="70" value="Session Manager"></input></td></tr>	

<tr><td rows="3">&nbsp;</td></tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td>Facitly Admin</td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td>FACADM</td></tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><input name="fac" type="text" size="70" value="http://localhost:8080/facility/services/FacilityAdminService"></input></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><input name="fac_wsdl" type="text" size="70" value="http://localhost:8080/facility/services/FacilityAdminService?wsdl"></input></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><input name="fac_desc" type="text" size="70" value="Facility Admin"></input></td></tr>

<tr><td rows="3">&nbsp;</td></tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td>Authentication</td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td>AUTH</td></tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><input name="auth" type="text" size="70" value="http://localhost:8080/authent/services/AUTH"></input></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><input name="auth_wsdl" type="text" size="70" value="http://localhost:8080/authent/services/AUTH?wsdl"></input></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><input name="auth_desc" type="text" size="70" value="Authentication"></input></td></tr>

<tr><td rows="3">&nbsp;</td></tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td>Look up</td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td>LOOKUP</td></tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><input name="look" type="text" size="70" value="http://localhost:8080/lookup/services/LookUpService"></input></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><input name="look_wsdl" type="text" size="70" value="http://localhost:8080/lookup/services/LookUpService?wsdl"></input></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><input name="look_desc" type="text" size="70" value="Lookup Module"></input></td></tr>

<tr><td rows="3">&nbsp;</td></tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td>Shopping Cart</td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td>CART</td></tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><input name="cart" type="text" size="70" value="http://localhost:8080/shopcart/services/ShoppingCart"></input></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><input name="cart_wsdl" type="text" size="70" value="http://localhost:8080/shopcart/services/ShoppingCart?wsdl"></input></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td><input name="cart_desc" type="text" size="70" value="Shopping Cart"></input></td></tr>
<tr><td rows="3">&nbsp;</td></tr>
<tr><td><input type="submit" name="Create" value="Create DataPortal"/></td></tr>

</table>
</form>
<br />
<br />
<div align="right"><a name="bottom"/><font size="-1"><a align="right" href="#top">top</a></font></div>

<hr />
<%@ include file="../html/footer.html"%>
</td>
</table>

</body>
</html>