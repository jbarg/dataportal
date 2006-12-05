<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>

<table width="90%" style="margin-top:-20px">
     <tbody>
        <tr>
            
            <td>
                <h:messages id="message" globalOnly="true" errorClass="error" infoClass="info" />
                <br/>
                <a4j:region   selfRendered="true" > 
                    <h:form>
                        
                        <h:panelGrid id="newFacility" columns="3" >
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Name:"/>
                            </h:outputLabel  >
                            
                            <h:inputText id="namej_id_1" size="30" required="true" value="#{facilityAdminBean.name}">
                                <f:validateLength minimum="5" maximum="16"/>
                            </h:inputText> 
                            
                            <h:message for="namej_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Type:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="type" value="#{facilityAdminBean.type}" >
                                <f:selectItem itemLabel="Wrapper" itemValue="DPAL"/>          
                                <f:selectItem itemLabel="Access and Control" itemValue="ACM"/>    
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Data in folders:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="folders" value="#{facilityAdminBean.folders}" >
                                <f:selectItem itemLabel="Yes" itemValue="Y"/>
                                <f:selectItem itemLabel="No" itemValue="N"/>
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>  
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Active:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="active" value="#{facilityAdminBean.activeFac}" >
                                <f:selectItem itemLabel="Yes" itemValue="Y"/>
                                <f:selectItem itemLabel="No" itemValue="N"/>
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>  
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Spaces in keywords:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="keywords" value="#{facilityAdminBean.keywords}" >
                                <f:selectItem itemLabel="Yes" itemValue="Y"/>
                                <f:selectItem itemLabel="No" itemValue="N"/>
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>  
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="All Keywords:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="allKeywords" value="#{facilityAdminBean.allKeywords}" >
                                <f:selectItem itemLabel="Yes" itemValue="Y"/>
                                <f:selectItem itemLabel="No" itemValue="N"/>
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>  
                            
                            <h:commandButton actionListener="#{facilityAdminBean.createFacility}" title="Add" value="Add"/>
                            
                            <h:panelGroup/>
                            <h:panelGroup/>
                        </h:panelGrid> 
                    </h:form>
                  
                </a4j:region>     
            </td>
        </tr>
    </tbody>
</table>

