<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@page contentType="text/html" import="java.io.*"%>
 errorPage="error.jsp"

<%
        //set up initial variables for badc
        String discipline = null;

//remove attributes that were set from the basic search
                session.removeAttribute("noCon");
                session.removeAttribute("noRes");
                session.removeAttribute("fac");
     
        boolean button = false;
        String currentlevel ="";    
        String currentlevelSupplied = "earth sciences"; 
        String slevel = null;
        int level = 0;
        String name = null;
        String newlevel = null;
        int last = 0;
        String[] level1 = {"earth sciences"};
  
        String[] level11 = {"atmosphere","oceans","Back"};
   
        String[] level111 = {"atmospheric temperature", "water vapour", "atmospheric winds","Back"};
        String[] level112 = {"ocean circulation","Back"};

        String[] level1111 = {"temperature","Back"};
        String[] level1112 ={"relative humidity","Back"};
        String[] level1113 = {"vertical velocity","Back"};
        
        String[] level1121 = {"u-velocity", "v- velocity","Back"};


        //set out levels for final search
        String level11111 = "earth sciences/atmosphere/atmospheric temperature/temperature";


        //set up initial variables for badc
        String silevel = null;
        int ilevel = 0;
        String iname = null;

        String[] ilevel1 = {"biology","chemistry"};
        String[] ilevel11 = {"biochemistry","Back"};
        String[] ilevel12 = {"biochemistry","Back"};
        String[] ilevel111 = {"inorganic chemistry","organic chemistry","comptational chemistry","Back"};
         String[] ilevel121 = {"inorganic chemistry","organic chemistry","comptational chemistry","Back"};
      

        //check which facility
        String fac = request.getParameter("facility");
        if(fac.equals("BADC")){

            slevel = request.getParameter("level");
                    
            level = Integer.parseInt(slevel);
    
           name = request.getParameter("value");
          
            
            if(name == null){    
                newlevel = "1";
            }
            else  if(name.equals("Back")){
                last = slevel.length()-1;
                newlevel = slevel.substring(0,last);
              
            }
            else{
                newlevel = level+name;
            }
          
     
          
            }
       else if(fac.equals("SR") || fac.equals("ISIS")){
                
                silevel = request.getParameter("level");
                ilevel = Integer.parseInt(silevel);
                iname = request.getParameter("value");
                if(iname == null){    
                    newlevel = "1";
                }
                else  if(iname.equals("Back")){
                    last = silevel.length()-1;
                    newlevel = silevel.substring(0,last);
                   
                }
                else{
                    newlevel = ilevel+iname;
                }
            
}

            
        %>

<html>
<head>
<title>CLRC Data Portal/Estedi - Basic database search</title>
<link rel="stylesheet" href="../style/mainNN4frame.css" type="text/css" /> 
<style type="text/css"> 
@import url(../style/mainframe.css); /*IE and NN6x styles*/
</style>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

<script type="text/javascript">
<!--
var lastlevel = <%=newlevel%>;
var facility = "<%=fac%>";

function checking(){

 if(document.study.StudyName.value.length < 1){
        alert("Please enter a value in the Study Name");
        return false;
    }
else return true;
}


function check(){
var index = document.glen.testbed.selectedIndex;
var value1=document.glen.testbed.options[index].value;




 //alert("frontpage.jsp?level="+lastlevel +"&value=" + value1+ "&fac=BADC");
window.location.href = "AdvancedSearch1.jsp?level="+ lastlevel+ "&value=" + value1 + "&facility=" + facility;
}

function change(){

    var index = document.glen.facility.selectedIndex;
var fac =  document.glen.facility.options[index].value;


    if(fac == "BADC"){
        window.location.href="AdvancedSearch1.jsp?level=1&facility=BADC";
    }
    else if(fac == "SR"){
        //alert("AdvancedSearch.jsp?level=1&facility=SR");
        window.location.href="AdvancedSearch1.jsp?level=1&facility=SR";

    }
     else if(fac == "ISIS"){
        window.location.href="AdvancedSearch1.jsp?level=1&facility=ISIS";
    }
}

function submitting(){
     var fac = document.glen.facility.value;
var level = document.glen.level.value;

//get start and end dates
var startdayi = document.glen.StartDay.selectedIndex;
var startday = document.glen.StartDay.options[startdayi].value;

var startmonthi = document.glen.StartMonth.selectedIndex;
var startmonth = document.glen.StartMonth.options[startmonthi].value;

var startyeari = document.glen.StartYear.selectedIndex;
var startyear = document.glen.StartYear.options[startyeari].value;

var StartDate = ""+startyear + startmonth + startday+"";

var enddayi = document.glen.EndDay.selectedIndex;
var endday = document.glen.EndDay.options[enddayi].value;

var endmonthi = document.glen.EndMonth.selectedIndex;
var endmonth = document.glen.EndMonth.options[endmonthi].value;

var endyeari = document.glen.EndYear.selectedIndex;
var endyear = document.glen.EndYear.options[endyeari].value;

var EndDate = ""+endyear + endmonth + endday+"";

//alert("./servlet/AdvancedSearchServlet?Discipline=" + level + "&StartDate="+StartDate+"&EndDate="+EndDate);
parent.window.location.href="./servlet/AdvancedSearchServlet?Discipline=" + level + "&StartDate="+StartDate+"&EndDate="+EndDate+"&DMInstitutionName="+facility;
}
//-->

</script>




</head>
<body>
<h1 align="right">Advanced Search</h1>
<hr />
<form name="glen" action="" method="post">
<table border="0">
<tr><td width="130">Facility </td>
<td width="180"><select name="facility" onchange="change();" >
<option value="nothing"></option>
<option value="BADC">British Atmospheric Data Centre (BADC)</option>
<option value="SR">Synchrotron Radiation (SR)</option>
<option value="ISIS">Neutron Spallation Source (ISIS)</option>
</select></td>
<td width="200">Chosen facility : &nbsp;<%=fac%></td></tr>
<tr><td></td>
<td />
<td />

</tr>
<tr><td valign="top">
<p>Discipline </p></td>
 <td>

<select name="testbed" onchange="check();" size="5">
<%
int newlevelint = Integer.parseInt(newlevel);


if(fac.equals("BADC")){
    switch (newlevelint){
        case 1:
            for(int i =0; i< level1.length;i++){
                out.println("<option value='"+(i+1)+"'>"+level1[i]+"</option>");
            }
             currentlevelSupplied = "earth%20sciences";
            break;
            
        case 11:
            currentlevel = "earth sciences";
            for(int i =0; i< level11.length;i++){
                if(level11[i].equals("Back")){
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.println("<option value='"+(i+1)+"'>"+level11[i]+"</option>");
                }
            }
             currentlevelSupplied = "earth%20sciences";
            break;
            
        case 111:
            currentlevel = "earth sciences/atmosphere";
            for(int i =0; i< level111.length;i++){
                if(level111[i].equals("Back")){
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.println("<option value='"+(i+1)+"'>"+level111[i]+"</option>");
                }
            }
             currentlevelSupplied = "earth%20sciences/atmosphere";
            break;
            
        case 112:
            currentlevel = "earth sciences/oceans";
            for(int i =0; i< level112.length;i++){
                if(level112[i].equals("Back")){
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.println("<option value='"+(i+1)+"'>"+level112[i]+"</option>");
                }
            }
             currentlevelSupplied = "earth%20sciences/oceans";
            break;
            
        case 1111:
            currentlevel = "earth sciences/atmosphere/atmospheric temperature ";
            for(int i =0; i< level1111.length;i++){
                if(level1111[i].equals("Back")){
                    
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.println("<option value='"+(i+1)+"'>"+level1111[i]+"</option>");
                }
            }
             currentlevelSupplied = "earth%20sciences/atmosphere/atmospheric%20temperature";
            break;
            
        case 1112:
            currentlevel = "earth sciences/atmosphere/relative humidity";
            for(int i =0; i< level1112.length;i++){
                if(level1112[i].equals("Back")){
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.println("<option value='"+(i+1)+"'>"+level1112[i]+"</option>");
                }
            }
             currentlevelSupplied = "earth%20sciences/atmosphere/relative%20humidity";
            break;
            
        case 1113:
            for(int i =0; i< level1113.length;i++){
                currentlevel = "earth sciences/atmosphere/atmospheric winds";
                if(level1113[i].equals("Back")){
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.println("<option value='"+(i+1)+"'>"+level1113[i]+"</option>");
                }
            }
             currentlevelSupplied = "earth%20sciences/atmosphere/atmospheric%20winds";
            break;
            
        case 1121:
            currentlevel = "earth sciences/oceans/ocean circulation";
            for(int i =0; i< level1121.length;i++){
                if(level1121[i].equals("Back")){
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.println("<option value='"+(i+1)+"'>"+level1121[i]+"</option>");
                }
            }
             currentlevelSupplied = "earth%20sciences/atmosphere/ocean%20circulation";
            break;
            
        case 11111:
            currentlevel = "earth sciences/atmosphere/atmospheric temperature/temperature";
            currentlevelSupplied = "earth%20sciences/atmosphere/atmospheric%20temperature/temperature";

            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            
            break;
            
        case 11121:
            currentlevel = "earth sciences/atmosphere/water vapour/relative humidity";
            currentlevelSupplied = "earth%20sciences/atmosphere/water%20vapour/relative%20humidity";

            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;
            
        case 11131:
            currentlevel = "earth sciences/atmosphere/atmospheric winds/vertical velocity";
            currentlevelSupplied = "earth%20sciences/atmosphere/atmospheric%20winds/vertical%20velocity";

            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;
            
        case 11211:
            currentlevel = "earth sciences/oceans/ocean circulation/u-velocity";
            currentlevelSupplied = "earth%20sciences/oceans/ocean%20circulation/u-velocity";

            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;
            
        case 11212:
            
            currentlevel = "earth sciences/oceans/ocean circulation/v-velocity";
            currentlevelSupplied = "earth%20sciences/oceans/ocean%20circulation/v-velocity";
            
            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;
            
    }//end of switch
}//end of if

else if(fac.equals("ISIS") || fac.equals("SR")){
    
    switch (newlevelint) {
        
        case 1:
            for(int i =0; i< ilevel1.length;i++){
                out.println("<option value='"+(i+1)+"'>"+ilevel1[i]+"</option>");
            }
             currentlevelSupplied = "biology";
            break;
            
        case 11:
            currentlevel = "biology";
            for(int i =0; i< ilevel11.length;i++){
                if(ilevel11[i].equals("Back")){
                    
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.println("<option value='"+(i+1)+"'>"+ilevel11[i]+"</option>");
                }
            }
             currentlevelSupplied = "biology";
            break;
            
        case 12:
            currentlevel = "chemistry";
            for(int i =0; i< ilevel12.length;i++){
                if(ilevel12[i].equals("Back")){
                    
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.println("<option value='"+(i+1)+"'>"+ilevel12[i]+"</option>");
                }
            }
             currentlevelSupplied = "chemistry";
            break;
            
        case 111:
            currentlevel = "biology/biochemistry";
            for(int i = 0;i<ilevel111.length; i++){
                if(ilevel111[i].equals("Back")){
                    
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.print("<option value='" +(i+1)+"'>"+ilevel111[i] + " </option>");
                }
            }
             currentlevelSupplied = "biology/biochemistry";
            break;
    
        case 121:
            currentlevel = "chemistry/biochemistry";
            for(int i = 0;i<ilevel121.length; i++){
                if(ilevel121[i].equals("Back")){
                    
                    out.println("<option value='Back'> --- Back --- </option>");
                }
                else{
                    out.print("<option value='" +(i+1)+"'>"+ilevel121[i] + " </option>");
                }
            }
              currentlevelSupplied = "chemistry/biochemistry";
            break;
        
        case 1111:
            currentlevel = "biology/biochemistry/inorganic chemistry";
            currentlevelSupplied = "biology/biochemistry/inorganic%20chemistry";

            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;
            
        case 1112:
            currentlevel = "biology/biochemistry/organic chemistry";
            currentlevelSupplied = "biology/biochemistry/organic%20chemistry";

            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;
            
        case 1113:
            currentlevel = "biology/biochemistry/computational chemistry";
            currentlevelSupplied = "biology/biochemistry/computational%20chemistry";

            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;

        case 1211:
            currentlevel = "chemistry/biochemistry/inorganic chemistry";
            currentlevelSupplied = "chemistry/biochemistry/inorganic%20chemistry";

            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;
            
        case 1212:
            currentlevel = "chemistry/biochemistry/oragnic chemistry";
            currentlevelSupplied = "chemistry/biochemistry/oragnic%20chemistry" ;

            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;
            
        case 1213:
            currentlevel = "chemistry/biochemistry/computational chemistry";
            currentlevelSupplied = "chemistry/biochemistry/computational%20chemistry";
            out.println("<option value='Back'> --- Back --- </option>");
            button = true;
            break;
    }//end of switch
}// end of if
   
 
/* here if new level is the something like level11111, see above, 
then response.sendRedirect(../servlet/sinpleSearch?where=Diciple="level11111"  
etc pull out the anwsers*/

%>
</select>


</td>
<td>Chosen discipline : &nbsp;<%=currentlevel%></td>
</tr>



</table>
<table border="0">

<tr>
<td width="130">Person</td>

<td>
<input type="text" name="Person" value="" size="20" /> <small>(Not implemented yet)</small>
</td>
</tr>
<tr>
<td width="60">Location</td>

<td valign="top">
<select name="Locations">
<%
    String wd = (String)session.getAttribute("wd");
    FileReader file = new FileReader(wd+File.separator+"xml"+File.separator+"countries.txt");
    try{
        BufferedReader br  = new BufferedReader(file);
        String sr = "";
        while((sr = br.readLine()) != null){
            out.println("<option value='"+sr +"'>"+ sr +"</option>");
            }
}   
catch(Exception e){}
%>
        

</select> <small>(No data yet)</small>
</td>
</tr>
<tr>
<td width="60" nowrap="nowrap">Period</td>

<td valign="top">
<select name="StartDay">
<%
 
        for(int i = 1;i<32;i++){
            if(i < 10){
                out.println("<option value='0"+i+"'>"+i+"</option>");
            }
else  out.println("<option value='"+i+"'>"+i+"</option>");
}
    
%>


</select>
<select name="StartMonth">
<%
        String[] months={"","January", "February", "March", "April","May","June", "July","August","September","October","November","December"};
        String[ ]mon = {"","01","02","03","04","05","06","07","08","09","10","11","12"};
        for(int i = 1;i<months.length;i++){
            out.println("<option value='"+mon[i]+"'>"+months[i]+"</option>");
    }
    
%>


</select>
<select name="StartYear">
<%
        for(int i = 1985;i<2004;i++){
            if(i == 2002){
            out.println("<option selected='selected' value='"+i+"'>"+i+"</option>");
            }
            else{ out.println("<option value='"+i+"'>"+i+"</option>");
}
    }
    
%>
</select>
 to
<select name="EndDay">

<%
       for(int i = 1;i<32;i++){
            if(i < 10){
                out.println("<option value='0"+i+"'>"+i+"</option>");
            }
else  out.println("<option value='"+i+"'>"+i+"</option>");
}
    
%>
</select>
<select name="EndMonth">
<%
        //String[] months={"January", "February", "March", "April","May","June", "July","August","September","October","November","December"};
        for(int i = 1;i<months.length;i++){
            out.println("<option value='"+mon[i]+"'>"+months[i]+"</option>");
    }
    
%>
</select>
<select name="EndYear">
<%
         for(int i = 1985;i<2004;i++){
            if(i == 2002){
            out.println("<option selected='selected' value='"+i+"'>"+i+"</option>");
            }
            else{ out.println("<option value='"+i+"'>"+i+"</option>");
}
    }
    
%></select>
</td>
</tr>
<tr>
<td width="60"  valign="top" nowrap="nowrap">&nbsp;</td>

<td align="left" valign="top">
<% out.println("<input type='hidden' name='level' value='"+currentlevelSupplied + "' />");
%>
<input type='button' onclick='submitting();'  value='Submit' /></td>
</tr>
</table>
</form>
<form method="post" action="../servlet/AdvancedStudyServlet" onsubmit="return checking();" name="study" target="_top">
<table  border="0">
<tr>
<td width="130">Study Name</td>

<td>
<input type="text" name="StudyName" size="25" />
</td>
</tr>
<tr><td width="60" align="right" valign="top" nowrap="nowrap">&nbsp;</td>
<td><input type="submit" value="Submit"/></td></tr>
</table>
</form>
<hr />
<p class="footer" align="right"><a href="http://validator.w3.org/check/referer"><img
         src="../img/vxhtml.gif"
        alt="Valid XHTML 1.0!"  border="0"  height="31" width="88" /></a>
<br />
<br />&nbsp;&nbsp;<a href="http://www.clrc.ac.uk/Activity/ACTIVITY=Copyright;" target="_top">&copy;  2003 CLRC DataPortal</a></p>

</body>
</html>

