<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ page   import="org.apache.log4j.*,java.util.*, java.io.*" %>
errorPage="error.jsp" 
<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store");

 %>

<%@ include file="loggedin.jsp" %>
<%@ include file="logger.jsp"%>
<%@ taglib uri="/tldweb" prefix="xtags" %>
<html>
<script type="text/javascript">

<!--

function openwindow(page){

    // window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
 window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");


<br />
<form name="history" action="history.jsp" method="post">
<input type="submit" class="button" value="History" />

</form>
    //window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");

}

//-->

</script>
<head>

<title>CCLRC Data Portal - Transfer</title>

<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->

<style type="text/css"> 

@import url(../style/mainmenu.css); /*IE and NN6x styles*/

</style>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<script type="text/javascript">

</script>

</head>

<body>

<table width="100%" cellspacing="0" border="0"><tr>

<td valign="top" width="180" align="left">

<!--<a href="http://www.clrc.ac.uk"><img border="0" src="../img/clrc.gif" alt="CLRC logo" /></a>-->

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

<br />&nbsp;<br /><br />
<br />&nbsp;<br /><br />
<!-- out puts the content if the browser is not netsacpe 4.  If so out puts

html/netscape.html file from the content root.-->
<xtags:menu netscapefile="html/netscape.html" iefile="html/ie.html">

</xtags:menu>


</td>

<td valign="top">

<p id="topheader">Transfer DataSet</p>
<br />
<br />
<br />
<hr />
<br />
<br />

<h4>There was an error with the transfer.</h4>



<%
    ArrayList ok  =(ArrayList)session.getAttribute("transferedok");
    if(ok == null){}
    else if(ok.isEmpty()){}
    else{
        out.print("<p><font color='blue'>These files were transfered ok</font></p>");
        for(int i = 0;i<ok.size();i++){
           
            out.print(ok.get(i)+"<br/>");
        }
    }
%>
<br />
<%
HashMap error  =(HashMap)session.getAttribute("transferederror");
    if(error == null){}
    else if(error.isEmpty()){}
    else{
         out.print("<p><font color='red'>These files incurred an error</font></p>");
        Set list = error.keySet();
        Iterator it = list.iterator();
        while(it.hasNext()){
           String url = (String)it.next();
           String errorMessage = (String)error.get(url);
            out.print("<b>"+url+"</b><br/>");
            out.println("<i>Error:</i>  "+errorMessage+"</br><br/>");
        }
    }

%>

<hr />

<%@ include file="../html/footer.html"%>
</td>

</tr>

</table>

</body>

</html>
