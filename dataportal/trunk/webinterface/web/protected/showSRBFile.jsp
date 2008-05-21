<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@page import="java.net.URL,java.io.*,org.apache.log4j.*,java.awt.*,uk.ac.dl.dp.web.util.*"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
       
        <link rel="stylesheet" type="text/css" href="../css/basic.css" />
        <script language="javascript" src="../css/download.js"></script>
        <title><%=request.getParameter("name")%></title>
          
        <script>
             
            
         
            
            function doIt(){
            <%
                String filename = request.getParameter("file");

               
                System.out.println(""+new File(filename).exists());
                ImageInfo image = new ImageInfo();
                image.setInput(new FileInputStream(new File("c:/extract.png")));
                int width = image.getWidth();
                int height = image.getHeight();


            %>
            var width = <%=width%> ;        
            var height = <%=height%> ;     

            if (self.innerWidth)
            {
            frameWidth = parent.innerWidth;
            frameHeight = parent.innerHeight;
            }
            else if (document.documentElement && document.documentElement.clientWidth)
            {
            frameWidth = parent.document.documentElement.clientWidth;
            frameHeight = parent.document.documentElement.clientHeight;
            }
            else if (document.body)
            {
            frameWidth = parent.document.body.clientWidth;
            frameHeight = parent.document.body.clientHeight;
            }
            else return;
//alert(width +"  " +height);
            if (width >= 0 || height > screen.height/2 )
            {
           
            newWidth = self.screen.width/2;
            newHeight = self.screen.height/2;
            if (document.layers)
            {
            tmp1 = parent.outerWidth - parent.innerWidth;
            tmp2 = parent.outerHeight - parent.innerHeight;
            newWidth -= tmp1;
            newHeight -= tmp2;
            }
            self.window.resizeTo(screen.width/1.5,screen.height/1.5);
            self.window.moveTo(self.screen.width/4,self.screen.height/4);
            
            //center
            //self.window.moveTo(self.screen.width/4,self.screen.height/4);
            }
            else
            {
            //alert('No resize necessary');
            }
             self.resizable = 1;
             
              if(BrowserDetect.browser == "Explorer"){
            //set auto width
            e=document.getElementById("div");
            e.style.width = screen.width/1.5 -12 + 'px'
            e.style.height = screen.height/1.5 -35 + 'px'
            }
            else {
            
            }
               
          }
        </script>
    </head>
    <body  onload="doIt();">        
       
    <div style="overflow: auto;" id="div">

         <%--<t:graphicImage url="../../view/#{param.name}" border="0" />--%>
     <img src="../view/<%=request.getParameter("name")%>" border="0" />
           <!--<img src="../view/hourglass-a.gif" border="0" />-->
      
         </div>
                
  <%
        //delete file
       // new File(request.getParameter("file")).delete();     
  %>
 
    </body>
</html>
