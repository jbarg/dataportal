<HTML>
<TITLE>Facility Administration</TITLE>
<BODY>
<H1>Facility Administration Web Service</H1>
<P>http://<%
out.println(request.getHeader("Host") + request.getContextPath() + "/service/FacilityAdminService");
%></P>
<P>Create Facility Request</P>
<TEXTAREA cols='100' rows='20' readonly>
<fadm:facilityAdmin xmlns:fadm="http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd">
  <createFacility>
    <facilityName>MAWTEST</facilityName>
    <facilityDescription>Mark testing 1 2 3</facilityDescription>
    <service>
      <serviceName>XML Wrapper</serviceName>
      <serviceDescription>Returns XML data from database</serviceDescription>
      <serviceType>XMLW</serviceType>
      <serviceEndpoint>http://localhost:8080/axis/services/xml_wrapper_badc</serviceEndpoint>
      <serviceWSDL>http://localhost:8080/axis/services/xml_wrapper_badc?wsdl</serviceWSDL>
    </service>
    <service>
      <serviceName>Access Control List</serviceName>
      <serviceDescription>Returns Access Control details for user</serviceDescription>
      <serviceType>ACL</serviceType>
      <serviceEndpoint>http://localhost:8080/axis/services/acl</serviceEndpoint>
      <serviceWSDL>http://localhost:8080/axis/services/acl?wsdl</serviceWSDL>
    </service>
  </createFacility>
</fadm:facilityAdmin>
</TEXTAREA>
<P>Delete Facility Request</P>
<TEXTAREA cols='100' rows='3' readonly>
<fadm:facilityAdmin xmlns:fadm="http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd">
  <deleteFacility facilityID="b4f27310-27de-11d7-b4c0-b8a03c50a862"/>
</fadm:facilityAdmin>
</TEXTAREA>
<P>Find Facility Request</P>
<TEXTAREA cols='100' rows='3' readonly>
<fadm:facilityAdmin xmlns:fadm="http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd">
  <findFacility>FAC</findFacility>
</fadm:facilityAdmin>
</TEXTAREA>
<P>Create Services Request</P>
<TEXTAREA cols='100' rows='18' readonly>
<fadm:facilityAdmin xmlns:fadm="http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd">
  <createServices facilityID="0a0445b0-3ab6-11d7-ba2d-b8a03c50a862">
    <service>
      <serviceName>A new service</serviceName>
      <serviceDescription>Create a new service</serviceDescription>
      <serviceType>XMLW</serviceType>
      <serviceEndpoint>http://localhost:8080/axis/services/xml_wrapper_badc</serviceEndpoint>
      <serviceWSDL>http://localhost:8080/axis/services/xml_wrapper_badc?wsdl</serviceWSDL>
    </service>
    <service>
      <serviceName>Another service</serviceName>
      <serviceDescription>Create another new services</serviceDescription>
      <serviceType>ACM</serviceType>
      <serviceEndpoint>http://localhost:8080/axis/services/acl</serviceEndpoint>
      <serviceWSDL>http://localhost:8080/axis/services/acl?wsdl</serviceWSDL>
    </service>
  </createServices>
</fadm:facilityAdmin>
</TEXTAREA>
<P>Delete Services Request</P>
<TEXTAREA cols='100' rows='6' readonly>
<fadm:facilityAdmin xmlns:fadm="http://www.e-science.cclrc.ac.uk/schema/facilityAdmin.xsd">
 <deleteServices>
  <service ID="771a5ba0-435b-11d7-ba2d-b8a03c50a862"/>
  <service ID="771ca590-435b-11d7-ba2d-b8a03c50a862"/>
 </deleteServices>
</facilityAdmin>
</TEXTAREA>
</HTML>