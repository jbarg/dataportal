<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page errorPage="error.jsp" import="org.apache.log4j.*,java.util.*, java.io.*,org.jdom.input.*,org.jdom.*,ac.dl.xml.*"%>

<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store"); %>
<%@ include file="loggedin.jsp" %>
<%@ include file="logger.jsp" %>
<%@ taglib uri="/tldweb" prefix="xtags" %>
<html>
<head>
<title>CCLRC Data Portal - Exlored results</title>
<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->
<style type="text/css"> 
@import url(../style/mainmenu.css); /*IE and NN6x styles*/
</style>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<script type="text/javascript">
<!--

function openwindow(page){

     // window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
 window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");


  //  window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");

}
// sets all checkboxes
function selectAll(formID, fieldName, value)
{
  var elements = document.forms[formID].elements;
  for (i = 0; i < elements.length; i++) {
    if (elements[i].name && elements[i].name.indexOf(fieldName) > -1) {
      elements[i].checked = value; 
    }
  }
  return null;
}

//-->

</script>
</head>
<body>
<a name="top" />
<table width="100%" cellspacing="3" border="0"><tr>
<td valign="top" width="20%" align="left">
<a href="http://www.clrc.ac.uk"><img border="0" src="../img/clrc.gif" alt="CLRC logo" /></a>

<br />&nbsp;<br /><br />
<!-- out puts the content if the browser is not netsacpe 4.  If so out puts
html/netscape.html file from the content root.-->
<xtags:menu netscapefile="html/netscape.html" iefile="html/ie.html">
</xtags:menu>
<!--<xtags:HPC /> -->
</td>

<td valign="top">

<h1>Data Portal explored results</h1>
<br />
&nbsp;Home&nbsp;&nbsp;&nbsp;>&nbsp;<a href="BasicSearch.jsp">Basic Search</a>
&nbsp;&nbsp;>&nbsp;<a href="SimpleSearch.jsp">Studies</a>
&nbsp;&nbsp;>&nbsp;<a href="Expand.jsp">Expanded Studies</a>
&nbsp;&nbsp;>&nbsp;<a href="Explore.jsp">Data</a>

<hr />
<div align="right"><a name="#top"/><font size="-1"><a align="right" href="#bottom">bottom</a></font></div>

<br />
 <form class="mess" name="simpleresult" action="../servlet/AddToCart" method="post">

<%

        if(loggedIn){ 
                String workingDirXml = (String)session.getAttribute("wdx");
                String sid = (String)session.getAttribute("sessionid");
                File xml = null;
                                  //get username 
                xml =  new File(wd+File.separator+"profiles"+File.separator+sid+"3.xml");
                 if(!xml.exists()){
                    response.sendRedirect("BasicSearch.jsp");
                    return;
                }
              try{
               XSLTransformer.transformFiletoWriter(xml,  new File(wd+File.separator+"xsl","datasettable.xsl"), out);
               }
                catch(Exception e){
                    logger.fatal("Could not transform xml",e);
                    response.sendRedirect("error.jsp");
                }
        }
    else{}
%>


</form>

<div align="right"><a name="bottom"/><font size="-1"><a align="right" href="#top">top</a></font></div>

<hr />
<%@ include file="../html/footer.html"%></td>
</tr>
</table>
</body>
</html>
