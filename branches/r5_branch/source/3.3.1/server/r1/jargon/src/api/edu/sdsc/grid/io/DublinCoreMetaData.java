//
//	Copyright (c) 2003 San Diego Supercomputer Center (SDSC),
//	University of California, San Diego (UCSD), San Diego, CA, USA.
//
//	Users and possessors of this source code are hereby granted a
//	nonexclusive, royalty-free copyright and design patent license
//	to use this code in individual software. License is not granted
//	for commercial resale, in whole or in part, without prior written
//	permission from SDSC/UCSD. This source is provided "as is"
//	without express or implied warranty of any kind.
//
//
// FILE
//	DublinCoreMetaData.java	- edu.sdsc.grid.io.DublinCoreMetaData
//
// CLASS HIERARCHY
//	java.lang.Object
//	 |
//	 +-.StandardMetaData
//	 			|
//	 			+-.DublinCoreMetaData
//
// PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;



/**
 * The metadata naming interface for dublin core metadata.
 * Some meta groups are standard regardless of implementation. To
 * handle standard metadata, we defined interfaces that are implemented
 * by protocol-specific metadata groups. The DublinCoreMetaData interface
 * contract is that the implementor supports meta data we might find
 * as common dublin core metadata, such as:
 *<ul>
 * <li> DC_DATA_NAME
 * <li> DC_COLLECTION
 * <li> DC_CONTRIBUTOR_TYPE
 * <li> DC_SUBJECT_CLASS
 * <li> DC_DESCRIPTION_TYPE
 * <li> DC_TYPE
 * <li> DC_SOURCE_TYPE
 * <li> DC_LANGUAGE
 * <li> DC_RELATION_TYPE
 * <li> DC_COVERAGE_TYPE
 * <li> DC_RIGHTS_TYPE
 * <li> DC_TITLE
 * <li> DC_CONTRIBUTOR_NAME
 * <li> DC_SUBJECT_NAME
 * <li> DC_DESCRIPTION
 * <li> DC_PUBLISHER
 * <li> DC_SOURCE
 * <li> DC_RELATED_DATA_DESCRIPTION
 * <li> DC_RELATED_DATA
 * <li> DC_RELATED_DIRECTORY
 * <li> DC_COVERAGE
 * <li> DC_RIGHTS
 * <li> DC_PUBLISHER_ADDR
 * <li> DC_CONTRIBUTOR_ADDR
 * <li> DC_CONTRIBUTOR_EMAIL
 * <li> DC_CONTRIBUTOR_PHONE
 * <li> DC_CONTRIBUTOR_WEB
 * <li> DC_CONTRIBUTOR_AFFILIATION
 *</ul>
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
*/
public interface DublinCoreMetaData extends StandardMetaData
{
//----------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------
	/** Dublin Core File Name same as file name  */
	public final static String DC_DATA_NAME = "Dublin Core: dataset";

	/** DC: Collection Name same as Collection Name */
	public final static String DC_COLLECTION = "Dublin Core: collection";

	/** DC: Contributor Type: Eg. Author, Illustrator */
	public final static String DC_CONTRIBUTOR_TYPE = "Dublin Core: contributor type";

	/** DC: Subject Classification */
	public final static String DC_SUBJECT_CLASS = "Dublin Core: subject class";

	/** DC: Type of Description */
	public final static String DC_DESCRIPTION_TYPE = "Dublin Core: description type";

	/** DC: Type of the Object */
	public final static String DC_TYPE = "Dublin Core: type";

	/** DC: Type of the Source */
	public final static String DC_SOURCE_TYPE = "Dublin Core: source type";

	/** DC: Language of the Object */
	public final static String DC_LANGUAGE = "Dublin Core: language";

	/** DC: Relation with another Object in  170,171  */
	public final static String DC_RELATION_TYPE = "Dublin Core: relation type";

	/** DC: Coverage Type */
	public final static String DC_COVERAGE_TYPE = "Dublin Core: coverage type";

	/** DC: Rights Type */
	public final static String DC_RIGHTS_TYPE = "Dublin Core: rights type";

	/** DC: Title of the Object */
	public final static String DC_TITLE = "Dublin Core: title";

	/** DC: Contributor Name. NOT same as  7  */
	public final static String DC_CONTRIBUTOR_NAME = "Dublin Core: contributor name";

	/** DC: Subject */
	public final static String DC_SUBJECT_NAME = "Dublin Core: subject name";

	/** DC: Description */
	public final static String DC_DESCRIPTION = "Dublin Core: description";

	/** DC: Publisher Name */
	public final static String DC_PUBLISHER = "Dublin Core: publisher";

	/** DC: Source Name */
	public final static String DC_SOURCE = "Dublin Core: source";

	/** DC: Related Data Description */
	public final static String DC_RELATED_DATA_DESCRIPTION = "Dublin Core: related data descriptionr";

	/** DC: Date Related to  152,153  */
	public final static String DC_RELATED_DATA = "Dublin Core: related data";

	/** DC:  */
	public final static String DC_RELATED_DIRECTORY = "Dublin Core: related directory";

	/** DC: Coverage Information */
	public final static String DC_COVERAGE = "Dublin Core: coverage";

	/** DC: Rights Information */
	public final static String DC_RIGHTS = "Dublin Core: rights";

	/** DC: Publisher Address */
	public final static String DC_PUBLISHER_ADDR = "Dublin Core: publisher addr";

	/** DC: Contributro Address */
	public final static String DC_CONTRIBUTOR_ADDR = "Dublin Core: contributor addr";

	/** DC: Contributor Email */
	public final static String DC_CONTRIBUTOR_EMAIL = "Dublin Core: contributor email";

	/** DC: Contributor Phone */
	public final static String DC_CONTRIBUTOR_PHONE = "Dublin Core: contributor phone";

	/** DC: Contributor Web Address */
	public final static String DC_CONTRIBUTOR_WEB = "Dublin Core: contributor web";

	/** DC: Contributor Affiliation */
	public final static String DC_CONTRIBUTOR_AFFILIATION = "Dublin Core: contributor affiliation";


//----------------------------------------------------------------------
// Fields
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------


}
