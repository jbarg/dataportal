<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib prefix="tiles" uri="http://struts.apache.org/tags-tiles"%>

<h:form>
 
 <br />
 <br />
 <br />
    
                <h:panelGrid columns="2" >
                   
                    <h:panelGrid columns="3">
                        <h:messages globalOnly="true" styleClass="errors"/>
                        <h:panelGroup/>
                        <h:panelGroup/>

                        <h:outputLabel for="usernameinput">
                            <h:outputText value="Username"/>
                        </h:outputLabel  >
                                                                                           
                        <h:inputText id="Username" size="20" required="true" value="#{authorisationBean.username}">
                            <f:validateLength minimum="1" maximum="10"/>
                        </h:inputText> 
                        <h:message for="Username" styleClass="error"  />

                        <h:outputLabel for="passwordinput">
                            <h:outputText value="Password"/>
                        </h:outputLabel  >

                        <h:inputSecret id="Password"  size="20" required="true" value="#{authorisationBean.password}">
                            <f:validateLength minimum="6" maximum="10"/>
                        </h:inputSecret> 
                        <h:message for="Password" styleClass="error"/>
                        <h:panelGroup/>

                        <h:commandButton action="#{authorisationBean.login}" title="Login" value="Logon"/>
                        <h:panelGroup/>

                    </h:panelGrid>
                </h:panelGrid>

                    <br />
                    <br />
                 <%--// Goto   <h:commandLink target="_blank"> <h:outputText value="MyProxy Upload Home"/> </h:commandLink> to upload credentials
                   --%>
             


            </h:form>
