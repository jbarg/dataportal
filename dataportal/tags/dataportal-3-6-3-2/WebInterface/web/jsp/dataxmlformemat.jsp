<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<!--   this si the proper one  -->


<%@ page errorPage="frameerror.jsp"  import="uk.ac.dl.xml.*,org.apache.log4j.*,java.util.*,java.io.*,org.w3c.dom.*,org.xml.sax.*,javax.xml.transform.dom.*,uk.ac.dl.dn.*" %>
<%@ taglib uri="/tldweb" prefix="xtags" %>
 <%@ taglib uri="/tld/taglibs-xtags.tld" prefix="xtags1" %>
 <%@ include file="loggedin.jsp" %>
  <%@ include file="logger.jsp" %>
<html>

<head>
<title>CCLRC Data Portal - Basic database search</title>
<link rel="stylesheet" href="../style/mainNN4frame.css" type="text/css" /> 

<style type="text/css"> 
@import url(../style/mainframe.css); /*IE and NN6x styles*/
</style>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<% 

        
        String sid = (String)session.getAttribute("sessionid");
        String dn = Convert.removeSpaces((String)session.getAttribute("dn"));
        String[] list2 = request.getParameterValues("list");
        //get level and value from request
	String value = request.getParameter("value");  
      
	Document  doc = null;
doc = (Document)session.getAttribute("xml");
        
        String fileName = "_"+dn+".xml";


         if(value == null){}
         else{
        int n2 = value.length();

        StringBuffer h1 = new StringBuffer();
        for(int i = 0; i<n2; i++){
            if( value.charAt(i) == ' ') h1.append(" ");                    

        else h1.append(value.charAt(i));
        }
        value = h1.toString();
        }
	String level = request.getParameter("level");
        //initialise variables
	String facs = "";
	String final1 = "";
       
	if(level == null){
       		//not really needed
		session.setAttribute("fac", list2);
        	//oringinal starting points for the xml document
		level = "0";
		value = "start";
               // System.out.println("value of list2 is "+list2[0]);
                //System.out.println(list2.length);
                if(list2[0].equals("No_Access")) {
                    //System.out.println("value of list2 is "+list2);
                }
                else{
                File[] filename = new File [(list2.length)];
                                 if(list2.length == 1) {
                   //System.out.print("trying to parse "+wd+File.separator+"profiles"+File.separator+conf.getProperty(list2[0]).trim()+sid);
                    doc = XML_DOMBuilder.parse(new File(wd+File.separator+"profiles"+File.separator+list2[0].trim().toLowerCase()+fileName));
                 }
                 else{
                     for(int i = 0 ; i< list2.length;i++){
                        filename[i] = new File(wd+File.separator+"profiles"+File.separator+File.separator+list2[i].trim().toLowerCase()+fileName);
                      }
                     doc = Combiner.build(filename,"active");
                 }   
                 session.setAttribute("xml",doc);
                 org.jdom.Document docjdom = Converter.DOMtoJDOM(doc);
                 org.jdom.Element root = docjdom.getRootElement();
                 root.setAttribute("value","start");
                 doc = Converter.JDOMtoDOM(docjdom);
               }
}
        //get int value of the level
	int formlevel = 0;
        formlevel = Integer.parseInt(level);
	if(level.equals("0"))formlevel = 0;
%>

<script type="text/javascript">

<!--

var level = <%=level%>+1;
var list = "<%=list2[0]%>";

function go1(){

	var typeindex = document.type_of_Study.type.selectedIndex;
	var type = document.type_of_Study.type.options[typeindex].value;

var value1 = document.for2.hidden.value
 var max_wait = document.waiting.max_wait.value;
var no_max = document.waiting.no_max.checked;
	//alert(value1);

		//window.location.href="response.jsp?value="+value1;
//check value of time is a number
       

  if(no_max){
	   if(max_wait == "") max_wait = "15";
             parent.window.location.href="../servlet/BasicSearch?Discipline=" + value1 + "&DMInstitutionName=" + list + "&max_wait="+max_wait+"&no_max="+no_max +"&type="+type;
        }
        else if(max_wait>=0.1){
           // alert(type);
            parent.window.location.href="../servlet/BasicSearch?Discipline=" + value1 + "&DMInstitutionName=" + list + "&max_wait="+max_wait+"&no_max="+no_max +"&type="+type;
        }
                else {
            alert("Please insert maximum waiting time value");
            document.waiting.max_wait.value = "";
            document.waiting.max_wait.focus();
        }

}

function go(){

	var index = document.for1.test.selectedIndex;
	var value = document.for1.test.options[index].value;
	var typeindex = document.type_of_Study.type.selectedIndex;
	var type = document.type_of_Study.type.options[typeindex].value;

	if(value == "back"){
		history.back(-1);
	}

	else if(value == "submit"){
	var value1 = document.for2.hidden.value
        var max_wait = document.waiting.max_wait.value;
	alert(value1);
		window.location.href="response.jsp?value="+value1+"&max_wait="+max_wait;	
	}
	else {
        var max_wait = document.waiting.max_wait.value;
        var no_max = document.waiting.no_max.checked;
	//alert("xmlform.jsp?level="+level+"&value="+value+"&list="+list);        

//alert(type);
	window.location.href="dataxmlformemat.jsp?level="+level+"&value="+value+"&list="+list+"&max_wait="+max_wait+"&no_max="+no_max+"&checked="+type;
	}
}

function change(){
 var index = window.document.facilities.list.selectedIndex;

var value = "";
 //alert(index);
if(index == -1){
    alert("Please select one or more facilities");
    return false
}
else {

value = window.document.facilities.list.options[index].value;
//alert("dataxmlform.jsp?level=0&value=start&list="+value);
//window.location.href="dataxmlformemat.jsp?level=0&value=start&list="+value;
return true;

}


}

function check(){
var checked = "1";

//for(document.facilities.element[i]

}

//-->
</script>

</head>

<body>
&nbsp;Home&nbsp;&nbsp;&nbsp;>&nbsp;<a href="#" >Basic Search</a>
<hr />
<br />
<br />
<table border="0">
<tr><td width="90" valign="top">Facility :</td>
<td width="380">
<form name="facilities"  onsubmit="return change();">
<select name="list" multiple="multiple" size="3">
<%
ArrayList list3 = (ArrayList)session.getAttribute("facs");
if(list3 == null){}
else{
    Iterator e1 = list3.iterator();
    int  i1  = 1;
            while(e1.hasNext()) {
                String name = (String)e1.next();      
          out.println("<option  value=\""+name+"\">"+name+"</option>");
             }
    }
%>
</select>
<input type="submit"  value="Change" />
</form><!--<a href="" onClick=" change();return false;"  value="Change" >ddfds</a>--></td>
<td valign="center" width="*"><u>Chosen facility :</u><br/>
<!-- prints out the list of facs that are in the fac session attribute-->
 <i><xtags:facs name="fac" text="" /></i>

</td>

<tr ><td></td>
<td />
<td />
</tr>
</tr>
<tr><td valign="top">
<p>Discipline :</p></td> <td>
<form name="for1"  action="post">
<select name="test" onchange="go();" size="5">
<%

boolean submit =  false;
ArrayList valuesOut =  new ArrayList();
if(list2[0].equals("No_Access")){}
else{

for(int y = 0; y < 20;y++){
	if(formlevel == y){
		Element e = null;
		Node n = null;

		Element el = doc.getDocumentElement();
		NodeList list = el.getElementsByTagName("level"+(y+1));
		for(int i = 0; i < list.getLength(); i++){
		    n = list.item(i);
		    el = (Element)n.getParentNode();
		    //System.out.println("the value is"+ value);
		    Element o = null;
		    if(el.getAttribute("value").equals(value)){
			 o = (Element)n;
                         String value2 = o.getAttribute("value");
                         //change spaces with _
                        int n1 = value2.length();
                        StringBuffer h = new StringBuffer();
                        for(int i2 = 0; i2<n1; i2++){
                            if( value2.charAt(i2) == ' ') h.append("%20");                   
                        else h.append(value2.charAt(i2));
                        }
                        value2 = h.toString();
                        boolean hadbefore = false;
			 if(o.getAttribute("canSubmit").equals("true")){
                                //do No_Access
                                //revome this bit once the xml has been finalised 
                               submit = true;
			 }
                         //add a vector here keeping tabs on the last values
                         else{
                            for(int j = 0; j < valuesOut.size(); j ++){
                                if(valuesOut.get(j).equals(value2)) {
                                    hadbefore = true;
                                    logger.debug("Values already here is "+valuesOut.get(j));
                                    }
                            }
                            if(!hadbefore) {
                                out.println("<option value='"+value2+"'>"+o.getAttribute("value")+"</option>");
                                valuesOut.add(value2);
                            }
			 }
                        // if(o.getAttribute("canSubmit").equals("true")){
                         //     submt = true;
                        //}   
		    	//if(level.equals("1")) facs = o.getAttribute("fac");
		    	//System.out.println(o.getAttribute("value"));		    	
		    	final1 = o.getAttribute("final");
                        session.setAttribute("final",final1);                      
                       }
	}
		if(!level.equals("0")){

		//out.println("<option value='submit'>-Submit-</option>");
			 out.println("<option value='back'> --Back-- </option>");
		}
		if(list.getLength() == 0){
			final1 = (String)session.getAttribute("final")+value;
		}
           }
	}
}
      //System.out.println("--");  
%>

</select>
</form>
</td>
<td valign="center"><u>Chosen discipline :</u> <br /><i><%=final1%></i></td>
</tr>
<tr>
<td valign="top">Study type :</td>
<td><form name="type_of_Study" action="">
<select  name="type" size="1">
<%

    //find out which was checked before
    String checked1 = (String)request.getParameter("checked");
   //System.out.println("check thing is  "+checked1);
    String isEchecked = "";
    String isSchecked = "";
    String isMchecked = "";
String isAchecked = "";

 if(checked1 == null) isAchecked="selected='selected'";

   else  if(checked1.equals("experiment")){
           isEchecked = "selected='selected'";
           isSchecked = "";
           isMchecked = "";
 isAchecked = "";
    }
   else  if(checked1.equals("simulation")){
           isEchecked = "";
           isSchecked = "selected='selected'";
           isMchecked = "";
  isAchecked = "";
    }

    else if(checked1.equals("measurement")){
           isEchecked = "";
           isSchecked = "";
           isMchecked = "selected='selected'";
           isAchecked = "";
    }

    else if(checked1.equals("all")){
           isEchecked = "";
           isSchecked = "";
           isMchecked = "";
           isAchecked = "selected='selected'";
    }

%>

<option value="all" <%=isAchecked%>>All</option>
<!--<option value="experiment" <%--=isEchecked--%>>Experiment</option>
<option value="simulation" <%--=isSchecked--%>>Simulation</option>
<option  value="measurement" <%--=isMchecked--%>>Measurement</option>-->

</select>
<%
    if(submit == true){
%>
&nbsp;&nbsp;<input type="button" value="Submit" onclick="go1();" />
<% } 
else {}
%>
</form>
</td>
<td>&nbsp;</td>
</tr>
<tr>
<td></td>
<td><form name="waiting" action="" onsubmit="return false;">Maximum waiting time for the search:<br />
<%

    String wait = request.getParameter("max_wait");
    if(wait == null) wait = "15";
  out.println("<input type='text' value='"+wait+"' size='3' name='max_wait' />seconds");

String checked = request.getParameter("no_max");
if(checked == null) checked = "";
else if(checked.equals("false")) checked = "";
else checked = "checked='checked'";
%>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="checkbox" value="no_max" name="no_max" <%=checked%> />No maximum</form>
</td>
<td></td>
</tr>
<tr>


</table>

<form name="for2">
<input name="hidden" type="hidden" value="<%


                String fin = null;
                int n = final1.length();
                StringBuffer h = new StringBuffer();
                for(int i = 0; i<n; i++){
                    if( final1.charAt(i) == ' ') h.append("%20");
                    else h.append(final1.charAt(i));

                }
                fin = h.toString();
                out.print(fin);

%>" />
</form>
<hr />
<%@ include file="../html/footer.html" %>

</body>

</html>
