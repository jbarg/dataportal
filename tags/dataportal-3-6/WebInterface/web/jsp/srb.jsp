<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ page errorPage="error.jsp"   import="org.apache.axis.client.*,javax.xml.rpc.ParameterMode,org.apache.axis.encoding.XMLType,javax.xml.namespace.QName,ac.dl.xml.*,org.apache.log4j.*,java.util.*, java.io.*" %>

<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store");
 %>
<%@ include file="loggedin.jsp" %>
<%@ include file="logger.jsp" %>
<%@ taglib uri="/tldweb" prefix="xtags" %>
<html>
<head>
<script type="text/javascript">
<!--
function openwindow(page){
   // window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
    //window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");

}
function change(){ 
    //this means tar
    var checked = document.srbtransfer.format[0].checked;

    if(checked) document.srbdownload.format.value = "tar";
    else document.srbdownload.format.value = "jar";
    
    //alert(document.srbdownload.format.value);
}

//-->

</script>
<title>CCLRC Data Portal - SRB</title>
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
<p id="topheader">SRB Download</p>
<br />
<br /><br />
&nbsp;Home&nbsp;&nbsp;&nbsp;>&nbsp;<a href="BasicSearch.jsp">Basic Search</a>
&nbsp;&nbsp;>&nbsp;<a href="SimpleSearch.jsp">Studies</a>
&nbsp;&nbsp;>&nbsp;<a href="Expand.jsp">Expanded Studies</a>
&nbsp;&nbsp;>&nbsp;<a href="Explore.jsp">Data</a>
&nbsp;&nbsp;>&nbsp;<a href="Checkout.jsp">Shopping Cart</a>
&nbsp;&nbsp;>&nbsp;<a href="#" onclick="return false;">SRB transfer</a>
<hr />
<br />
<br />
<%  
       
       
        String sid = (String)session.getAttribute("sessionid");
        String url = request.getParameter("url");
         String ret[]  = null;
         try{
             Properties prop = (Properties)session.getAttribute("props");
            String url2 = prop.getProperty("CART");
            String endpoint = url2;

            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "getNote" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op2", XMLType.XSD_INT, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ARRAY );

            Object[] ob = new Object[]{url,sid};
            ret = (String[]) call.invoke(ob );
            if(ret[0] == null) ret[0] = ""; //ret[0] is the notes
    }
    catch(Exception e){
            logger.error("Could not display notes for url "+url,e);
    }
        
        // String url = request.getParameter("url");
        // int pos = url.lastIndexOf("/");
        // String dataset = url.substring(pos+1,url.length());

%>
<form name="srbtransfer" method="post" action="../servlet/SRBTransfer" >
<table>

<!--<tr><td>Name of collection to transfer : </td><td><input type="text" name="url" size="30" value="<%=url%>" /></td></tr>-->
<tr><td>Name of collection to transfer : </td><td><%=ret[1]%><input type="hidden" name="url" size="40" value="<%=url%>" /></td></tr>
<tr><td>Url of destination :</td><td> <input type="text" name="urlTo" size="40" /></td></tr>
<!--<tr><td>File to transfer : </td><td> <input type="text" size="30" /></td></tr>-->
<tr><td>Format&nbsp;&nbsp;&nbsp;</td><td>&nbsp;
<!--<input type="radio" name="format" value="zip">Zip</input>-->
<input type="radio" name="format" value="tar" onchange="change();" checked="checked">Tar</input>
<input type="radio" name="format" value="jar" onchange="change();" >Jar</input></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td><input type="submit" value="Transfer" /></td><td></td></tr>
</table>
</form>

<!-- delete this once it is working -->
<form name="srbdownload" method="post" action="../servlet/SRBDownload" >
<table>
<tr><td><!--Name of collection to download : --></td><td><input type="hidden" name="dir" size="40" value="<%=url%>" /></td></tr>
<!--<tr><td>File to transfer : </td><td> <input type="text" size="30" /></td></tr>-->
<tr><td><input type="hidden" value="tar" name="format" /></td><td>&nbsp;</td></tr>
<tr><td><input type="submit" value="Download" /></td><td>&nbsp;</td></tr>
</table>
</form>
<br />
<!--<blockquote>
a href="download.jsp?dir=<%=url%>">Download collection</a></blockquote>-->
<hr />

<%@ include file="../html/footer.html"%>
</td>

</tr>

</table>

</body>

</html>

