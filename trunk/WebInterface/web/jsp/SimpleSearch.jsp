<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page errorPage="error.jsp" import="ac.dl.xml.*,org.apache.log4j.*,java.util.*,org.jdom.filter.*,java.util.*, java.io.*,org.jdom.*,org.jdom.input.*"%>
<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store"); %>
<%@ taglib uri="/tldweb" prefix="xtags" %>
<%@ taglib uri="/tld/taglibs-xtags.tld" prefix="xtags1" %>

<%@ include file="loggedin.jsp" %>


<%@ include file="logger.jsp" %>
<%
/* Logger logger = Logger.getLogger("Manipulate3.jsp");

	 String wd = (String )session.getAttribute("wd");
         //locate the prop file.  Normal get this from web.xml file
         PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
*/

%>
<html>
<head>
<script type="text/javascript">
<!--
function openwindow(page){
     window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
    //window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");
}

//-->

</script>
<title>CCLRC Data Portal - Data Portal query results</title>
<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->
<style type="text/css"> 
@import url(../style/mainmenu.css); /*IE and NN6x styles*/
</style>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
</head>
<body background="../img/redbg.gif" >
<table width="100%" cellspacing="3" border="0">
<tr>
<td valign="top" width="20%" align="left">
<a href="http://www.clrc.ac.uk/"><img border="0" src="../img/clrc.gif" alt="CLRC logo" /></a>
<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br /><br />

<%-- out puts the content if the browser is not netsacpe 4.  If so out puts
html/netscape.html file from the content root.--%>
<xtags:menu netscapefile="html/netscape.html" iefile="html/ie.html">
</xtags:menu>
<!--<xtags:HPC /> -->
</td>
<td valign="top">
<h1>Data Portal query results 
<!-- prints out the list of facs that are in the fac session attribute-->
<xtags:facs name="fac" text=""/>

</h1>
<br />
&nbsp;Home&nbsp;&nbsp;&nbsp;>&nbsp;<a href="BasicSearch.jsp">Basic Search</a>
&nbsp;&nbsp;>&nbsp;<a href="SimpleSearch.jsp">Studies</a>
<hr />
<br />
<form class="mess" name="simpleresult" action="../servlet/ExpandResultsServlet" method="post">
<%
    
   //String workingDir = (String)session.getAttribute("wd");
    String sid =(String)session.getAttribute("sessionid");

    //get what the type of study the user wants
    String typeOfStudy =(String)session.getAttribute("type");
    //Document xml = null;
    File xml = null;

    //try and use tag libs
    String sxml = null;
    String sxsl = null;
    //sxml = workingDir+File.separator+"profiles"+File.separator+username+File.separator+"users.xml";
    sxml = "http://localhost:8080/dataportal/profiles/"+sid+"1.xml";
    //sxml = "file:"+File.separator+File.separator+workingDir+File.separator+"profiles"+File.separator+sid+"1.xml";
    xml =  new File(wd+File.separator+"profiles"+File.separator+sid+"1.xml");
    if(!xml.exists()){
       response.sendRedirect("../jsp/BasicSearch.jsp");
       return;
    }
    sxsl = "http://localhost:8080/dataportal/xsl/simpleresulttableDOM4J.xsl";
    //sxsl="file:"+File.separator+File.separator+workingDir+File.separator+"xsl"+File.separator+"simpleresulttableDOM4J.xsl";
%>
<%
    //get the position of the list to be presented
    String list = request.getParameter("from");
    if(list == null) list="1";
    int max1 = new Integer(list).intValue()*10;
    int min1 =  max1 - 10;

%>
<xtags1:style xml="<%=sxml%>" xsl="<%=sxsl%>">
   <xtags1:param name="min" value="<%=String.valueOf(min1)%>"/>
 <xtags1:param name="max" value="<%=String.valueOf(max1)%>"/>
</xtags1:style>


<br />
<%
ac.dl.taglib.Counter counter  = new ac.dl.taglib.Counter();
org.xml.sax.InputSource in = new org.xml.sax.InputSource(new FileInputStream(xml));
int max= counter.count(in, "MetadataRecord");

int current = Integer.parseInt(list)*10;

if(current > 10) out.println("<blockquote><input type=\"submit\" value=\"previous_ten\" name=\"this_one\" /></blockquote>");
if(current<max){
    out.println("<blockquote><input type=\"submit\" value=\"next_ten\" name=\"this_one\" /></blockquote>");
}
%>

</form>
<br />
<%--  pulls out the list of facilities that have no results from the following
request.  This is stored in the session attribute NoRes--%>
<xtags:query name="noConn" send="no" project="eminerals" message="No connections to: " host="exchange06.dl.ac.uk" />
<xtags:query name="noRes" send="no" project="eminerals" message="No results from: " host="exchange06.dl.ac.uk" />
<xtags:query name="timedOut" send="no" project="eminerals" message="Results timed out from: " host="exchange06.dl.ac.uk" />
<br />
<hr />
<%@ include file="../html/footer.html"%>/td>
</tr>
</table>
</body>
</html>