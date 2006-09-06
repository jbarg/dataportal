<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib prefix = "c" uri = "http://java.sun.com/jsp/jstl/core" %>


 
    <br />
    <br />
    <br />   
                                              
        
    <h:outputText styleClass="nodeFolder" value="Searching facility(s):  "/>
      
    <c:forEach var="facility" items="${sessionScope.visit.queryRequest.facilities}">
        <c:out value="${facility}, "/>
    </c:forEach>
    <br />
    <br />
    
        <c:choose>
            <c:when  test="${sessionScope.visit.finished}">
                Finished
      
            </c:when>
            <c:otherwise>
                <h:outputText styleClass="nodeFolder" value="Finished searching:  "/>
     
                <c:forEach var="facility" items="${sessionScope.visit.completedFacilities}">
                    <c:out value="${facility}, "/>
                </c:forEach>
            </c:otherwise>
        </c:choose>
 
    

    <br />
    <br />
   


