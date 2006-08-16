<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
<%--
The taglib directive below imports the JSTL library. If you uncomment it,
you must also add the JSTL library to the project. The Add Library... action
on Libraries node in Projects view can be used to add the JSTL 1.1 library.
--%>
<%--
<%@taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%> 
--%>


<%@taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%> 
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<html>
    <body>
        <h:form>
            <h:panelGrid width="150" columns="1" border="1">
                <h:panelGrid columns="1" border="0">
                    <h:outputLabel value="Main" />
                    <h:commandLink value="Search"/>
                    <h:commandLink value="Data Centre"/>
                    <h:commandLink value="Book Mark"/>
                    <h:outputLabel value="User"/>
                    <h:commandLink value="History"/>
                    <h:commandLink value="Logout"/>
                    <h:outputLabel value="Information"/>
                    <h:commandLink value="Help"/>
                    <h:commandLink value="Documentation"/>
                    <h:commandLink value="Data Portal"/>
                                
                    <%--
                    <h:commandLink value="login" action="goFacesConfigNavRule"/>
                    <h:commandLink value="logout" action="goFacesConfigNavRule2"/>
                    <h:commandLink value="page3" action="goFacesConfigNavRule3"/>
                    <h:commandLink value="page4" action="goFacesConfigNavRule4"/>
                    --%>

                </h:panelGrid>
            </h:panelGrid>
            
        </h:form>
    </body>
</html>
