<%--
<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
--%>

<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<%-- <%--
   <<f:view>
<%--  --%>
       <html>
           <head>
               <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
               <title>Data Portal Login</title>
           </head>
           <body>

<h:form id="foo">
<h:panelGrid columns="2" border="1" width="100%">
    
    <t:tree2 id="clientTree" value="#{treeBacker.treeData}" var="node" varNodeToggler="t">
        <f:facet name="person">
            <h:panelGroup>
                <f:facet name="expand">
                    <t:graphicImage value="./images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                </f:facet>
                <f:facet name="collapse">
                    <t:graphicImage value="./images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                </f:facet>
                <h:outputText value="#{node.description}" styleClass="nodeFolder"/>
            </h:panelGroup>
        </f:facet>
        <f:facet name="foo-folder">
            <h:panelGroup>
                <f:facet name="expand">
                    <t:graphicImage value="./images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                </f:facet>
                <f:facet name="collapse">
                    <t:graphicImage value="./images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                </f:facet>
                <h:outputText value="#{node.description}" styleClass="nodeFolder"/>
                <h:outputText value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
            </h:panelGroup>
        </f:facet>
        <f:facet name="bar-folder">
            <h:panelGroup>
                <f:facet name="expand">
                    <t:graphicImage value="./images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                </f:facet>
                <f:facet name="collapse">
                    <t:graphicImage value="./images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                </f:facet>
                <h:outputText value="#{node.description}" styleClass="nodeFolder"/>
                <h:outputText value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
            </h:panelGroup>
        </f:facet>
        <f:facet name="document">
            <h:panelGroup>
                <h:commandLink immediate="true" styleClass="#{t.nodeSelected ? 'documentSelected':'document'}" actionListener="#{t.setNodeSelected}">
                    <t:graphicImage value="./images/document.png" border="0"/>
                    <h:outputText value="#{node.description}"/>
                    <f:param name="docNum" value="#{node.identifier}"/>
                </h:commandLink>
            </h:panelGroup>
        </f:facet>
    </t:tree2>
<h:panelGrid width="100%" columns="1">
    <h:outputLabel value="to do something" />
</h:panelGrid>
                   
</h:panelGrid>
               </h:form>
           </body>
       </html>
       
       <%--
   <</f:view>
   < --%>
   
