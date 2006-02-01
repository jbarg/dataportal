<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ page errorPage="error.jsp"   import="org.apache.axis.client.*,javax.xml.rpc.ParameterMode,org.apache.axis.encoding.XMLType,javax.xml.namespace.QName,ac.dl.xml.*,org.apache.log4j.*,java.util.*, java.io.*,javax.xml.transform.*,javax.xml.transform.dom.*,javax.xml.transform.stream.*,javax.xml.parsers.*,org.jdom.*,org.xml.sax.*,java.security.*,java.security.interfaces.*,java.math.*" %>

<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store");

 %>

<%@ include file="loggedin.jsp" %>
<%@ taglib uri="/tldweb" prefix="xtags" %>
<html>

<head>

<title>CCLRC Data Portal - Checkout</title>

<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->

<style type="text/css"> 

@import url(../style/mainmenu.css); /*IE and NN6x styles*/

</style>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

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



//-->

</script>

</head>

<body background="../img/redbg.gif">

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

<%  



            //set static log for the class

            Logger logger = Logger.getLogger("Checkout.jsp");

             //locate the prop file.  Normal get this from web.xml file
            String wd =  (String)session.getAttribute("wd");
            PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            
            
            //if dpuser, not allowed to download data, so change stylesheet

            //so that when the user presses the download button, it takes them

            // to a another page

            String xsl  = null;

            String xsldpuser = "";

            //get the sorting request of the shopping cart

            String by  = (String)request.getParameter("by");

           



            org.jdom.Document doc1  = null;

             //get users name

            String dn  = (String)session.getAttribute("sessionid");

           

            if(by == null){

                 xsl = "checkoutWS";

            }

            else  if(by.equals("date")) xsl = "checkoutWSdate";

            else  if(by.equals("name")) xsl = "checkoutWS";



            

            session.setAttribute("sort",by);

            //call ws to get the shopping basket

             org.w3c.dom.Element ret = null;

           try{ 

            Properties prop = (Properties)session.getAttribute("props");

            String url = prop.getProperty("CART");
          // System.out.println("url of shopping cart "+url);

            //normal long winded service call

            String endpoint = url;

            Service  service = new Service();

            Call     call    = (Call) service.createCall();



            call.setTargetEndpointAddress( new java.net.URL(endpoint) );

            call.setOperationName( "getCart" );

            call.addParameter( "op1", XMLType.XSD_INT, ParameterMode.IN );



            call.setReturnType( XMLType.SOAP_ELEMENT );







            //old way using invoke

            Object[] ob = new Object[]{dn};



            ret = (org.w3c.dom.Element) call.invoke(ob );

            //System.out.println(ret);





            org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();

            org.jdom.Element el = buildert.build(ret);
            el.detach();
           doc1  = new org.jdom.Document(el);

            //Saver.save(doc1,"c:/check.xml£");

              

        } catch (Exception e) {
            logger.error("building document",e);
            e.printStackTrace();

        }



        //check if cart is empty

        Element root = doc1.getRootElement();

        List list = root.getChildren("item");

        boolean isEmpty = false;

        if(list.size() == 0){

            isEmpty = true;

        }

%>



<form name="deleteSelected" action="../servlet/DeleteSelectedWS" method="post">

<%

    if(isEmpty){
        out.println("<br /><br /><br />There are no items in your shopping basket<br /><br /><br />");
    }

    else{

        try{

            XSLTransformer.transformJDOMtoWriter(doc1, new File(wd+File.separator+"xsl"+File.separator+xsl+".xsl"), out);



            XSLTransformer.transformJDOMtoWriter(doc1, new File(wd+File.separator+"xsl"+File.separator+xsl+"dataset.xsl"), out);



            XSLTransformer.transformJDOMtoWriter(doc1, new File(wd+File.separator+"xsl"+File.separator+xsl+"study.xsl"), out);

        }

        catch(Exception e){
            logger.error("unable to transofrm shopping cart",e);
         

        }

    }

%>

<br />

<%

    if(!isEmpty){

%>

<div align="right">

<input type="submit" value="Delete" /></div>

<%}else{}

%>

</form>

<%

    if(!isEmpty){

%>

<table border="0">

<tr>

<td width="140">

<form name="date" action="Checkout.jsp" method="post">

<select name="by">

<option value="date">Date</option>

<option value="name">Name</option>

</select>

<input type="submit" value="Sort" />

</form>

</td>



<td>



<form name="Delete_contents" onsubmit="return check();" action="../servlet/DeleteCart" method="post">

<input type="submit" value="Delete all contents" />

</form>



</td>

</tr>

</table>

<% }else {}

%>

<hr />

<p class="footer" align="right"><a href="http://validator.w3.org/check/referer"><img

         src="../img/vxhtml.gif"

        alt="Valid XHTML 1.0!" border="0" height="31" width="88" /></a>

<br />

<br /><a href="http://www.clrc.ac.uk/Activity/ACTIVITY=Copyright;" target="_top">&copy;  2003 CLRC DataPortal</a></p>

</td>

</tr>

</table>

</body>

</html>

