<?xml version="1.0"?><xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">	<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>	<xsl:template match="shoppingCart">		<xsl:choose>			<xsl:when test="item[@type='Study']">				<br/>				<!--<b><xsl:value-of select="//@type"/></b>-->				<!--              <b>Study</b>-->				<table border="1" width="100%" cellspacing="0" cellpadding="1">					<tr bgcolor="#ADD8E6">						<td width="25%" id="tablehead">							Study						</td>						<td width="8%" id="tablehead">							Notes						</td>						<td  id="tablehead">							Resend Query						</td>						<td width="13%" id="tablehead">							Transfer						</td>						<td width="35%" id="tablehead">							Date added						</td>						<td width="13%" id="tablehead">-</td>						<td width="5%" id="tablehead">							Delete						</td>					</tr>					<xsl:apply-templates select="item[@type='Study']">						<xsl:sort select="name"/>					</xsl:apply-templates>				</table>				<br/>			</xsl:when>			<xsl:otherwise/>		</xsl:choose>	</xsl:template>	<xsl:template match="item">		<xsl:for-each select=".">			<tr>				<td>					<xsl:value-of select="name"/>				</td>				<td align="center">					<!--<xsl:element name="a">			<xsl:attribute name="onMouseOver">window.status=Add Note';return true;</xsl:attribute>							<xsl:attribute name="onMouseOut">window.status='';</xsl:attribute>						<xsl:attribute name="href"></xsl:attribute>						<xsl:attribute name="onClick"><xsl:text>addnote('</xsl:text><xsl:value-of select='url' /><xsl:text>');return false;</xsl:text></xsl:attribute>					Add				</xsl:element>				<xsl:text> - </xsl:text>	-->					<xsl:element name="a">					<xsl:attribute name="id">tablerow</xsl:attribute>						<xsl:attribute name="onmouseover">window.status='See Note';return true;</xsl:attribute>						<xsl:attribute name="onmouseout">window.status='';</xsl:attribute>						<xsl:attribute name="href">javascript:return false;</xsl:attribute>						<xsl:attribute name="onmousedown"><xsl:text>seenoteWS('</xsl:text><xsl:value-of select="url"/><xsl:text>',event);return false;</xsl:text></xsl:attribute>					View				</xsl:element>				</td>				<td align="center">					<xsl:element name="a">					<xsl:attribute name="id">tablerow</xsl:attribute>						<xsl:attribute name="onmouseover">window.status='See Study';return true;</xsl:attribute>						<xsl:attribute name="onmouseout">window.status='';</xsl:attribute>						<xsl:attribute name="href">../servlet/RedoBasicSearch?url=<xsl:value-of select="url"/></xsl:attribute>									Ok					</xsl:element>				</td>				<xsl:choose>					<xsl:when test="@type='File'">						<td align="center">							<xsl:element name="a">							<xsl:attribute name="id">tablerow</xsl:attribute>								<xsl:attribute name="onmouseover">window.status='Transfer';return true;</xsl:attribute>								<xsl:attribute name="onmouseout">window.status='';</xsl:attribute>								<xsl:attribute name="href">../servlet/TransferServlet?url=<xsl:value-of select="url"/></xsl:attribute>										transfer					</xsl:element>						</td>					</xsl:when>					<xsl:otherwise>						<td id="tablerow">n/a</td>					</xsl:otherwise>				</xsl:choose>				<td id="tablerow">					<xsl:value-of select="date"/>				</td>				<xsl:choose>					<xsl:when test="name[starts-with(.,'li')] and string-length(name) > 8">						<td>							<xsl:element name="a">							<xsl:attribute name="id">tablerow</xsl:attribute>								<xsl:attribute name="onmouseover">window.status='Manipulate';return true;</xsl:attribute>								<xsl:attribute name="onmouseout">window.status='';</xsl:attribute>								<xsl:attribute name="href">ManipulateWS.jsp?url=<xsl:value-of select="url"/>&amp;where=checkout</xsl:attribute>													manipulate							</xsl:element>						</td>					</xsl:when>					<xsl:otherwise>						<td id="tablerow">n/a</td>					</xsl:otherwise>				</xsl:choose>				<td align="center">					<!--<xsl:element name="a">						<xsl:attribute name="onMouseOver">window.status='Delete';return true;</xsl:attribute>							<xsl:attribute name="onMouseOut">window.status='';</xsl:attribute>						<xsl:attribute name="href">../servlet/RemoveServlet1?url=<xsl:value-of select="url"/></xsl:attribute>					delete					</xsl:element>-->					<xsl:element name="input">					<xsl:attribute name="id">tablerow</xsl:attribute>						<xsl:attribute name="name">delete</xsl:attribute>						<xsl:attribute name="type">checkbox</xsl:attribute>						<xsl:attribute name="value"><xsl:value-of select="url"/></xsl:attribute>					</xsl:element>				</td>			</tr>		</xsl:for-each>	</xsl:template></xsl:stylesheet>