<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html" indent="yes" omit-xml-declaration="yes"/>
	<xsl:template match="help">
			<xsl:apply-templates select="page"/>
			</xsl:template>
	<xsl:template match="page">
		<table border="0" width="80%">
			<tr>
				<th>
					<h1><xsl:value-of select="title" /></h1>
				</th>
			</tr>
			<tr>
				<td>
					<br />		
							</td>
			</tr>

<tr>
				<td>
					<br />
				</td>
			</tr>

			<tr>
				<td>
					<font align="center" size="5"><xsl:apply-templates select="//header"/></font>
				</td>
			</tr>
			<tr>
				<td>
					<br />
				</td>
			</tr>


			<tr>
				<td>
					<xsl:apply-templates select="//para[@no='1']"/>
				</td>
			</tr>
			<tr>
				<td>
					<br />
				</td>
			</tr>


			<tr>
				<td>
					<xsl:apply-templates select="//para[@no='2']"/>
				</td>
			</tr>
		</table>
	</xsl:template>
	<xsl:template match="header">
					<xsl:value-of select="."/>
			</xsl:template>
	<xsl:template match="para">
				<xsl:value-of select="."/>
		</xsl:template>
</xsl:stylesheet>
