<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">



<%@ page  import="org.apache.log4j.*,java.util.*, java.io.*,ac.dl.xml.*,org.jdom.*,org.jdom.input.*,org.apache.axis.client.*,javax.xml.rpc.ParameterMode,org.apache.axis.encoding.XMLType" %>
errorPage="error.jsp" 


<%@ include file="loggedin.jsp" %>
<%@ taglib uri="/tldweb" prefix="xtags" %>
<%



        

    	//set static log for the class

	 Logger logger = Logger.getLogger("Manipulate.jsp");

	 String wd = (String )session.getAttribute("wd");
            


        String url = request.getParameter("url");

        String wherefrom = request.getParameter("where");

        session.setAttribute("urlbadc",url);

        session.setAttribute("where",wherefrom);



        //get all the info from the shopping cart

       // String url = (String)session.getAttribute("urlbadc");

       

    String name =(String)session.getAttribute("sessionid");

      Document doc1 = null;

        //call service get cart

        try{ 

            Properties prop = (Properties)session.getAttribute("props");

            String url2 = prop.getProperty("CART");

            //normal long winded service call

            String endpoint = url2;

            Service  service = new Service();

            Call     call    = (Call) service.createCall();



            call.setTargetEndpointAddress( new java.net.URL(endpoint) );

            call.setOperationName( "getCart" );

            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );



            call.setReturnType( XMLType.SOAP_ELEMENT );

            //old way using invoke
            Object[] ob = new Object[]{name};
            org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke(ob );
            //System.out.println(ret);

            org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();

            org.jdom.Element el = buildert.build(ret);
            el.detach();
            doc1  = new org.jdom.Document(el);

            //Saver.save(doc1,"c:/check.xml�");

              

        } catch (Exception e) {

            e.printStackTrace();

        }

        Saver.save(doc1, new File(wd+File.separator+"profiles"+File.separator+"cera"+name+".xml"));

       Element el = doc1.getRootElement();

      List list = el.getContent();

      Iterator it = list.iterator();

      Element efile = null;

      while(it.hasNext()){

        Object ob = it.next();

        if(ob instanceof Element){

            Element e = (Element)ob;

            if(e.getAttributeValue("url").equals(url)){

                 efile = e.getChild("cera");

            }

        }

      }

        

        

        //get all start and end dates

        String startyear = efile.getAttributeValue("startyear");

        String startmonth = efile.getAttributeValue("startmonth");

        String startday = efile.getAttributeValue("startday");

    

        String endyear = efile.getAttributeValue("endyear");

        String endmonth = efile.getAttributeValue("endmonth");

        String endday = efile.getAttributeValue("endday");



        //get all bounding areas

        String minlon = efile.getAttributeValue("minlon");

        minlon  = String.valueOf(Float.parseFloat(minlon)/1000);



        String minlat = efile.getAttributeValue("minlat");

        minlat  = String.valueOf(Float.parseFloat(minlat)/1000);



        String minalt = efile.getAttributeValue("minalt");



        String maxlon = efile.getAttributeValue("maxlon");

        maxlon  = String.valueOf(Float.parseFloat(maxlon)/1000);



        String maxlat = efile.getAttributeValue("maxlat");

         maxlat  = String.valueOf(Float.parseFloat(maxlat)/1000);



        String maxalt = efile.getAttributeValue("maxalt");

        

	

%>



<html>

<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<title>CCLRC Data Portal - Manipulate</title>

<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->

<style type="text/css"> 

@import url(../style/mainmenu.css); /*IE and NN6x styles*/

</style>

<script type="text/javascript">

<!--

function openwindow(page){

  /// window.open("../help/Help.html","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
    // window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
 window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");



  //  window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");

}





var minalt = 1;

var i;

function check(){

var message = "Your values are out of the given ranges in the following sections: -\n";

var val = new Array();

val[0] =<%= minalt%>;

val[1] = <%= maxalt%>;

val[2] =<%= minlat%>;

val[3] = <%= maxlat%>;

val[4] =<%= minlon%>;

val[5] =<%= maxlon%>;

val[6] = "2";



var Error_message = new Array();

Error_message[0] = "Minimum altitude";

Error_message[1] = "Maximum altitude";



Error_message[2] = "Minimum latitiude";



Error_message[3] = "Maximum latitude";



Error_message[4] = "Minimum longitiude";

Error_message[5] = "Maximum longitiude";





for(i = 0; i < 6;i +=2){



    if(document.manipulate.elements[i].value < val[i] || document.manipulate.elements[i].value.length < 1){

        message += "                         "+Error_message[i] +"\n";

    }

    

}

var i1;

for(i1 = 1; i1 < 6; i1+=2){

    if(document.manipulate.elements[i1].value > val[i1] ||  document.manipulate.elements[i1].value.length < 1){

        message += "                         " + Error_message[i1] +"\n";

    }

   }





/*check the date now */

var startyear = <%= startyear%>;

var endyear  = <%=endyear%>;

var startmonth = <%=startmonth%>;

var endmonth =<%=endmonth%>;

var timemessage = "Your dates conflict ";



//get values from the date

var startyearindex = document.manipulate.StartYear.selectedIndex;

var startyearvalue = document.manipulate.StartYear.options[startyearindex].value;

var endyearindex = document.manipulate.EndYear.selectedIndex;

var endyearvalue = document.manipulate.EndYear.options[endyearindex].value;

var startmonthindex = document.manipulate.StartMonth.selectedIndex;

var startmonthvalue = document.manipulate.StartMonth.options[startmonthindex].value;

var endmonthindex = document.manipulate.EndMonth.selectedIndex;

var endmonthvalue = document.manipulate.StartMonth.options[endmonthindex].value;



if(startyearvalue < startyear){

    message += "\n Start year must be greater  than "+startyearvalue+"\n";

}

if(startyearvalue == startyear && startmonthvalue < startmonth){

message += "Start month must be greater than "+startmonth+"\n";

 }





if(endyearvalue > endyear){

    message += "\n End year must be less  than "+endyearvalue+"\n";

}

if(endyearvalue == endyear && endmonthvalue > endmonth){

message += "End month must be less than "+endmonth+"\n";

 }





if(startyearvalue > endyearvalue){

message += " \nStart year must be less than end year\n";

}

if(startyearvalue == endyearvalue){

   

    if(startmonthvalue > endmonthvalue){



        message += "\nYour dates conflict with each others \n";

    }

}



if(document.manipulate.minlat.value > document.manipulate.maxlat.value){

message += "Minimum latitude must be less than maximum latitude\n";

}



if(message == "Your values are out of the given ranges in the following sections: -\n"){

  

    return true;

}

else {

    alert(message);

    return false;

}

}



function checknone(){

var index = document.country.countries.selectedIndex;

var indexvalue = document.country.countries.options[index].value;



    if(indexvalue == "none"){

        alert("Please choose a country");

        return false;

}

else return true;

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
</td><td valign="top">

<h1>Results</h1>



<hr />

<br />

<form action="../jsp/Manipulate2WS.jsp" method="post" onsubmit="return check();" name="manipulate">

<table border="1" width="100%">

<tr  bgcolor="C0C0C0">

<td align="center" width="16%">&nbsp;</td>

<td align="center" width="16%">Range</td>

<td align="center" width="23%">Min</td>



<td align="center" width="23%">Max</td>

</tr>

<tr>

<td>

Altitude (hectoPascals)</td>

<td><%= minalt%> to <%= maxalt%> </td>

<td align="center"><input type="text" size="10" name="minalt"/></td>

<td align="center"><input type="text" size="10" name="maxalt"/></td>

</tr>

<tr>

<td>

Latitude (degrees)</td>

<td><%= minlat%> to <%= maxlat%> </td>

<td align="center"><input type="text" size="10" name="minlat"/></td>

<td align="center"><input type="text" size="10" name="maxlat"/></td>

</tr>

<tr>

<td>

Longitude (degrees)</td>

<td><%=minlon%> to <%= maxlon%> </td>

<td align="center"><input type="text" size="10" name="minlon"/></td>

<td align="center"><input type="text" size="10" name="maxlon"/></td>

</tr>



<tr><td>Date</td>

<td><%= startday%>/<%= startmonth%>/<%=startyear%> - <%= endday%>/<%=endmonth%>/<%= endyear%></td>

<td align="center">

<select name="StartDay">

<%



	for(int i = 1;i<32;i++){

		out.println("<option value='"+i+"'>"+i+"</option>");

		}

		%>

	

</select>

<select name="StartMonth">

<%

	

		for(int i = 1;i<13;i++){

			out.println("<option value='"+i+"'>"+i+"</option>");

			}

		%>



</select>

<select name="StartYear">

<%

        for(int i = 1996;i < 2004; i++){

            out.println("<option value='"+i+"'>"+i+"</option>");

            }

%>

</select>



</td><td align="center">

<select name="EndDay">

<%



	for(int i = 1;i<32;i++){

		out.println("<option value='"+i+"'>"+i+"</option>");

		}

		%>

	</select>

<select name="EndMonth">

<%

	

		for(int i = 1;i<13;i++){

			out.println("<option value='"+i+"'>"+i+"</option>");

			}

		%></select>

<select name="EndYear">

 <%

            for(int i = 1996;i < 2004; i++){

                  out.println("<option value='"+i+"'>"+i+"</option>");

            }

%>

</select>



</td>



</tr>

</table>



<br />

<table>





<tr><td colspan="2">Process to work out the :&nbsp;&nbsp;<select name="type">

<option value="minimum">Minimum</option>

<option value="maximum">Maximum</option>

<option value="average">Average</option>

</select>&nbsp;<select name="typeOfQuery">

<option value="PARAM_130">Temperature</option>

<option value="PARAM_129">Geopotential</option>

<option value="PARAM_131">U-velocity</option>

<option value="PARAM_132">V-velocity</option>



<!--<option value="Vertical velocity">Vertical velocity</option>-->

<option value="PARAM_157">Relative Humidity</option></select>

</td></tr>

<tr>

<td><input type="submit" value="Process" /></td>

</tr></table>

</form>

<form action="../jsp/Manipulate3WS.jsp" name="country" method="post" onsubmit="return checknone();">



<table>

<tr>



<td>Or use a countries latitude and longitude&nbsp;<select name="countries">

<option value="none"></option>

    

<% StreamTokenizer st = null;

      try {

           

           

            File newFile = new File(wd+File.separator+"xml"+File.separator+"locations.txt");

            FileReader read  = new FileReader(newFile);

            st = new StreamTokenizer(read);

            st.wordChars(32,32);

               for(int i = 0;i< 3;i++){

                    st.nextToken();

                    }

               

                

                while(st.nextToken() != StreamTokenizer.TT_EOF){

                    //if(st.nextToken() ==st.sval){

                    

                    out.println("<option value=\""+st.sval+"\">"+st.sval+"</option>");

                    for(int i = 0;i< 12;i++){

                    st.nextToken();

                    }

                 }

          

            }

   catch(Exception e){

logger.warn("Exception reading locations file",e);

}

%>



</select>&nbsp;&nbsp;<input type="submit" value="Use a country"/></td>



</tr>

</table>

</form>

<br />

<b>Answer:</b>

<br />

<hr />

<p class="footer" align="right"><a href="http://validator.w3.org/check/referer"><img

         src="../img/vxhtml.gif"

        alt="Valid XHTML 1.0!"  border="0" height="31" width="88" /></a>

<br />

<br /><a href="http://www.clrc.ac.uk/Activity/ACTIVITY=Copyright;" target="_top">&copy;  2003 CLRC DataPortal</a></p>

</td></tr>



</table>

</body>

</html>
