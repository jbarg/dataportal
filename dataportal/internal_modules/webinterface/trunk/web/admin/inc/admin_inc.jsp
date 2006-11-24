<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>

<table width="90%">
     <tbody>
        <tr>
            <td width="10">&nbsp;</td>
            <td>
                <table width="90%" border="0">
                     <tbody>
                        <tr>
                            <td> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
                            </td>                 
                            
                        </tr>
                    </tbody>   
                </table>
                
                
                <a4j:region selfRendered="true"> 
                    <h:form>
                        <h:panelGrid columns="4">
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Search DNs:"/>
                            </h:outputLabel  >
                            
                            <h:inputText id="dnj_id_1" size="30" required="true" value="#{adminBean.searchString}">
                                <f:validateLength minimum="1" maximum="60"/>
                            </h:inputText> 
                            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                                     closePopupOnExitingPopup="true"
                                     displayAtDistanceX="5"
                                     displayAtDistanceY="-40" >
                                
                                <t:graphicImage url="../../images/help.gif" border="0" />
                                <f:facet name="popup">
                                    <h:panelGroup>
                                        <h:panelGrid columns="1" >
                                            <table width="100" >
                                                <tr>
                                                    <td nowrap="false" width="150">
                                                        <h:outputText style="font-size: 14px" value="Use * to search all DNs." />
                                                    </td>
                                                </tr>
                                            </table>
                                        </h:panelGrid>
                                    </h:panelGroup>
                                </f:facet>
                            </t:popup>
                            <h:message for="dnj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            <h:panelGroup/>
                            <%-- Does not work with rendering renerdre=false sections" <a4j:commandButton reRender="searchResults" actionListener="#{dataAuthorisationBean.search}" value="Search"/>--%>
                            <%-- Old button before ajax button--%>
                            <h:commandButton actionListener="#{adminBean.search}" title="Search" value="Search"/>
                            <h:panelGroup/>
                            <h:panelGroup/>
                        </h:panelGrid>
                    </h:form>
                    <br />
                    <h:form>
                        <h:panelGrid id="searchResults" rendered="#{visit.adminData.searched}" columns="3">
                            <h:outputLabel >
                                <h:outputText styleClass="body" value="View Stats: "/>&nbsp;
                            </h:outputLabel  >
                            
                            <h:selectOneMenu id="usernameselectj_id_1" required="true" value="#{visit.adminData.searchUser}" >
                                <f:selectItems value="#{visit.adminData.searchedUsers}"/>
                                <f:validateLength minimum="1" />
                            </h:selectOneMenu>
                            
                            <h:message for="usernameselectj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                             <h:panelGroup/>
                            <%-- Does not work with rendering renerdre=false sections" <a4j:commandButton reRender="searchResults" actionListener="#{dataAuthorisationBean.search}" value="Search"/>--%>
                            <%-- Old button before ajax button--%>
                            <h:commandButton action="#{adminBean.viewStats}" title="View" value="View"/>
                            <h:panelGroup/>
                        </h:panelGrid> 
                        
                    </h:form>    
                </a4j:region>
            </td>
        </tr>
    </tbody>
</table>

