<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page errorPage="error.jsp"   import="org.apache.axis.client.*,javax.xml.rpc.ParameterMode,org.apache.axis.encoding.XMLType,javax.xml.namespace.QName,ac.dl.xml.*,org.apache.log4j.*,java.util.*, java.io.*,javax.xml.transform.*,javax.xml.transform.dom.*,javax.xml.transform.stream.*,javax.xml.parsers.*,org.jdom.*,org.xml.sax.*,java.security.*,java.security.interfaces.*,java.math.*" %>
<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store");
 %>
<%@ include file="loggedin.jsp" %>
<%@ include file="logger.jsp" %>
<%@ taglib uri="http://java.sun.com/jstl/core_rt" prefix="c" %>
<%@ taglib uri="/tldweb" prefix="xtags" %>

<html>
<head>
<title>CCLRC Data Portal - Checkout</title>
<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->
<style type="text/css"> 
@import url(../style/mainmenu.css); /*IE and NN6x styles*/
</style>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<script type="text/javascript">
<!--

var win;
function seenoteWS(page){

    if(navigator.appVersion.indexOf("MSIE") !=-1 ){

        //alert("micrsoft");
        if(win) win.close();
        var x = event.screenX+15;
        var y = event.screenY-150;
        win = window.open("../jsp/seenote.jsp?url="+page+"","Add_note","toolbar=no, width=380, height=350, left="+x+",top="+y+"");

    }
    else if(navigator.userAgent.indexOf("Mozilla/4") != -1 && navigator.appName.indexOf("Netscape") !=-1 && parseInt(navigator.appVersion) < 5){
        //alert("netsacpe 4");
        //win = window.open("../jsp/seenote.jsp?url="+page+"","Add_note"," width=100, height=100,toolbar=no");
        win = window.open("../jsp/seenote.jsp?url="+page+"","Add_note","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=yes,width=380,height=350,left=0,top=0");
    }
    else{
        win = window.open("../jsp/seenote.jsp?url="+page+"","Add_note","toolbar=no, width=385, height=360");
    }
}

function addnote(page){
    window.open("../jsp/addnote.jsp?url="+page+"","Add_note","toolbar=no, width=300, height=300");

}

function nothing(){
    window.status = 'Done';
    return true;

}

function openwindow(page){
    //window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
    window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");

    // window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");
}

function check(){
    var temp = window.confirm("Are you sure you want to clear your shopping cart?");

    if(temp == true) return true;
    else return false;
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

//checks that all items are not selected
function checkall1(formID, fieldName){

var oneNotChecked = false;

var elements = document.forms[formID].elements;

 for (i = 0; i < elements.length; i++) {
    
    if (elements[i].name && elements[i].name.indexOf(fieldName) > -1  && (elements[i].checked == false)) {
      oneNotChecked = true;
    //alert(elements[i].checked +" name  " + elements[i].name);
    }
 }
 if(!oneNotChecked){
    var temp = window.confirm("Are you sure you want to clear all of your shopping cart?");

    if(temp == true) return true;
    else return false;
 }
 else return true;

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
<h1>Shopping Cart</h1>
<br />
&nbsp;Home&nbsp;&nbsp;&nbsp;>&nbsp;<a href="BasicSearch.jsp">Basic Search</a>
&nbsp;&nbsp;>&nbsp;<a href="SimpleSearch.jsp">Studies</a>
&nbsp;&nbsp;>&nbsp;<a href="Expand.jsp">Expanded Studies</a>
&nbsp;&nbsp;>&nbsp;<a href="Explore.jsp">Data</a>
&nbsp;&nbsp;>&nbsp;<a href="Checkout.jsp">Shopping Cart</a>
<hr />
<div align="right"><font size="-1"><a align="right" href="#bottom">bottom</a></font></div>

<%
        String sid  = (String)session.getAttribute("sessionid");
        
        Properties prop  = (Properties)session.getAttribute("props");
%>
<%-- Set the cart instance and pass through the sid, url and properties--%>
<jsp:useBean class="uk.ac.dl.beans.CheckOutBean" id="checkout" scope="request">
<jsp:setProperty name="checkout" property="*" />
<jsp:setProperty name="checkout" property="sid" value="<%=sid%>" />
<jsp:setProperty name="checkout" property="properties" value="<%=prop%>" />
<jsp:setProperty name="checkout" property="wd" value="<%=wd%>" />
<jsp:setProperty name="checkout" property="*"  />

</jsp:useBean>
<%--  Call the web service and get user cart information --%>
<% 
    
   try{
        session.setAttribute("sort",checkout.getBy());
        checkout.setFiles();
        checkout.getCart();
       
   }
   catch(Exception e){
        throw e;
   }
%>

<form name="deleteSelected" onsubmit="return checkall1('deleteSelected','delete');" action="../servlet/DeleteSelectedWS" method="post">

<c:choose>
<c:when test="<%=checkout.isEmpty()%>">
    <br /><br /><br />There are no items in your shopping basket<br /><br /><br />
</c:when>
<c:otherwise>
<%
    try{
        XSLTransformer.transformJDOMtoWriter(checkout.getDoc(), checkout.getFiles() , out);

        XSLTransformer.transformJDOMtoWriter(checkout.getDoc(),checkout.getDatasets() , out);

        XSLTransformer.transformJDOMtoWriter(checkout.getDoc(),checkout.getStudys() , out);
    }
    catch(Exception e){
        logger.error("Unable to transform shopping cart",e);
    }
%>
</c:otherwise>
</c:choose>



<c:choose>
<c:when test="<%=!checkout.isEmpty()%>" >
    <table>
    <tr>

            <td width="70%" />
           <td>
    <input type="submit" value="Delete Selected" />
    </td>
            <td align="right">
                    <input type="button" value="Select None" onclick="selectAll('deleteSelected','delete',false);"/>
            </td>
            <td align="right">
                    <input type="button" value="Select All" onclick="selectAll('deleteSelected','delete',true);"/>
            </td>
    </tr>
</table>

</c:when>
</c:choose>
</form>

<c:choose>
<c:when test="<%=!checkout.isEmpty()%>" >
    <table border="0">
    <tr>
    <td width="140">

    <form name="date" action="Checkout.jsp" method="post">
    <select name="by">
    <option value="date">Date</option>
    <option value="name">Name</option>
    </select><input type="submit" value="Sort" />
    </form>
    </td>
    <td>

    <form name="Delete_contents" onsubmit="return check();" action="../servlet/DeleteCart" method="post">
    <input type="submit" value="Delete all contents" />
    </form>
    </td>
    </tr>
    </table>
</c:when>
</c:choose>
<div align="right"><a name="bottom"/><font size="-1"><a align="right" href="#top">top</a></font></div>

<hr />
<%@ include file="../html/footer.html"%></td>
</tr>
</table>
</body>
</html>

