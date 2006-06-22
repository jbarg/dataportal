<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f" %>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h" %>
<f:view>
    <!DOCTYPE .>
    <html>
    <head><title>DataPortal Search Page</title></head>
    <body>
    <center>
        <table BORDER=5>
            <tr><th CLASS="TITLE">DataPortal Search Page</th></tr>
        </table>
        <p>
        <h:form id="form1">
            <!-- Keyword Search <h:inputText id="keyword"/><br/> -->
            Keyword Search <h:inputText value="#{searchController.keyword}"/><br/> 
            <h:commandButton value="Search" action="#{searchController.studySearch}"/>
        </h:form>
    </center></body></html>
</f:view>
