<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%-- style="width:947px"   works but makes it static on div --%>

<f:loadBundle basename="uk.ac.dl.dp.web.messages.facility" var="facility_properties" />
 

<c:choose>
    <c:when test="${visit.width == 954}">
              
        <img  src="../../images/${facility_properties['facility.banner']}_954px.jpg"  alt="Sci-Tech" border="0"  />
        
    </c:when>
    
    <c:when test="${visit.width == 1210}">
        
           <img  src="../../images/${facility_properties['facility.banner']}_1210px.jpg"  alt="Sci-Tech" border="0"  />
            
    </c:when>
    
    <c:when test="${visit.width == 1530}">
        
         <img  src="../../images/${facility_properties['facility.banner']}_1600px.jpg"  alt="Sci-Tech" border="0"  />
    
    </c:when>
</c:choose>     
