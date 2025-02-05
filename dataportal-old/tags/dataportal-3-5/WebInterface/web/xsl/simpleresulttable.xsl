<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>

<xsl:template match="CLRCMetadata">
  <xsl:choose>
      <xsl:when test="MetadataRecord">
                  
    
         <table border="1" width="100%">
  	 <thead><tr><th>Study Name</th><th>Facility</th><th>Owner</th><th>Institution</th><th>Date</th><th>Select</th></tr></thead>
  	 <tbody>
  	 <xsl:apply-templates select="MetadataRecord" >
		<xsl:sort select="@facility"/>
  	 </xsl:apply-templates>
  	 </tbody>
  	 </table>
  
      </xsl:when>
      <xsl:otherwise>
	 <h2>No records found which satisfy query.</h2>
      </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<xsl:template match="MetadataRecord" >
     <tr>
        <td><xsl:apply-templates select="*/StudyName"/></td>
        <td><xsl:value-of select="@facility" /></td>
        <td><xsl:apply-templates select="*/Investigator[1]/Name"/></td>
        <td><xsl:apply-templates select="*/Investigator[1]/Institution"/></td>
        <td><xsl:apply-templates select="*/StudyInformation/TimePeriod"/></td>
        <td><input type="checkbox" name="expand">
		<xsl:attribute name="value"><xsl:value-of select="@metadataID"/></xsl:attribute>
		<xsl:if test="@expand"><xsl:attribute name="checked"></xsl:attribute></xsl:if>
	    </input></td>
     </tr>
</xsl:template>
   

<xsl:template match="StudyName">
  <xsl:value-of select="."/> 
</xsl:template>

<xsl:template match="Investigator/Name">
  <xsl:if test="PersonTitle"><xsl:value-of select="PersonTitle"/><xsl:text> </xsl:text></xsl:if>
  <xsl:if test="Initials"><xsl:value-of select="Initials"/><xsl:text> </xsl:text></xsl:if>
  <xsl:value-of select="Surname"/>
</xsl:template>

<xsl:template match="Institution">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="TimePeriod">
  <xsl:apply-templates select="StartDate"/>
</xsl:template>

<xsl:template match="StartDate">
  <xsl:apply-templates select="Date"/>
</xsl:template>

</xsl:stylesheet>

