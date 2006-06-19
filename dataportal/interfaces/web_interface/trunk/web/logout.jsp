<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f" %>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h" %>
<f:view>
   <html>
      <head>
         <title>
            JSF in Action - Hello, world!
         </title>
      </head>
      <body>
         <h:form id="goodbyeForm">
            <p>
               <h:outputText id="welcomeOutput" value="Goodbye!" style="font-family: Arial, sans-serif; font-size: 24;
                                   font-style: bold; color: green;"/>
            </p>
            <p>
              <h:outputText id="helloBeanOutputLabel" value="Number of controls displayed:"/>
              <h:outputText id="helloBeanOutput" value="#{helloBean.numControls}"/>
            </p>
         </h:form>
      </body>
   </html>
</f:view>
