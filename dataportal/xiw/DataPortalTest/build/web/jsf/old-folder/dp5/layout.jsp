<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
<%--
The taglib directive below imports the JSTL library. If you uncomment it,
you must also add the JSTL library to the project. The Add Library... action
on Libraries node in Projects view can be used to add the JSTL 1.1 library.
--%>

<%@taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%> 
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>JSP Page</title>
    </head>
    <body>

        <h1>JSP Page</h1>
    
    
        <f:view>
        
            <h:panelGrid columns="3" style="width: 400px;">
                
                <f:subview id="menu">
                    <c:import url="menu.jsp"/>
                </f:subview>
                
                <h:panelGroup style="width: 50px;"/>
                
                <f:subview id="content">
                    <c:import url="menu.jsp"/>
                </f:subview>
                
                
            
            </h:panelGrid>
        
        </f:view>

    
    </body>
</html>
