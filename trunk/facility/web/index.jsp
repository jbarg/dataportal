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
</HTML>