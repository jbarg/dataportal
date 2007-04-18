
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib prefix="tiles" uri="http://struts.apache.org/tags-tiles"%>

<html>  
    <head>        
        <link rel="stylesheet" type="text/css" href="css/basic.css" />
      
    </head>

    <body>
        <f:view>
            
            <f:subview id="header">
                <tiles:insert attribute="header" flush="false"/>
            </f:subview>
           
            <f:subview id="navigation">
                <tiles:insert attribute="navigation" flush="false"/>
            </f:subview>
           
            <f:subview id="navigation2">
                <tiles:insert attribute="navigation2" flush="false"/>
            </f:subview>
                    
             <f:subview id="body">
                <tiles:insert attribute="body" flush="false"/>
            </f:subview>
          
            <f:subview id="footer2">
                <tiles:insert attribute="footer2" flush="false"/>
            </f:subview>
            <f:subview id="footer">
                <tiles:insert attribute="footer" flush="false"/>
            </f:subview>
        </f:view>
       
    </body>

</html>
