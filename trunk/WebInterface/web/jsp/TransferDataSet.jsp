<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ page errorPage="error.jsp"   import="org.apache.axis.client.*,javax.xml.rpc.ParameterMode,org.apache.axis.encoding.XMLType,javax.xml.namespace.QName,ac.dl.xml.*,org.apache.log4j.*,java.util.*, java.io.*,javax.xml.transform.*,javax.xml.transform.dom.*,javax.xml.transform.stream.*,javax.xml.parsers.*,org.jdom.*,org.xml.sax.*,java.security.*,java.security.interfaces.*,java.math.*" %>
<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store");

 %>

<%@ include file="loggedin.jsp" %>
<%@ include file="logger.jsp"%>

<%@ taglib uri="/tldweb" prefix="xtags" %>
<html>

<head>

<title>CCLRC Data Portal - Transfer</title>

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



    //window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");

}

//-->

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
<!--<xtags:HPC /> -->
</td>

<td valign="top">

<p id="topheader">Transfer DataSet</p>
<br />
<br />
<br />
<hr />
<br />
<br />
<%  

         String url = request.getParameter("url");
         org.jdom.Document doc1 = null; 
         //call web service
    try{ 
              String dn  = (String)session.getAttribute("sessionid");
            Properties prop = (Properties)session.getAttribute("props");

            String url1 = prop.getProperty("CART");
          // System.out.println("url of shopping cart "+url);
            //normal long winded service call
            String endpoint = url1;
            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "getCart" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ELEMENT );

            //old way using invoke
            Object[] ob = new Object[]{dn};
            org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke(ob );
            org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();

            org.jdom.Element el = buildert.build(ret);
            el.detach();
           doc1  = new org.jdom.Document(el);
           //Saver.save(doc1,"c:/shop.xml2");
        } catch (Exception e) {
           // logger.error("building document",e);
            e.printStackTrace();

        }
        
        org.jdom.Element el = doc1.getRootElement();
        List list = el.getChildren("item");
        Iterator it = list.iterator();
        String urls = "";
        String name = "";
        while(it.hasNext()){
            Element e = (Element)it.next();
            //System.out.println(e);
            if(e.getAttribute("url").getValue().equals(url)){
             urls = e.getChild("urls").getText().trim();
            name = e.getChild("name").getText();

            }
submit
        }
        String[] urlsnow= urls.split("   ");
        for(int i = 0; i<urlsnow.length;i++){
            System.out.println("url "+urlsnow[i]);
        }
            
%>
<form name="filetransfer" method="post" action="../servlet/TransferDataSet" >
<table>
<tr><td>Name of Dataset : </td><td><input   type="test" name="name" size="30" value="<%=name%>" /><input type="hidden" name="url" value="<%=url%>" /></td></tr>

<!--<tr><td>Urls of dataset</td></tr>-->

<%--
for(int i = 0; i<urlsnow.length;i++){
            out.println("<tr><td><input   type=\"text\" name=\"urls\" size=\"30\" value=\""+urlsnow[i]+"\" /></td></tr>");
        }
--%>
<tr><td>Url of folder destination :</td><td> <input type="text" name="urlTo" size="30" /></td></tr>
<!--<tr><td>File to transfer : </td><td> <input type="text" size="30" /></td></tr>-->
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td><input type="submit" value="Transfer" /></td><td>&nbsp;</td></tr>
</table>
</form>
<br />
<hr />

<%@ include file="../html/footer.html"%>
</td>

</tr>

</table>

</body>

</html>
