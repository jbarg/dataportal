<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page errorPage="error.jsp" import="org.apache.log4j.*,ac.dl.xml.*,java.util.*, java.io.*,org.jdom.input.*,org.jdom.*" %>

<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store"); %>

<%@ include file="loggedin.jsp" %>

<%@ include file="logger.jsp" %>
<%@ taglib uri="/tldweb" prefix="xtags" %>

<html>

<head>

<title>CCLRC Data Portal - Expanded results</title>

<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->

<style type="text/css"> 

@import url(../style/mainmenu.css); /*IE and NN6x styles*/

</style>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<script type="text/javascript">

<!--

function openwindow(page){

       //window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
 window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");


   // window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");

}

//-->

</script> 

</head>

<body background="../img/redbg.gif">

<table width="100%" cellspacing="3" border="0">

<tr>

<td class="tdwidth" valign="top" width="20%" align="left">

<a href="http://www.clrc.ac.uk"><img border="0" src="../img/clrc.gif" alt="CLRC logo" /></a>

<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br /><br />



<!-- out puts the content if the browser is not netsacpe 4.  If so out puts

html/netscape.html file from the content root.-->
<xtags:menu netscapefile="html/netscape.html" iefile="html/ie.html">

</xtags:menu>
</td>

<td valign="top">

<h1>Data Portal expanded results</h1>
<br/>
&nbsp;Home&nbsp;&nbsp;&nbsp;>&nbsp;<a href="BasicSearch.jsp">Basic Search</a>
&nbsp;&nbsp;>&nbsp;<a href="SimpleSearch.jsp">Studies</a>
&nbsp;&nbsp;>&nbsp;<a href="Expand.jsp">Expanded Studies</a>


<hr />

<br />

 <form class="mess" name="simpleresult" action="../servlet/Explore" method="post">

<%

   	//set static log for the class

	if(loggedIn){

            String sid = (String)session.getAttribute("sessionid");
      
            //Document xml = null;
            //transfomr xml 
            File xml = null;
            xml =  new File(wd+File.separator+"profiles"+File.separator+sid+"2.xml");
            if(!xml.exists()){
                 response.sendRedirect("../jsp/BasicSearch.jsp");
                 return;
            }
            try{
                 XSLTransformer.transformFiletoWriter(xml,  new File(wd+File.separator+"xsl","fullstudydesctable.xsl"), out);
            }
            catch(Exception e){
                 logger.fatal("Could not transform xml",e);
                 response.sendRedirect("error.jsp");
            }
}
	else{}

%>



</form>



<hr />

<p class="footer" align="right"><a href="http://validator.w3.org/check/referer"><img

         src="../img/vxhtml.gif"

        alt="Valid XHTML 1.0!"  border="0" height="31" width="88" /></a>

<br />

<br /><a href="http://www.clrc.ac.uk/Activity/ACTIVITY=Copyright;" target="_top">&copy;  2003 CLRC DataPortal</a></p>

</td>

</tr>

</table>

</body>

</html>


