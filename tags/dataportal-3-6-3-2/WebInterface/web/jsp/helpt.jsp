<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">


<%@ page import="uk.ac.dl.xml.*,java.util.*, java.io.*"%>
<html>
<head>
<title>CLRC Data Portal/Estedi - Data Portal query results</title>
<!--<link rel="stylesheet" href="../style/help.css" type="text/css" /> -->
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<style type="text/css">
span {background-color: yellow}

</style>


</head>
<body>
<%!
String replaceatt(String str, String pattern, String replace){
int s =  0;
        int e = 0;
        
        StringBuffer result = new StringBuffer();
        
        while((e = str.indexOf(pattern,s)) >0){
            int p  = e;
            
            char i  ='f';
            boolean ele_at = false;
            //str.charAt(p-1);
            // System.out.println(i);..
            while(p > 0){
                 i = str.charAt(p-1);
                if(i == '<'){
                    ele_at = true;
                    break;
                }
                  if(i == '>'){
                    ele_at = false;
                    break;
                }
                else p--;
            }
            if(!ele_at){
                result.append(str.substring(s,e));
                result.append(replace);
                s = e + pattern.length();
            }
            else{
                result.append(str.substring(s,e));
                result.append(pattern);
                s = e + pattern.length();
            }
        }
        result.append(str.substring(s));
        return result.toString();
        
        
    }
%>
<%
    String page1 = request.getParameter("page");
    String wd = (String)session.getAttribute("wd");
    String args = (String)session.getAttribute("string");

 try{
        XSLTransformer.transformFiletoFile(new File(wd+File.separator+"help"+File.separator+"xml"+File.separator+page1),new File(wd+File.separator+"xsl"+File.separator+"help.xsl"),new File(wd+File.separator+"help"+File.separator+"xml"+File.separator+page1+".html"));
    }
    catch(Exception e){
    
        //System.out.print(e);
    }
    //need to read in the xml as a string , replace the search string, the transform
    //read in the xml
    File file  = new File(wd+File.separator+"help"+File.separator+"xml"+File.separator+page1+".html");
    BufferedReader buff = new BufferedReader(new FileReader(file));
    String line = "";
    StringBuffer xml = new StringBuffer();
    while((line = buff.readLine()) != null){
        //  System.out.println(line);
        xml.append(line +"\n");
    }
    buff.close();
    //replace the seacrh string    
    String newxml = replaceatt(xml.toString(),args,"<span class='highlight'>"+args+"</span>");
    //build xml into dom
    //BufferedWriter writer = new BufferedWriter(new FileWriter(wd+File.separator+"help"+File.separator+"xml"+File.separator+page1+"ch"));
    //writer.write(newxml);
    //writer.close();

   out.println(newxml);

%>

</body>
</html>
