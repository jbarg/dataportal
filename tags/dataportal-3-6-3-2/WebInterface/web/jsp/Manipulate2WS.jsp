<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">



<%@ page errorPage="error.jsp"  import="org.apache.log4j.*,java.util.*, java.net.*,java.io.*,javax.xml.transform.*,org.jdom.*,org.jdom.input.*,org.apache.axis.client.*,javax.xml.rpc.ParameterMode,org.apache.axis.encoding.XMLType" %>



<%@ include file="loggedin.jsp" %>
<%@ taglib uri="/tldweb" prefix="xtags" %>
<%



    	//set static log for the class

	 Logger logger = Logger.getLogger("Manipulate2.jsp");

	    //locate the prop file.  Normal get this from web.xml file
            String wd =  (String)session.getAttribute("wd");
            


	//get the inforation from last page

        

	String min_alt = request.getParameter("minalt");

	String max_alt = request.getParameter("maxalt");

	

        String deg_min_lat =  request.getParameter("minlat");

        String deg_max_lat =  request.getParameter("maxlat");

        String deg_min_lon =  request.getParameter("minlon");

        String deg_max_lon =  request.getParameter("maxlon");



	String min_lat = String.valueOf(1000*Float.parseFloat(deg_min_lat));

	String max_lat = String.valueOf(1000*Float.parseFloat(deg_max_lat));

	

	String min_lon = String.valueOf(1000*Float.parseFloat(deg_min_lon));

	String max_lon = String.valueOf(1000*Float.parseFloat(deg_max_lon));

	

	String type = request.getParameter("type");

	

        String typeOfQuery  = request.getParameter("typeOfQuery");

        String units = null;

        String requested = null;

        if(typeOfQuery.equals("PARAM_130")) {

            units = "K";

            requested = "Temperature";

        }

        if(typeOfQuery.equals("PARAM_129")){

              units = "m<sup>2</sup> s<sup>-2</sup>";

              requested = "geopotential";

              }

        if(typeOfQuery.equals("PARAM_131")){

                units = "m s<sup>-1</sup>";

                requested = "U - velocity";

}

        if(typeOfQuery.equals("PARAM_132")) {

                units = "m s<sup>-1</sup>";

                requested = "V - velocity";

                }

        if(typeOfQuery.equals("PARAM_157")) {

                units = "%";

                requested = "Relative humidity";

}



        String startday_sel = request.getParameter("StartDay");

        String startmonth_sel = request.getParameter("StartMonth");

        String startyear_sel = request.getParameter("StartYear");

    submit

        String endday_sel = request.getParameter("EndDay");

        String endmonth_sel = request.getParameter("EndMonth");

        String endyear_sel = request.getParameter("EndYear");

	//String param = (String)session.getAttribute("param");

	String val = null;

 try{ 

            Properties prop = (Properties)session.getAttribute("props");

            String url2 = prop.getProperty("CART");

            String url3  = prop.getProperty("RASGRIB");

            //normal long winded service call

            String endpoint = url3;

            Service  service = new Service();

            Call     call    = (Call) service.createCall();



            call.setTargetEndpointAddress( new java.net.URL(endpoint) );

            call.setOperationName( "init" );

            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op2", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op3", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op4", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op5", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op6", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op7", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op8", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op9", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op10", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op11", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op12", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op13", XMLType.XSD_STRING, ParameterMode.IN );

            call.addParameter( "op14", XMLType.XSD_STRING, ParameterMode.IN );

            call.setReturnType( XMLType.XSD_STRING );







            //old way using invoke

            Object[] ob = new Object[]{max_lat,min_lat,max_lon,min_lon,max_alt,min_alt,startyear_sel,endyear_sel,startmonth_sel, endmonth_sel,startday_sel,endday_sel,typeOfQuery,type};

            val = (String) call.invoke(ob );

            //System.out.println(ret);





           

              

        } catch (Exception e) {

            val = e.getMessage();
            logger.warn("could not call ras grib",e);
           // e.printStackTrace();

        }





	/*URL urlt = new URL("http://twister.badc.rl.ac.uk:8080/examples/RasGribServlet?min_lat="+min_lat+"&max_lat="+max_lat+"&min_lon="+min_lon+"&max_lon="+max_lon+"&min_alt="+min_alt+"&max_alt="+max_alt+"&min_year="+startyear_sel+"&max_year="+endyear_sel+"&min_month="+startmonth_sel+"&max_month="+endmonth_sel+"&min_day="+startday_sel+"&max_day="+endday_sel+"&operation_mode="+type+"&ras_collection="+typeOfQuery+"");

	logger.warn(urlt.toString());

	URLConnection conn = urlt.openConnection();

	InputStream input = conn.getInputStream();

	/*BufferedReader in = new BufferedReader(new InputStreamReader(input));

			String inputLine;

					

	

			while ((inputLine = in.readLine()) != null){

			      System.out.println(inputLine);

	

		}*/

	/*org.w3c.dom.Document document= null;

        

	try{

		//build document from the input stream

		javax.xml.parsers.DocumentBuilderFactory dbf = javax.xml.parsers.DocumentBuilderFactory.newInstance();

		javax.xml.parsers.DocumentBuilder db = dbf.newDocumentBuilder();

		document = db.parse(input);

                org.w3c.dom.NodeList Nodelist = document.getElementsByTagName("body");

        	org.w3c.dom.Element element = (org.w3c.dom.Element)Nodelist.item(0);

                val = element.getFirstChild().getNodeValue();

	}

	catch(Exception e){

	logger.warn("Returned file from RasGrib is thrown a error",e);

		

                val  = "<b>-&nbsp / &nbsp  Unable to process your request, change the values and try again</b>";

                units  = "";

	}*/

	

	//System.out.println(val);

	%>

	



<%

	

     String url = (String)session.getAttribute("urlbadc");

        //get all the info from the shopping cart

       // String url = (String)session.getAttribute("urlbadc");

       

    String name =(String)session.getAttribute("sessionid");

       DOMBuilder builder = new DOMBuilder();

       Document doc = builder.build(new File(wd+File.separator+"profiles"+File.separator+"cera"+name+".xml"));



       Element el = doc.getRootElement();

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

<title>CCLRC Data Portal - Manipulate</title>

<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->

<style type="text/css"> 

@import url(../style/mainmenu.css); /*IE and NN6x styles*/

</style>

<script type="text/javascript">

<!--

function openwindow(page){

   //window.open("../help/Help.html","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
    // window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");

 window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");


    //window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");

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

message += " \n  Start year must be less than end year\n";

}

if(startyearvalue == endyearvalue){

   

    if(startmonthvalue > endmonthvalue){



        message += "\n your dates conflict with each others \n";

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

<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> 

<style type="text/css"> 

@import url(../style/main.css); /*IE and NN6x styles*/

</style>

</head>

<body background="../img/redbg.gif">

<table width="100%" cellspacing="3" border="0"><tr>

<td valign="top" width="20%" align="left">

<!--<a href="http://www.clrc.ac.uk"><img border="0" src="../img/clrc.gif" alt="CLRC logo" /></a>-->
<br />&nbsp;<br /><br />
<!-- out puts the content if the browser is not netsacpe 4.  If so out puts

html/netscape.html file from the content root.-->

<xtags:menu netscapefile="html/netscape.html" iefile="html/ie.html">

</xtags:menu>


</td><td valign="top">

<h1>Results</h1>



<hr />





<form action="../jsp/Manipulate2WS.jsp" method="post"  onsubmit="return check();" name="manipulate">

<table border="1" width="100%">

<tr  bgcolor="C0C0C0">

<td align="center" width="16%">&nbsp;</td>

<td align="center" width="16%">Range</td>

<td align="center"  width="23%">Min</td>



<td align="center" width="23%">Max</td>

</tr>

<tr>

<td>

Altitude (hectoPascals)</td>

<td><%= minalt%> to <%= maxalt%> </td>

<td align="center"><input type="text" size="10" name="minalt" value="<%=min_alt%>"/></td>

<td align="center"><input type="text" size="10" name="maxalt" value="<%=max_alt%>"/></td>

</tr>

<tr>

<td>

Latitude (degrees)</td>

<td><%= minlat%> to <%= maxlat%> </td>

<td align="center"><input type="text" size="10" name="minlat" value="<%=deg_min_lat%>"/></td>

<td align="center"><input type="text" size="10" name="maxlat" value="<%=deg_max_lat%>"/></td>

</tr>

<tr>

<td>

Longitude (degrees)</td>

<td><%=minlon%> to <%= maxlon%> </td>

<td align="center"><input type="text" size="10" name="minlon" value="<%=deg_min_lon%>"/></td>

<td align="center"><input type="text" size="10" name="maxlon" value="<%=deg_max_lon%>"/></td>

</tr>

<tr><td>Date</td>

<td><%= startday%>/<%= startmonth%>/<%=startyear%> - <%= endday%>/<%=endmonth%>/<%= endyear%></td>

<td align="center">



<select name="StartDay">

<%



	for(int i = 1;i<32;i++){

                String s = String.valueOf(i);

                if(startday_sel.equals(s)){

		out.println("<option selected='selected' value='"+i+"'>"+i+"</option>");

		}

else out.println("<option  value='"+i+"'>"+i+"</option>");

}

		%>

	

</select>

<select name="StartMonth">

<%

	

		for(int i = 1;i<13;i++){

			        String s = String.valueOf(i);

                       if(startmonth_sel.equals(s)){

                        	out.println("<option selected='selected' value='"+i+"'>"+i+"</option>");

                        	}

                        else out.println("<option  value='"+i+"'>"+i+"</option>");

                        }

		%>



</select>

<select name="StartYear">

<%

	

		for(int i = 1996;i<2002;i++){

			        String s = String.valueOf(i);

                       if(startyear_sel.equals(s)){

                        	out.println("<option selected='selected' value='"+i+"'>"+i+"</option>");

                        	}

                        else out.println("<option  value='"+i+"'>"+i+"</option>");

                        }

		%>

</select>

</td><td align="center">

<select name="EndDay">

<%



	for(int i = 1;i<32;i++){

		        String s = String.valueOf(i);

                       if(endday_sel.equals(s)){

                        	out.println("<option selected='selected' value='"+i+"'>"+i+"</option>");

                        	}

                        else out.println("<option  value='"+i+"'>"+i+"</option>");

                        }

		%>

	</select>

<select name="EndMonth">

<%

	

		for(int i = 1;i<13;i++){

        String s = String.valueOf(i);

                       if(endmonth_sel.equals(s)){

                        	out.println("<option selected='selected' value='"+i+"'>"+i+"</option>");

                        	}

                        else out.println("<option  value='"+i+"'>"+i+"</option>");

                        }

		%></select>

<select name="EndYear"><%

	

		for(int i = 1996;i<2002;i++){

                        String s = String.valueOf(i);

                       if(endyear_sel.equals(s)){

                        	out.println("<option selected='selected' value='"+i+"'>"+i+"</option>");

                        	}

                        else out.println("<option  value='"+i+"'>"+i+"</option>");

                        }

		%></select>

</td></tr>





</table>



<br />

<table>





<tr><td colspan="2">Process to work out the :&nbsp;&nbsp;<select name="type">

<%

        String array[]  = {"minimum","maximum","average"};

        for(int i  = 0; i< array.length;i++){

        //String ts = String.valueOf(i);

            if(type.equals(array[i])){

            out.println("<option selected='selected'  value='"+array[i]+"'>"+array[i]+"</option>");

                }

else out.print("<option value='"+array[i]+"'>"+array[i]+"</option>");

}

%>





</select>&nbsp;<select name="typeOfQuery">

<option value="PARAM_130">Temperature</option>

<option value="PARAM_129">Geopotential</option>

<option value="PARAM_131">U-velocity</option>

<option value="PARAM_132">V-velocity</option>

<option value="PARAM_157">Relative Humidity</option>

<!--<option value="Vertical velocity">Vertical velocity</option>-->

</select></td></tr>

<tr>

<td><input type="submit" value="Process" /></td>

</tr>

</table>



</form>

<form action="Manipulate3WS.jsp" name="country" method="post" onsubmit="return checknone();">

<table>

<tr><td>Or use a countries latitude and longitude&nbsp;<select name="countries">

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

logger.warn("Unable to reas locations.txt file",e);

}

%>



</select>&nbsp;&nbsp;<input type="submit" value="Use a country"/></td></tr>





</table>

</form>

<br />

<b>Answer:</b>&nbsp;&nbsp;&nbsp;&nbsp;<%=type%> value of <%= requested %> is <%=val%> &nbsp;<%= units%>

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


