<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page errorPage="error.jsp"   %>
<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store");
 %>
<%@ include file="loggedin.jsp" %>
<%@ include file="logger.jsp" %>
<html>
<head>
<title>E-minerals Mini Grid</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Pragma" content="no-cache" />
<meta http-equiv="cache-control" content="no-store" />
<meta name="robots" content="index, follow" />
<meta name="description" content="This site is the dataportal" />
<meta name="keywords" content="Dataportal, metadata, clrc, dl,
facilities,E-minerals, data, search, Glen Drinkwater, Synchrotron Radiation,British Atmospheric Data Centre, shopping basket" />
<link rel="stylesheet" href="../style/eminNN4.css" type="text/css" />
<style type="text/css">
@import url(../style/emin2.css); /*IE and NN6x styles*/
</style>
</head>
<body>


<table id="top_table"  width="100%" cellspacing="3" ><tr>
<td id="td_top1" valign="top" width="18%">
<a href="http://www.clrc.ac.uk"><img src="../img/emin1.gif" width="70" height="90" alt="E-minerals" border="0" /></a>
<br />
<br />
<a href="http://eminerals.org"><img src="../img/emin2.gif" width="70" height="90"  alt="E-minerals" border="0" /></a>
<br />
<br />
<a href="http://eminerals.org"><img src="../img/emin3.gif" width="70" height="90"  alt="E-minerals" border="0" /></a>
<br />
<br />
<a href="http://eminerals.org"><img src="../img/emin4.gif" width="70" height="90"  alt="E-minerals" border="0" /></a>
<br />
<br />
<a href="http://eminerals.org"><img src="../img/emin5.gif" width="70" height="90"  alt="E-minerals" border="0" /></a>
<br />

<br />
<a href="http://eminerals.org"><img src="../img/emin6.gif" width="70" height="90"  alt="E-minerals" border="0" /></a>


</td>
<td id="td_top2" valign="top">
<h1>Welcome to the E-minerals Mini Grid</h1>
<br />
<br />
<br />
<%
    //get users session id
    String sessionid = (String)session.getAttribute("sessionid");
%>
<br />
<br />
<br />
<a href="../jsp/BasicSearch.jsp">DataPortal</a>
<br />
<br />
<a href="http://wk-pc1.dl.ac.uk/HPCPortal/Session/usertransfer.cgi?session=<%=sessionid%>">HPC Portal</a>
<br />
<br />
<a href="http://wk-pc1.dl.ac.uk:8080/datainsertion/SelectStudy.jsp">Insertion Tool</a>
<br />
<br />
<!--<a href="http://wk-ibm1.dl.ac.uk/INFOPortal">Info Portal</a>
<br />
<br />-->
<br />
<br />
<br />
<br />
 <div id="footer" align="right"><a
href="http://validator.w3.org/check/referer"><img src="../img/vxhtml.gif" border="0" width="88" height="31" alt="Valid XHTML 1.0!"></a>
<br /><a href="http://eminerals.org/">&copy; 2004 E-minerals Mini Grid</a></div>
</td>
</tr>
</table>
</body>
</html>
