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
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>Data Portal</title>
    </head>
    <body>


        <f:view>
            <jsp:include page="./layout/header.jsp"/>
            
            <h:form>
                <h:panelGrid columns="1" width="100%" style="width: 1200px">
                    <%--               
                    <f:subview id="menu">
                    <c:import url="./layout/header.jsp"/>
                    </f:subview>
                    --%>
                    <h:panelGrid columns="4" width="100%" style="width: 1200px" bgcolor="#FFFFCC">
                        <h:outputLabel value="You are here:"/>
                        <h:outputLabel value="Home"/>
                        <h:commandLink value="Login"/>
                        <h:commandLink value="Logout"/>
                    </h:panelGrid>
                
                    <h:panelGrid columns="2" border="1" width="100%" style="width: 1200px">
                    
                        <h:panelGroup style="height: 250px;">
                            <f:subview id="navigation">
                                <c:import url="./navigation/navigation.jsp"/>
                            </f:subview>
                        </h:panelGroup>
                        <h:panelGrid  columns="1" border="1" width="100%">
                            <h:panelGrid columns="1">
    
                                <t:jscookMenu layout="hbr" theme="ThemeIE">
                                    <t:navigationMenuItem id="home" itemLabel="Home" action="gohome"/>
                                    <t:navigationMenuItem id="search" itemLabel="Search" action="search"/>                                 
                                    <t:navigationMenuItem id="expand" itemLabel="Expand" action="expand"/>
                                    <t:navigationMenuItem id="data" itemLabel="Data" action="data"/>
                                    <t:navigationMenuItem id="bookmarks" itemLabel="Bookmarks" action="bookmarks"/>
                                </t:jscookMenu> 

                               
                               
                               
                               
                            </h:panelGrid>
                            <h:panelGroup style="width: 300px;">
                                <f:subview id="login">
                                    <c:import url="./login/login.jsp"/>
                                </f:subview>
                            </h:panelGroup>
                        </h:panelGrid>
                        

                    </h:panelGrid>
                    <%--                
                    <f:subview id="content">
                    <c:import url="./layout/footer.jsp"/>
                    </f:subview>
                    --%>
                
                
            
                </h:panelGrid>
            </h:form>
        
            <jsp:include page="./layout/footer.jsp"/>
            
        </f:view>

    
    </body>
</html>
