<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">


<%@ page import="ac.dl.xml.*,java.util.*, java.io.*"%>
<html>
<head>
<title>CLRC Data Portal/Estedi - Data Portal query results</title>
<!--<link rel="stylesheet" href="../style/help.css" type="text/css" /> -->
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<style type="text/css">
span.yellow {background-color: yellow}
span.red {background-color: red}
span.blue {background-color: #D55ECC}
span.black {background-color: #C6D3E8}
span.gray {background-color: #A3F8FC}
span.green {background-color: #63EA5B}
</style>
<%!
String replacef(String str,String pattern , String replace){
        int s =  0;
        int e = 0;
        
        StringBuffer result = new StringBuffer();
        
       while((e = str.indexOf(pattern,s)) >0){
            result.append(str.substring(s,e));
            result.append(replace);
            s = e + pattern.length();


        }
        result.append(str.substring(s));
        return result.toString();
}
%>
<%!
ArrayList getList(String text){
    String[] values = null;
    ArrayList a =new ArrayList();
    int pos = text.indexOf('"');
    if(pos == -1){
        values = text.split(" ");
        for(int d = 0 ; d < values.length; d++){
           a.add(values[d]);
        }
        
    }
    else{
    int i = 0;
    int j = 0;
    ArrayList list = new ArrayList();
    boolean quit = true;
    while(quit){
    //try again
            int g = text.length();
            pos = text.indexOf('"');
            i = text.indexOf('"',pos+1);
            if(text.equals(text.substring(pos,i+1))){
                //System.out.println("ifg "+text.substring(pos+1,i));
                a.add(text.substring(pos+1,i));
                quit = false;
            }
            else if(text.startsWith("\"")){
            if(i == -1) quit = false;
            else{
                //System.out.println("starts with \" "+text);
                //System.out.println("adding "+text.substring(pos+1,i)+" to the list");
                 a.add(text.substring(pos+1,i));
                    text = text.substring(i+1,g);
                    //System.out.println("txt is now "+text);

                     //System.out.println(text);
                }
            }
            else{
                a.add(text.substring(pos+1,i));
                 text= replacef(text,text.substring(pos,i+1),"");
            //     System.out.println("esle "+text);
            }
            int newpos = text.indexOf('"');
            if(newpos == -1) {
               // System.out.println("no \"");
                quit = false;
    }
            if(text.indexOf('"',newpos+1) == -1) quit = false;
        }
         String[] spaces  =text.split(" ");
        for(int s  =0 ; s< spaces.length;s++){
            if(spaces[s].equals("\"")){}
            else a.add(spaces[s]);
        }
        //System.out.println("values are");
      //  System.out.println("size of the list is "+a.get(0));
     //System.out.println("size of the list is "+a.get(1));
     //System.out.println("size of the list is "+a.get(2));
        for(int si = 0 ; si < a.size(); si++){
            if(a.get(si).equals("")){
                //a.remove(si);
            }
            else  list.add(a.get(si));
        }
        a = list;

    }
    return a;

}
%>
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
    String args = request.getParameter("string_search");
    //System.out.println("args are "+args);
    
 //need to read in the xml as a string , replace the search string, the transform
    //read in the xml
    File file  = new File(wd+File.separator+"help"+File.separator+"DataPortal_help.htm");
    BufferedReader buff = new BufferedReader(new FileReader(file));
    String line = "";
    StringBuffer xml = new StringBuffer();
    while((line = buff.readLine()) != null){
        //  System.out.println(line);
        xml.append(line +"\n");
    }
    buff.close();
    //System.out.println("here");
    ArrayList list = getList(args);
    String[] colours =  {"yellow","red","blue","green","black","gray","yellow","red","blue","green","black","gray","yellow","red","blue","green","black","gray"};
    String newxml = xml.toString();
    int size = list.size();
    //go through arraylist and change
    for(int j = 0;j<size;j++){
        String text = (String)list.get(j);
        //System.out.println("values giev are     "+text);
       //replace the seacrh string    
         newxml = replaceatt(newxml,text,"<span class='"+colours[j]+"'>"+text+"</span>");
         if(j == 17) size = 17;
       }
    //build xml into dom
    //BufferedWriter writer = new BufferedWriter(new FileWriter(wd+File.separator+"help"+File.separator+"xml"+File.separator+page1+"ch"));
    //writer.write(newxml);
    //writer.close();

   out.println(newxml);

%>

</body>
</html>

