<%@page contentType="text/html" import="uk.ac.clrc.dataportal.facility.facilityAdmin.*, ac.dl.xml.*"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>

<title>CCLRC Facility Manager</title>
<link rel="stylesheet" href="../style/mainmenu.css" type="text/css">
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
<p id="topheader">Manager</p>
<br /><br /><br /><br />
<hr />
<br />
<table>
<tr><td width="200"><b>Facility (ID)</b></td><td width="20">&nbsp;</td><td>ISIS</td></tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3" align="left"><b>Services</b></td></tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td>XML Wrapper</td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td>XMLW</td></tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><a href="http://escdmg.dl.ac.uk:8080/wrappers/services/xml_wrapperISIS">http://escdmg.dl.ac.uk:8080/wrappers/services/xml_wrapperISIS</a></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><a href="">http://escdmg.dl.ac.uk:8080/wrappers/services/xml_wrapperISIS?wsdl</a></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td>Returns XML data from remote database</td></tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td width="200">Name </td><td width="20">&nbsp;</td><td>Access and Control Module</td></tr>
<tr><td width="200">Type </td><td width="20">&nbsp;</td><td>ACM</td></tr>
<tr><td rows="3"></td>&nbsp;</tr>
<tr><td width="200">URL Endpoint</td><td width="20">&nbsp;</td><td><a href="http://escdmg.dl.ac.uk:8080/acmisis/services/acm"/>http://escdmg.dl.ac.uk:8080/acmisis/services/acm<a/></td></tr>
<tr><td width="200">WSDL Endpoint</td><td width="20">&nbsp;</td><td><a href="">http://escdmg.dl.ac.uk:8080/acmisis/services/acm?wsdl</a></td></tr>
<tr><td width="200">Description </td><td width="20">&nbsp;</td><td>Returns XML data from remote database</td></tr>
</table>
<br />


<br />
<hr />
<%@ include file="../html/footer.html"%>
</td>
</table>

<%

AdminUDDI ad = new AdminUDDI();
System.out.println("hello");
org.jdom.Document build = XML_JDOMBuilder.build(new java.io.File("c:/isis.xml"));
System.out.println(build.toString());

org.jdom.Element el = build.getRootElement();


ad.createFacility(Converter.JDOMtoDOM(build).getDocumentElement());

%>
</body>
</html>