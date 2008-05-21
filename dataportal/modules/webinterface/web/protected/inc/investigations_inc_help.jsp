<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<h:form>
    <table width="300" border="0">
        <td>
            <t:tree2 id="tree166" value="#{investigationHelpTree.data}" var="node" varNodeToggler="t" showRootNode="false">
    
                <f:facet name="hfoo-folder">
                    <h:panelGroup>                
                        <t:graphicImage id="gr34h" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                  
                        <t:graphicImage id="gr3h" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                        
                        <h:outputText id="text" value="#{node.description}" styleClass="nodeFolder"/>
                           
                       
                        <h:outputText id="t3ext1h" value=" (#{node.identifier})" styleClass="childCount" rendered="#{!empty node.children}"/>
                            
                    </h:panelGroup>
                </f:facet>
                <f:facet name="hfoo1-folder">
                    <h:panelGroup>
                        <f:facet name="expand22h">
                            <t:graphicImage id="gr43h2" value="../../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <f:facet name="collapse2h">
                            <t:graphicImage id="gr32h" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <h:outputText id="text2" value="#{node.description}" styleClass="nodeFolder"/>
                       
                    </h:panelGroup>
                </f:facet>
               
    
            </t:tree2>
        </td>
    </table>
</h:form>