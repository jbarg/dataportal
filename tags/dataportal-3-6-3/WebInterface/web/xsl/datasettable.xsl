<?xml version="1.0"?><xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">	<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>	<xsl:template match="/CLRCMetadata">		<xsl:choose>			<xsl:when test="MetadataRecord">				<xsl:apply-templates select="MetadataRecord">					<xsl:sort select="@metadataID"/>				</xsl:apply-templates>				<table>					<tr>						<td width="70%" />						<td align="right">Studies</td>						<td align="right">							<input type="button" class="button70"  value="Select None" onclick="selectAll('simpleresult','studydownload',false);"/>						</td>						<td align="right">							<input type="button"  class="button70" value="Select All" onclick="selectAll('simpleresult','studydownload',true);"/>						</td>					</tr>					<tr>						<td width="70%" />						<td align="right">Data Sets: </td>						<td align="right">							<input type="button"  class="button70" value="Select None" onclick="selectAll('simpleresult','datasetdownload',false);"/>						</td>						<td align="right">							<input type="button" class="button70"  value="Select All" onclick="selectAll('simpleresult','datasetdownload',true);"/>						</td>					</tr>					<tr>						<td>							<input name="see_results" type="submit" value="Add selected items to cart"/>						</td>						<td width="60%" align="right">						Files:												</td>						<td align="right">							<input type="button" class="button70"  value="Select None" onclick="selectAll('simpleresult','filedownload',false);"/>						</td>						<td align="right">							<input type="button" class="button70"  value="Select All" onclick="selectAll('simpleresult','filedownload',true);"/>						</td>					</tr>				</table>			</xsl:when>									<xsl:otherwise>				<h2>No records found which satisfy query.</h2>			</xsl:otherwise>		</xsl:choose>	</xsl:template>	<xsl:template match="MetadataRecord">		<table border="0" width="95%" align="center" cellspacing="0" cellpadding="1" bgcolor="#FFFFFF">			<tr bgcolor="#ADD8E6">				<td colspan="4">					<b>						<font size="4">							<xsl:apply-templates select="*/StudyName"/>						</font>					</b>				</td>			</tr>			<tr bgcolor="#ADD8E6">				<td colspan="1">					<i>Institution:    </i>					<xsl:apply-templates select="*/Investigator[1]/Institution"/>				</td>				<td align="right">Select Study: 				<input type="checkbox" name="studydownload" style="cursor:crosshair;">						<xsl:attribute name="value"><xsl:value-of select="@metadataID"/></xsl:attribute>						<xsl:if test="@expand">							<xsl:attribute name="checked"/>						</xsl:if>					</input>				</td>				<td width="20">					<xsl:text> </xsl:text>				</td>			</tr>			<tr bgcolor="#ADD8E6">				<td colspan="4">					<i>	PI:     </i>					<xsl:apply-templates select="*/Investigator[1]/Name"/>				</td>			</tr>			<tr bgcolor="#ADD8E6">				<td colspan="4">					<i>Start date:      </i>					<xsl:apply-templates select="*/StudyInformation/TimePeriod"/>				</td>			</tr>			<tr>				<td>					<br/>				</td>			</tr>						<!--<tr>			<td colspan="4">				<xsl:value-of select="substring(*/StudyInformation/Purpose/Abstract,1,400)"/>			</td>		</tr>-->			<!--	<xsl:apply-templates select="*/StudyName"/>			<xsl:apply-templates select="*/Investigator[1]/Name"/>			<xsl:apply-templates select="*/Investigator[1]/Institution"/>			<xsl:apply-templates select="*/StudyInformation/TimePeriod"/>			<xsl:apply-templates select="@metadataID"/>-->		</table>		<br/>		<xsl:apply-templates select="DataHolding"/>		<br/>		<br/>		<br/>	</xsl:template>	<xsl:template match="StudyName">		<xsl:value-of select="."/>	</xsl:template>	<xsl:template match="Investigator/Name">		<xsl:if test="PersonTitle">			<xsl:value-of select="PersonTitle"/>			<xsl:text> </xsl:text>		</xsl:if>		<xsl:if test="Initials">			<xsl:value-of select="Initials"/>			<xsl:text> </xsl:text>		</xsl:if>		<xsl:value-of select="Surname"/>	</xsl:template>	<xsl:template match="Investigator/Institution">		<xsl:value-of select="."/>	</xsl:template>	<xsl:template match="TimePeriod">		<xsl:apply-templates select="StartDate"/>	</xsl:template>	<xsl:template match="@metadataID">		<tr>			<td>Select study :</td>			<td>				<input type="checkbox" style="cursor:crosshair;" name="studydownload">					<xsl:attribute name="value"><xsl:value-of select="."/></xsl:attribute>				</input>			</td>		</tr>	</xsl:template>	<xsl:template match="StartDate">		<xsl:apply-templates select="Date"/>	</xsl:template>	<xsl:template match="DataHolding">		<table border="1" width="95%" cellspacing="0" cellpadding="1" align="center">			<xsl:apply-templates select="DataName"/>			<xsl:apply-templates select="TypeOfData"/>			<xsl:apply-templates select="Status"/>			<xsl:apply-templates select="LogicalDescription"/>			<xsl:apply-templates select="FileFormat"/>			<xsl:apply-templates select="RelatedStudy"/>			<!-- <tr><td><b>Download whole data holding</b></td>             <td><input type="checkbox" name="dataholdingdownload">                 <xsl:attribute name="value"><xsl:value-of select="DataName"/></xsl:attribute> 		<xsl:if test="@dataholdingdownload"><xsl:attribute name="checked"></xsl:attribute></xsl:if>                 </input></td>         </tr>-->			<xsl:if test="DataSet[not(ParentDataSetRef)]">				<tr>					<td valign="top">						<strong>Data Sets</strong>					</td>					<td>						<xsl:apply-templates select="DataSet[not(ParentDataSetRef)]"/>					</td>				</tr>			</xsl:if>					</table>	</xsl:template>	<xsl:template match="DataSet">		<xsl:choose>			<xsl:when test="starts-with(@dataid,'PARAM')"> </xsl:when>			<xsl:otherwise>				<table border="1" width="100%">					<xsl:apply-templates select="DataName"/>					<xsl:apply-templates select="TypeOfData"/>					<xsl:apply-templates select="Status"/>					<xsl:apply-templates select="LogicalDescription"/>					<xsl:apply-templates select="FileFormat"/>					<xsl:apply-templates select="ParentDataSetRef"/>					<xsl:apply-templates select="DerivedFromDataSetRef"/>					<xsl:apply-templates select="RelatedDataSetRef"/>					<tr>						<td>							<b>Select dataset</b>						</td>						<td>							<input type="checkbox" style="cursor:crosshair;" name="datasetdownload">								<xsl:attribute name="value"><xsl:value-of select="DataName"/></xsl:attribute>								<xsl:if test="@datasetdownload">									<xsl:attribute name="checked"/>								</xsl:if>							</input>						</td>					</tr>					<!--     <xsl:if test="ChildDataSetRef"> <tr>   <td valign="top"><b>Child data sets</b></td>   <td>	<xsl:apply-templates select="ChildDataSetRef"/></td> </tr>          </xsl:if>-->					<xsl:if test="File">						<tr>							<td valign="top">								<b>Files</b>							</td>							<td>								<xsl:apply-templates select="File"/>							</td>						</tr>					</xsl:if>				</table>			</xsl:otherwise>		</xsl:choose>	</xsl:template>	<xsl:template match="ParentDataSetRef">		<tr>			<td>				<b>Parent Data Set</b>			</td>			<td>				<a>					<xsl:attribute name="href">#<xsl:value-of select="."/></xsl:attribute>					<xsl:value-of select="."/>				</a>			</td>		</tr>	</xsl:template>	<xsl:template match="ChildDataSetRef">		<xsl:apply-templates select="//DataSet[DataName=current()/text()]"/>	</xsl:template>	<xsl:template match="RelatedStudy">		<tr>			<td>				<b>Related Study: <xsl:value-of select="RelationType"/>				</b>			</td>			<td>				<a>					<xsl:attribute name="href"><xsl:value-of select="StudyRef/@studyref"/></xsl:attribute>					<xsl:value-of select="StudyRef"/>				</a>			</td>		</tr>	</xsl:template>	<xsl:template match="RelatedDataSetRef">		<tr>			<td>				<b>					<xsl:value-of select="RelationType"/>				</b>			</td>			<td>				<xsl:apply-templates select="DataSetRef"/>			</td>		</tr>	</xsl:template>	<xsl:template match="DerivedFromDataSetRef">		<tr>			<td>				<b>Derived From Data Set:</b>			</td>			<td>				<a>					<xsl:apply-templates select="DataSetRef"/>				</a>			</td>		</tr>	</xsl:template>	<xsl:template match="DataSetRef">		<a>			<xsl:attribute name="href">#<xsl:value-of select="."/></xsl:attribute>			<xsl:value-of select="."/>		</a>	</xsl:template>	<xsl:template match="File">		<table border="1" width="100%">			<xsl:apply-templates select="DataName"/>			<xsl:apply-templates select="TypeOfData"/>			<xsl:apply-templates select="Status"/>			<xsl:apply-templates select="LogicalDescription"/>			<xsl:apply-templates select="FileFormat"/>			<xsl:apply-templates select="URL"/>		</table>	</xsl:template>	<xsl:template match="DataHolding/DataName">		<tr>			<td width="15%">				<b>Name of Data Holding</b>			</td>			<td>				<a>					<xsl:attribute name="name"><xsl:value-of select="."/></xsl:attribute>					<xsl:value-of select="."/>				</a>			</td>		</tr>	</xsl:template>	<xsl:template match="DataSet/DataName">		<tr>			<td width="50%">				<b>Name of Data Set</b>			</td>			<td>				<a>					<xsl:attribute name="name"><xsl:value-of select="."/></xsl:attribute>					<xsl:value-of select="."/>				</a>			</td>		</tr>	</xsl:template>	<xsl:template match="File/DataName">		<tr>			<td width="75%">				<b>File Name: </b>				<a>					<xsl:attribute name="name"><xsl:value-of select="."/></xsl:attribute>					<xsl:value-of select="."/>				</a>			</td>			<td>Select file:       <input type="checkbox" name="filedownload">					<xsl:attribute name="value"><xsl:value-of select="../URI"/></xsl:attribute>					<xsl:if test="../@filedownload">						<xsl:attribute name="checked"/>					</xsl:if>				</input>			</td>		</tr>	</xsl:template>	<xsl:template match="TypeOfData">		<tr>			<td>				<b>Type of Data</b>			</td>			<td>				<xsl:value-of select="."/>			</td>		</tr>	</xsl:template>	<xsl:template match="Status">		<tr>			<td>				<b>Data Status</b>			</td>			<td>				<xsl:value-of select="."/>			</td>		</tr>	</xsl:template>	<xsl:template match="DataHolding/LogicalDescription">		<tr>			<td valign="top">				<b>Data Holding Description</b>			</td>			<td>				<table>					<xsl:if test="text()">						<tr>							<xsl:value-of select="text()"/>						</tr>					</xsl:if>					<xsl:apply-templates/>				</table>			</td>		</tr>	</xsl:template>	<xsl:template match="DataSet/LogicalDescription">		<tr>			<td valign="top">				<b>Data Set Description</b>			</td>			<td>				<table>					<xsl:if test="text()">						<tr>							<xsl:value-of select="text()"/>						</tr>					</xsl:if>					<xsl:apply-templates/>				</table>			</td>		</tr>	</xsl:template>	<xsl:template match="File/LogicalDescription">		<tr>			<td valign="top">				<b>Data Set Description</b>			</td>			<td>				<table>					<xsl:if test="text()">						<tr>							<xsl:value-of select="text()"/>						</tr>					</xsl:if>					<xsl:apply-templates/>				</table>			</td>		</tr>	</xsl:template>	<xsl:template match="FileFormat">		<tr>			<td>				<b>File Format</b>			</td>			<td>				<xsl:value-of select="."/>			</td>		</tr>	</xsl:template>	<xsl:template match="Coverage">		<tr>			<td>				<b>Coverage</b>			</td>			<td>				<xsl:value-of select="."/>			</td>		</tr>	</xsl:template>	<xsl:template match="URL">		<tr>			<td>				<b>URL</b>			</td>			<td>				<xsl:value-of select="."/>			</td>		</tr>	</xsl:template>	<xsl:template match="Condition">		<tr>			<td>				<b>					<xsl:value-of select="ParamName"/>				</b>			</td>			<td>				<xsl:value-of select="ParamValue"/>			</td>			<td>				<xsl:value-of select="Units"/>			</td>			<td>				<xsl:value-of select="Range"/>			</td>		</tr>	</xsl:template>	<xsl:template match="Parameter">		<tr>			<td>				<b>					<xsl:value-of select="ParamName"/>				</b>			</td>			<td>				<xsl:value-of select="ParamValue"/>			</td>			<td>				<xsl:value-of select="Units"/>			</td>			<td>				<xsl:value-of select="Range"/>			</td>		</tr>	</xsl:template></xsl:stylesheet>