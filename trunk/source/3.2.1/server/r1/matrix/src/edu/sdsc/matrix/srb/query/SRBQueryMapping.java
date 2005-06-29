/* ====================================================================
 * SDSC Matrix Software License, Version 1.0
 *
 * Copyright (c) 2003 San Diego Supercomputer Center (SDSC), San Diego, USA.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that ALL the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        SDSC Matrix Project (http://www.npaci.edu/DICE/SRB/matrix/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "SDSC" and "SDSC Matrix" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact arun@sdsc.edu or srb-chat@sdsc.edu.
 *
 * 5. Products derived from this software may not be called "SDSC",
 *    nor may "SDSC" appear in their name, without prior written
 *    permission of the SDSC.
 *
 * 6. License is not granted for commercial resale, in whole or in part, 
 *	  without the prior written permission from SDSC.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL SDSC OR
 * ITS CONTRIBUTORS (SDSC Matrix Team) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software is intended to consist of voluntary contributions made by many
 * individuals for the SDSC Matrix Project.
 *
 * SDSC Matrix.  
 * Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
 *
 * Authors (Credits 02/SEP/2003):
 * Allen Ding (alding@sdsc.edu)
 * Arun Jagatheesan (arun@sdsc.edu)
 * Reena Mathew	(rmathew@sdsc.edu)
 * Erik Vandekieft (evk@sdsc.edu)
 */



package edu.sdsc.matrix.srb.query;

import edu.sdsc.grid.io.local.*;
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;
import java.util.Iterator;
import edu.sdsc.matrix.srb.util.MatrixUtil;
import java.util.StringTokenizer;

/*
 * 
 * @author rmathew
 *
 * This class is responsible for mapping
 * 		- Matrix Query Request to JARGON Query Request
 * 		- JARGON Query Results to Matrix Query Result
 */

public class SRBQueryMapping
{
	/*
	 * Contains the mappings of the Matrix Metadata fields to JARGON Metadata fields
	 * These fields obtained from SRBMetaDataTest.java from jargon code
	 */	
	private String [][] srbVariableMapping = 
						{
							{"OFFSET",SRBMetaDataSet.OFFSET},
							{"SIZE",SRBMetaDataSet.SIZE},
							{"DEFINABLE_METADATA_FOR_FILES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"USER_TYPE_NAME",SRBMetaDataSet.USER_TYPE_NAME},
							{"USER_NAME",SRBMetaDataSet.USER_NAME},
							{"USER_GROUP_NAME",SRBMetaDataSet.USER_GROUP_NAME},
							{"USER_ADDRESS",SRBMetaDataSet.USER_ADDRESS},
							{"ACCESS_CONSTRAINT",SRBMetaDataSet.ACCESS_CONSTRAINT},
							{"FILE_NAME",SRBMetaDataSet.FILE_NAME},
							
							{"FILE_ID",SRBMetaDataSet.FILE_ID},
							{"FILE_REPLICATION_ENUM",SRBMetaDataSet.FILE_REPLICATION_ENUM},
							{"FILE_NAME",SRBMetaDataSet.FILE_NAME},
							{"USER_GROUP_NAME",SRBMetaDataSet.USER_GROUP_NAME},
							{"FILE_TYPE_NAME",SRBMetaDataSet.FILE_TYPE_NAME},
							{"USER_TYPE_NAME",SRBMetaDataSet.USER_TYPE_NAME},
							{"USER_ADDRESS",SRBMetaDataSet.USER_ADDRESS},
							{"USER_NAME",SRBMetaDataSet.USER_NAME},
							{"ACCESS_CONSTRAINT",SRBMetaDataSet.ACCESS_CONSTRAINT},
							{"USER_DOMAIN",SRBMetaDataSet.USER_DOMAIN},
							{"PATH_NAME",SRBMetaDataSet.PATH_NAME},
							{"RESOURCE_NAME",SRBMetaDataSet.RESOURCE_NAME},
							{"RESOURCE_ADDRESS_NETPREFIX",SRBMetaDataSet.RESOURCE_ADDRESS_NETPREFIX},
							{"RESOURCE_TYPE_NAME",SRBMetaDataSet.RESOURCE_TYPE_NAME},
							{"MODIFICATION_DATE",SRBMetaDataSet.MODIFICATION_DATE},
							{"DIRECTORY_NAME",SRBMetaDataSet.DIRECTORY_NAME},
							{"USER_PHONE",SRBMetaDataSet.USER_PHONE},
							{"USER_EMAIL",SRBMetaDataSet.USER_EMAIL},
							{"SIZE",SRBMetaDataSet.SIZE},
							{"USER_AUDIT_TIME_STAMP",SRBMetaDataSet.USER_AUDIT_TIME_STAMP},
							{"USER_AUDIT_COMMENTS",SRBMetaDataSet.USER_AUDIT_COMMENTS},
							{"FILE_COMMENTS",SRBMetaDataSet.FILE_COMMENTS},
							{"AUDIT_ACTION_DESCRIPTION",SRBMetaDataSet.AUDIT_ACTION_DESCRIPTION},
							{"AUDIT_TIMESTAMP",SRBMetaDataSet.AUDIT_TIMESTAMP},
							{"AUDIT_COMMENTS",SRBMetaDataSet.AUDIT_COMMENTS},
							//{"GROUP_ACCESS_CONSTRAINT",SRBMetaDataSet.GROUP_ACCESS_CONSTRAINT},
							//{"ACCESS_GROUP_NAME",SRBMetaDataSet.ACCESS_GROUP_NAME},
							{"RESOURCE_DEFAULT_PATH",SRBMetaDataSet.RESOURCE_DEFAULT_PATH},
							{"PHYSICAL_RESOURCE_DEFAULT_PATH",SRBMetaDataSet.PHYSICAL_RESOURCE_DEFAULT_PATH},
							{"PHYSICAL_RESOURCE_NAME",SRBMetaDataSet.PHYSICAL_RESOURCE_NAME},
							{"PHYSICAL_RESOURCE_TYPE_NAME",SRBMetaDataSet.PHYSICAL_RESOURCE_TYPE_NAME},
							{"RESOURCE_REPLICATION_ENUM",SRBMetaDataSet.RESOURCE_REPLICATION_ENUM},
							{"FILE_ACCESS_LIST",SRBMetaDataSet.FILE_ACCESS_LIST},
							{"RESOURCE_ACCESS_LIST",SRBMetaDataSet.RESOURCE_ACCESS_LIST},
							{"FILE_IS_DELETED",SRBMetaDataSet.FILE_IS_DELETED},
							{"OWNER",SRBMetaDataSet.OWNER},
							{"TICKET_D",SRBMetaDataSet.TICKET_D},
							{"TICKET_BEGIN_TIME_D",SRBMetaDataSet.TICKET_BEGIN_TIME_D},
							{"TICKET_END_TIME_D",SRBMetaDataSet.TICKET_END_TIME_D},
							{"TICKET_ACC_COUNT_D",SRBMetaDataSet.TICKET_ACC_COUNT_D},
							{"TICKET_ACC_LIST_D",SRBMetaDataSet.TICKET_ACC_LIST_D},
							{"TICKET_OWNER_D",SRBMetaDataSet.TICKET_OWNER_D},
							{"TICKET_USER_D",SRBMetaDataSet.TICKET_USER_D},
							{"TICKET_C",SRBMetaDataSet.TICKET_C},
							{"TICKET_BEGIN_TIME_C",SRBMetaDataSet.TICKET_BEGIN_TIME_C},
							{"TICKET_END_TIME_C",SRBMetaDataSet.TICKET_END_TIME_C},
							{"TICKET_ACC_COUNT_C",SRBMetaDataSet.TICKET_ACC_COUNT_C},
							{"TICKET_ACC_LIST_C",SRBMetaDataSet.TICKET_ACC_LIST_C},
							{"TICKET_OWNER_C",SRBMetaDataSet.TICKET_OWNER_C},
							{"TICKET_USER_C",SRBMetaDataSet.TICKET_USER_C},
							{"TICKET_USER_DOMAIN_C",SRBMetaDataSet.TICKET_USER_DOMAIN_C},
							{"TICKET_USER_DOMAIN_D",SRBMetaDataSet.TICKET_USER_DOMAIN_D},
							{"TICKET_OWNER_DOMAIN_C",SRBMetaDataSet.TICKET_OWNER_DOMAIN_C},
							{"TICKET_OWNER_DOMAIN_D",SRBMetaDataSet.TICKET_OWNER_DOMAIN_D},
							{"PARENT_DIRECTORY_NAME",SRBMetaDataSet.PARENT_DIRECTORY_NAME},
							{"LOCATION_NAME",SRBMetaDataSet.LOCATION_NAME},
							{"IS_DIRTY",SRBMetaDataSet.IS_DIRTY},
							{"CONTAINER_MAX_SIZE",SRBMetaDataSet.CONTAINER_MAX_SIZE},
							{"CONTAINER_NAME",SRBMetaDataSet.CONTAINER_NAME},
							{"RESOURCE_CLASS",SRBMetaDataSet.RESOURCE_CLASS},
							{"MAX_OBJ_SIZE",SRBMetaDataSet.MAX_OBJ_SIZE},
							{"OFFSET",SRBMetaDataSet.OFFSET},
							{"CONTAINER_RESOURCE_NAME",SRBMetaDataSet.CONTAINER_RESOURCE_NAME},
							{"CONTAINER_RESOURCE_CLASS",SRBMetaDataSet.CONTAINER_RESOURCE_CLASS},
							{"CONTAINER_SIZE",SRBMetaDataSet.CONTAINER_SIZE},
							{"CONTAINER_LOG_RESOURCE_NAME",SRBMetaDataSet.CONTAINER_LOG_RESOURCE_NAME},
							{"OWNER_DOMAIN",SRBMetaDataSet.OWNER_DOMAIN},
							{"USER_DISTINGUISHED_NAME",SRBMetaDataSet.USER_DISTINGUISHED_NAME},
							{"USER_AUTHENTICATION_SCHEME",SRBMetaDataSet.USER_AUTHENTICATION_SCHEME},
							{"SERVER_LOCATION",SRBMetaDataSet.SERVER_LOCATION},
							{"SERVER_NETPREFIX",SRBMetaDataSet.SERVER_NETPREFIX},
							{"CONTAINER_REPLICATION_ENUM",SRBMetaDataSet.CONTAINER_REPLICATION_ENUM},
							{"FILE_ANNOTATION_USERNAME",SRBMetaDataSet.FILE_ANNOTATION_USERNAME},
							{"FILE_ANNOTATION_USERDOMAIN",SRBMetaDataSet.FILE_ANNOTATION_USERDOMAIN},
							{"FILE_ANNOTATION",SRBMetaDataSet.FILE_ANNOTATION},
							{"FILE_ANNOTATION_TIMESTAMP",SRBMetaDataSet.FILE_ANNOTATION_TIMESTAMP},
							{"CREATION_DATE",SRBMetaDataSet.CREATION_DATE},
							{"FILE_COMMENTS_TIMESTAMP",SRBMetaDataSet.FILE_COMMENTS_TIMESTAMP},
							{"FILE_ANNOTATION_POSITION",SRBMetaDataSet.FILE_ANNOTATION_POSITION},
							{"FILE_ACCESS_PRIVILEGE",SRBMetaDataSet.FILE_ACCESS_PRIVILEGE},
							{"RESOURCE_MAX_LATENCY",SRBMetaDataSet.RESOURCE_MAX_LATENCY},
							{"RESOURCE_MIN_LATENCY",SRBMetaDataSet.RESOURCE_MIN_LATENCY},
							{"RESOURCE_BANDWIDTH",SRBMetaDataSet.RESOURCE_BANDWIDTH},
							{"RESOURCE_MAX_CONCURRENCY",SRBMetaDataSet.RESOURCE_MAX_CONCURRENCY},
							{"RESOURCE_NUM_OF_HIERARCHIES",SRBMetaDataSet.RESOURCE_NUM_OF_HIERARCHIES},
							{"RESOURCE_NUM_OF_STRIPES",SRBMetaDataSet.RESOURCE_NUM_OF_STRIPES},
							{"RESOURCE_CAPACITY",SRBMetaDataSet.RESOURCE_CAPACITY},
							{"RESOURCE_DESCRIPTION",SRBMetaDataSet.RESOURCE_DESCRIPTION},
							{"FILE_CLASS_NAME",SRBMetaDataSet.FILE_CLASS_NAME},
							{"FILE_CLASS_TYPE",SRBMetaDataSet.FILE_CLASS_TYPE},
							{"STRUCTURED_METADATA_TYPE",SRBMetaDataSet.STRUCTURED_METADATA_TYPE},
							{"STRUCTURED_METADATA_COMMENTS",SRBMetaDataSet.STRUCTURED_METADATA_COMMENTS},
							{"STRUCTURED_METADATA_FILE_NAME",SRBMetaDataSet.STRUCTURED_METADATA_FILE_NAME},
							{"INTERNAL_STRUCTURED_METADATA",SRBMetaDataSet.INTERNAL_STRUCTURED_METADATA},
							{"INDEX_NAME_FOR_FILE",SRBMetaDataSet.INDEX_NAME_FOR_FILE},
							{"INDEX_NAME_FOR_DATATYPE",SRBMetaDataSet.INDEX_NAME_FOR_DATATYPE},
							{"INDEX_NAME_FOR_DIRECTORY",SRBMetaDataSet.INDEX_NAME_FOR_DIRECTORY},
							{"METHOD_NAME_FOR_FILE",SRBMetaDataSet.METHOD_NAME_FOR_FILE},
							{"METHOD_NAME_FOR_DATATYPE",SRBMetaDataSet.METHOD_NAME_FOR_DATATYPE},
							{"METHOD_NAME_FOR_DIRECTORY",SRBMetaDataSet.METHOD_NAME_FOR_DIRECTORY},
							{"IX_DIRECTORY_NAME_FOR_FILE",SRBMetaDataSet.IX_DIRECTORY_NAME_FOR_FILE},
							{"IX_DIRECTORY_NAME_FOR_DATATYPE",SRBMetaDataSet.IX_DIRECTORY_NAME_FOR_DATATYPE},
							{"IX_DIRECTORY_NAME_FOR_DIRECTORY",SRBMetaDataSet.IX_DIRECTORY_NAME_FOR_DIRECTORY},
							{"METHOD_DIRECTORY_NAME_FOR_FILE",SRBMetaDataSet.METHOD_DIRECTORY_NAME_FOR_FILE},
							{"METHOD_DIRECTORY_NAME_FOR_DATATYPE",SRBMetaDataSet.METHOD_DIRECTORY_NAME_FOR_DATATYPE},
							{"METHOD_DIRECTORY_NAME_FOR_DIRECTORY",SRBMetaDataSet.METHOD_DIRECTORY_NAME_FOR_DIRECTORY},
							{"IX_DATATYPE_FOR_FILE",SRBMetaDataSet.IX_DATATYPE_FOR_FILE},
							{"IX_DATATYPE_FOR_DATATYPE",SRBMetaDataSet.IX_DATATYPE_FOR_DATATYPE},
							{"IX_DATATYPE_FOR_DIRECTORY",SRBMetaDataSet.IX_DATATYPE_FOR_DIRECTORY},
							{"METHOD_DATATYPE_FOR_FILE",SRBMetaDataSet.METHOD_DATATYPE_FOR_FILE},
							{"METHOD_DATATYPE_FOR_DATATYPE",SRBMetaDataSet.METHOD_DATATYPE_FOR_DATATYPE},
							{"METHOD_DATATYPE_FOR_DIRECTORY",SRBMetaDataSet.METHOD_DATATYPE_FOR_DIRECTORY},
							{"STRUCTURED_METADATA_DIRECTORY_NAME",SRBMetaDataSet.STRUCTURED_METADATA_DIRECTORY_NAME},
							{"IX_LOCATION_FOR_FILE",SRBMetaDataSet.IX_LOCATION_FOR_FILE},
							{"IX_LOCATION_FOR_DATATYPE",SRBMetaDataSet.IX_LOCATION_FOR_DATATYPE},
							{"IX_LOCATION_FOR_DIRECTORY",SRBMetaDataSet.IX_LOCATION_FOR_DIRECTORY},
							{"METADATA_NUM",SRBMetaDataSet.METADATA_NUM},
							{"DEFINABLE_METADATA_FOR_FILES0",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0},
							{"DEFINABLE_METADATA_FOR_FILES1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES1},
							{"DEFINABLE_METADATA_FOR_FILES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"DEFINABLE_METADATA_FOR_FILES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"DEFINABLE_METADATA_FOR_FILES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"DEFINABLE_METADATA_FOR_FILES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"DEFINABLE_METADATA_FOR_FILES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"DEFINABLE_METADATA_FOR_FILES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"DEFINABLE_METADATA_FOR_FILES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"DEFINABLE_METADATA_FOR_FILES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"INTEGER_DEFINABLE_METADATA0",SRBMetaDataSet.INTEGER_DEFINABLE_METADATA0},
							{"INTEGER_DEFINABLE_METADATA1",SRBMetaDataSet.INTEGER_DEFINABLE_METADATA1},
							{"OWNER_EMAIL",SRBMetaDataSet.OWNER_EMAIL},
							{"AUDIT_USER",SRBMetaDataSet.AUDIT_USER},
							{"AUDIT_USER_DOMAIN",SRBMetaDataSet.AUDIT_USER_DOMAIN},
							{"CONTAINER_FOR_DIRECTORY",SRBMetaDataSet.CONTAINER_FOR_DIRECTORY},
							{"METADATA_NUM_DIRECTORY",SRBMetaDataSet.METADATA_NUM_DIRECTORY},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES0",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES0},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES1},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES},
							{"INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES0",SRBMetaDataSet.INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES0},
							{"INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES1",SRBMetaDataSet.INTEGER_DEFINABLE_METADATA_FOR_DIRECTORIES1},
							{"FILE_ACCESS_ID",SRBMetaDataSet.FILE_ACCESS_ID},
							{"USER_ID",SRBMetaDataSet.USER_ID},
							{"FILE_LAST_ACCESS_TIMESTAMP",SRBMetaDataSet.FILE_LAST_ACCESS_TIMESTAMP},
							{"DIRECTORY_OWNER_NAME",SRBMetaDataSet.DIRECTORY_OWNER_NAME},
							{"DIRECTORY_CREATE_TIMESTAMP",SRBMetaDataSet.DIRECTORY_CREATE_TIMESTAMP},
							{"DIRECTORY_COMMENTS",SRBMetaDataSet.DIRECTORY_COMMENTS},
							{"DC_FILE_NAME",SRBMetaDataSet.DC_FILE_NAME},
							{"DC_DIRECTORY",SRBMetaDataSet.DC_DIRECTORY},
							{"DC_CONTRIBUTOR_TYPE",SRBMetaDataSet.DC_CONTRIBUTOR_TYPE},
							{"DC_SUBJECT_CLASS",SRBMetaDataSet.DC_SUBJECT_CLASS},
							{"DC_DESCRIPTION_TYPE",SRBMetaDataSet.DC_DESCRIPTION_TYPE},
							{"DC_TYPE",SRBMetaDataSet.DC_TYPE},
							{"DC_SOURCE_TYPE",SRBMetaDataSet.DC_SOURCE_TYPE},
							{"DC_LANGUAGE",SRBMetaDataSet.DC_LANGUAGE},
							{"DC_RELATION_TYPE",SRBMetaDataSet.DC_RELATION_TYPE},
							{"DC_COVERAGE_TYPE",SRBMetaDataSet.DC_COVERAGE_TYPE},
							{"DC_RIGHTS_TYPE",SRBMetaDataSet.DC_RIGHTS_TYPE},
							{"DC_TITLE",SRBMetaDataSet.DC_TITLE},
							{"DC_CONTRIBUTOR_NAME",SRBMetaDataSet.DC_CONTRIBUTOR_NAME},
							{"DC_SUBJECT_NAME",SRBMetaDataSet.DC_SUBJECT_NAME},
							{"DC_DESCRIPTION",SRBMetaDataSet.DC_DESCRIPTION},
							{"DC_PUBLISHER",SRBMetaDataSet.DC_PUBLISHER},
							{"DC_SOURCE",SRBMetaDataSet.DC_SOURCE},
							{"DC_RELATED_FILE_DESCRIPTION",SRBMetaDataSet.DC_RELATED_FILE_DESCRIPTION},
							{"DC_RELATED_DATA",SRBMetaDataSet.DC_RELATED_DATA},
							{"DC_RELATED_DIRECTORY",SRBMetaDataSet.DC_RELATED_DIRECTORY},
							{"DC_COVERAGE",SRBMetaDataSet.DC_COVERAGE},
							{"DC_RIGHTS",SRBMetaDataSet.DC_RIGHTS},
							{"DC_PUBLISHER_ADDR",SRBMetaDataSet.DC_PUBLISHER_ADDR},
							{"DC_CONTRIBUTOR_ADDR",SRBMetaDataSet.DC_CONTRIBUTOR_ADDR},
							{"DC_CONTRIBUTOR_EMAIL",SRBMetaDataSet.DC_CONTRIBUTOR_EMAIL},
							{"DC_CONTRIBUTOR_PHONE",SRBMetaDataSet.DC_CONTRIBUTOR_PHONE},
							{"DC_CONTRIBUTOR_WEB",SRBMetaDataSet.DC_CONTRIBUTOR_WEB},
							{"DC_CONTRIBUTOR_AFFILIATION",SRBMetaDataSet.DC_CONTRIBUTOR_AFFILIATION},
							{"COLL_ACCS_ID",SRBMetaDataSet.COLL_ACCS_ID},
							{"COLL_ACCS_USER_ID",SRBMetaDataSet.COLL_ACCS_USER_ID},
							{"DATA_ACCS_ID",SRBMetaDataSet.DATA_ACCS_ID},
							{"DATA_ACCS_USER_ID",SRBMetaDataSet.DATA_ACCS_USER_ID},
							/*		  
							  cannot be accessed from outside package
							{"DEFINABLE_METADATA_FOR_FILES0_1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0_1},
							{"DEFINABLE_METADATA_FOR_FILES0_2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0_2},
							{"DEFINABLE_METADATA_FOR_FILES0_3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0_3},
							{"DEFINABLE_METADATA_FOR_FILES0_4",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0_4},
							{"DEFINABLE_METADATA_FOR_FILES1_1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES1_1},
							{"DEFINABLE_METADATA_FOR_FILES1_2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES1_2},
							{"DEFINABLE_METADATA_FOR_FILES1_3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES1_3},
							{"DEFINABLE_METADATA_FOR_FILES1_4",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES1_4},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES0_1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES0_1},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES0_2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES0_2},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES0_3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES0_3},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES0_4",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES0_4},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES1_1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES1_1},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES1_2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES1_2},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES1_3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES1_3},
							{"DEFINABLE_METADATA_FOR_DIRECTORIES1_4",SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES1_4},
							*/
							//{"C_TICKET_RECURSIVE",SRBMetaDataSet.C_TICKET_RECURSIVE},
							/* cannot be accessed from outside package
							{"DEFINABLE_METADATA_FOR_RESOURCES0_1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES0_1},
							{"DEFINABLE_METADATA_FOR_RESOURCES0_2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES0_2},
							{"DEFINABLE_METADATA_FOR_RESOURCES0_3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES0_3},
							{"DEFINABLE_METADATA_FOR_RESOURCES1_1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES1_1},
							{"DEFINABLE_METADATA_FOR_RESOURCES1_2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES1_2},
							{"DEFINABLE_METADATA_FOR_RESOURCES1_3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES1_3},
							{"DEFINABLE_METADATA_FOR_USERS0_1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS0_1},
							{"DEFINABLE_METADATA_FOR_USERS0_2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS0_2},
							{"DEFINABLE_METADATA_FOR_USERS0_3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS0_3},
							{"DEFINABLE_METADATA_FOR_USERS1_1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS1_1},
							{"DEFINABLE_METADATA_FOR_USERS1_2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS1_2},
							{"DEFINABLE_METADATA_FOR_USERS1_3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS1_3},
							*/
							{"TAPE_NUMBER",SRBMetaDataSet.TAPE_NUMBER},
							{"TAPE_OWNER",SRBMetaDataSet.TAPE_OWNER},
							{"TAPE_TYPE_VAL",SRBMetaDataSet.TAPE_TYPE_VAL},
							{"TAPE_LIBINX",SRBMetaDataSet.TAPE_LIBINX},
							{"TAPE_FULLFLAG",SRBMetaDataSet.TAPE_FULLFLAG},
							{"TAPE_CURRENT_FILE_SEQNUM",SRBMetaDataSet.TAPE_CURRENT_FILE_SEQNUM},
							{"TAPE_CURRENT_ABS_POSITION",SRBMetaDataSet.TAPE_CURRENT_ABS_POSITION},
							{"TAPE_BYTES_WRITTEN",SRBMetaDataSet.TAPE_BYTES_WRITTEN},
							{"FILE_VER_NUM",SRBMetaDataSet.FILE_VER_NUM},
							{"FILE_LOCK_NUM",SRBMetaDataSet.FILE_LOCK_NUM},
							{"FILE_LOCK_DESCRIPTION",SRBMetaDataSet.FILE_LOCK_DESCRIPTION},
							{"FILE_LOCK_OWNER_NAME",SRBMetaDataSet.FILE_LOCK_OWNER_NAME},
							{"FILE_LOCK_OWNER_DOMAIN",SRBMetaDataSet.FILE_LOCK_OWNER_DOMAIN},
							{"DIRECTORY_OWNER_DOMAIN",SRBMetaDataSet.DIRECTORY_OWNER_DOMAIN},
							//{"C_TICKET_DIRECTORY_NAME",SRBMetaDataSet.C_TICKET_DIRECTORY_NAME},
							//{"C_TICKET_DIRECTORY_CREATE_TIMESTAMP",SRBMetaDataSet.C_TICKET_DIRECTORY_CREATE_TIMESTAMP},
							//{"C_TICKET_DIRECTORY_COMMENTS",SRBMetaDataSet.C_TICKET_DIRECTORY_COMMENTS},
							{"DIRECTORY_ANNOTATION_USERNAME",SRBMetaDataSet.DIRECTORY_ANNOTATION_USERNAME},
							{"DIRECTORY_ANNOTATION_USERDOMAIN",SRBMetaDataSet.DIRECTORY_ANNOTATION_USERDOMAIN},
							{"DIRECTORY_ANNOTATION",SRBMetaDataSet.DIRECTORY_ANNOTATION},
							{"DIRECTORY_ANNOTATION_TIMESTAMP",SRBMetaDataSet.DIRECTORY_ANNOTATION_TIMESTAMP},
							{"DIRECTORY_ANNOTATION_TYPE",SRBMetaDataSet.DIRECTORY_ANNOTATION_TYPE},
							{"FILE_GROUP_ID",SRBMetaDataSet.FILE_GROUP_ID},
							{"FILE_HIDE",SRBMetaDataSet.FILE_HIDE},
							{"FILE_CHECKSUM",SRBMetaDataSet.FILE_CHECKSUM},
							{"FILE_AUDITFLAG",SRBMetaDataSet.FILE_AUDITFLAG},
							{"FILE_LOCK_EXPIRY",SRBMetaDataSet.FILE_LOCK_EXPIRY},
							{"FILE_PIN_VAL",SRBMetaDataSet.FILE_PIN_VAL},
							{"FILE_PIN_OWNER_NAME",SRBMetaDataSet.FILE_PIN_OWNER_NAME},
							{"FILE_PIN_OWNER_DOMAIN",SRBMetaDataSet.FILE_PIN_OWNER_DOMAIN},
							{"FILE_PIN_EXPIRY",SRBMetaDataSet.FILE_PIN_EXPIRY},
							{"FILE_EXPIRY_DATE",SRBMetaDataSet.FILE_EXPIRY_DATE},
							{"FILE_IS_COMPRESSED",SRBMetaDataSet.FILE_IS_COMPRESSED},
							{"FILE_IS_ENCRYPTED",SRBMetaDataSet.FILE_IS_ENCRYPTED},
							{"FILE_EXPIRE_DATE_2",SRBMetaDataSet.FILE_EXPIRE_DATE_2},
							{"NEW_AUDIT_ACTION_DESCRIPTION",SRBMetaDataSet.NEW_AUDIT_ACTION_DESCRIPTION},
							{"NEW_AUDIT_TIMESTAMP",SRBMetaDataSet.NEW_AUDIT_TIMESTAMP},
							{"NEW_AUDIT_COMMENTS",SRBMetaDataSet.NEW_AUDIT_COMMENTS},
							{"DEFINABLE_METADATA_FOR_RESOURCES0",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES0},
							{"DEFINABLE_METADATA_FOR_RESOURCES1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES1},
							{"DEFINABLE_METADATA_FOR_RESOURCES2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES2},
							{"DEFINABLE_METADATA_FOR_RESOURCES3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES3},
							{"DEFINABLE_METADATA_FOR_RESOURCES4",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES4},
							{"DEFINABLE_METADATA_FOR_RESOURCES5",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES5},
							{"DEFINABLE_METADATA_FOR_RESOURCES6",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES6},
							{"DEFINABLE_METADATA_FOR_RESOURCES7",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES7},
							{"DEFINABLE_METADATA_FOR_RESOURCES8",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES8},
							{"DEFINABLE_METADATA_FOR_RESOURCES9",SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES9},
							{"DEFINABLE_INTEGER_METADATA_RESOURCE0",SRBMetaDataSet.DEFINABLE_INTEGER_METADATA_RESOURCE0},
							{"METADATA_NUM_RESOURCE",SRBMetaDataSet.METADATA_NUM_RESOURCE},
							{"METADATA_NUM_USER",SRBMetaDataSet.METADATA_NUM_USER},
							{"COMPOUND_RESOURCE_NAME",SRBMetaDataSet.COMPOUND_RESOURCE_NAME},
							{"FILE_SEG_NUM",SRBMetaDataSet.FILE_SEG_NUM},
							{"INT_REPLICATION_ENUM",SRBMetaDataSet.INT_REPLICATION_ENUM},
							{"INT_SEG_NUM",SRBMetaDataSet.INT_SEG_NUM},
							{"INT_PATH_NAME",SRBMetaDataSet.INT_PATH_NAME},
							{"INT_RESOURCE_NAME",SRBMetaDataSet.INT_RESOURCE_NAME},
							{"INT_RESOURCE_ADDRESS_NETPREFIX",SRBMetaDataSet.INT_RESOURCE_ADDRESS_NETPREFIX},
							{"INT_RESOURCE_TYPE_NAME",SRBMetaDataSet.INT_RESOURCE_TYPE_NAME},
							{"INT_IS_DIRTY",SRBMetaDataSet.INT_IS_DIRTY},
							{"INT_RESOURCE_CLASS",SRBMetaDataSet.INT_RESOURCE_CLASS},
							{"INT_OFFSET",SRBMetaDataSet.INT_OFFSET},
							{"INT_SIZE",SRBMetaDataSet.INT_SIZE},
							{"COMP_OBJ_USER_NAME",SRBMetaDataSet.COMP_OBJ_USER_NAME},
							{"COMP_OBJ_USER_DOMAIN",SRBMetaDataSet.COMP_OBJ_USER_DOMAIN},
							{"INT_PATH_NAME_FOR_REPL",SRBMetaDataSet.INT_PATH_NAME_FOR_REPL},
							{"DIRECTORY_LINK_NUM",SRBMetaDataSet.DIRECTORY_LINK_NUM},
							{"REAL_DIRECTORY_NAME",SRBMetaDataSet.REAL_DIRECTORY_NAME},
							{"FILE_CONTAINER_NAME",SRBMetaDataSet.FILE_CONTAINER_NAME},
							{"DIRECTORY_CONTAINER_NAME",SRBMetaDataSet.DIRECTORY_CONTAINER_NAME},
							{"DEFINABLE_METADATA_FOR_USERS0",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS0},
							{"DEFINABLE_METADATA_FOR_USERS1",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS1},
							{"DEFINABLE_METADATA_FOR_USERS2",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS2},
							{"DEFINABLE_METADATA_FOR_USERS3",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS3},
							{"DEFINABLE_METADATA_FOR_USERS4",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS4},
							{"DEFINABLE_METADATA_FOR_USERS5",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS5},
							{"DEFINABLE_METADATA_FOR_USERS6",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS6},
							{"DEFINABLE_METADATA_FOR_USERS7",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS7},
							{"DEFINABLE_METADATA_FOR_USERS8",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS8},
							{"DEFINABLE_METADATA_FOR_USERS9",SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS9},
							{"UDIMD_USER0",SRBMetaDataSet.UDIMD_USER0},
							{"NONDISTINCT",SRBMetaDataSet.NONDISTINCT},
							{"USERDEFFUNC01",SRBMetaDataSet.USERDEFFUNC01},
							{"USERDEFFUNC02",SRBMetaDataSet.USERDEFFUNC02},
							{"USERDEFFUNC03",SRBMetaDataSet.USERDEFFUNC03},
							{"USERDEFFUNC04",SRBMetaDataSet.USERDEFFUNC04},
							{"USERDEFFUNC05",SRBMetaDataSet.USERDEFFUNC05},
							
							// For Retrieving User Defined Attribute Names
							{"USER_DEFINED_ATTRIBUTES_NAMES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},						
							{"USER_DEFINED_ATTRIBUTES_VALUES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES},
							{"USER_DEFINED_ATTRIBUTES_UNITS",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES}
						};
	
	/*
	 * Contains the mapping of the JARGON metadata field to Matrix selection field
	 * Key for the hashmap is the JARGON Metadata field name 
	 * Example:  SRBMetaDataSet.OFFSET = offset
	 * 
	 * Value for the hashmap is the Matrix selection field in the query request
	 * Example: $coll/SystemMetadata/OFFSET
	 */
	public HashMap srbFieldToMatrixFieldMapping = null;
	
	/*
	 * Return String used as a template to create matrix query result from jargon query results 
	 */
	public String queryReturnExpression = null;
	
	/*
	 * Flag to indicate query for retrieving all AVU Pairs
	 */
	public boolean queryForAllAVUPairs = false;
	
	/*
	 * Public Constructor
	 */	
	public SRBQueryMapping()
	{
			
	}
	
	/*
	 * Maps the Matrix Query Request to JARGON/SRB Query Request
	 * - MatrixQueryWrapper details to SRBQueryWrapper
	 */	
	public SRBQueryWrapper mapMatrixQueryToSRBQuery(MatrixQueryWrapper matrixQuery)									
	{
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixQueryToSRBQuery():BEGIN",MatrixUtil.DEBUG_LOG);
		SRBQueryWrapper srbQueryParameters = null;
		GeneralFileSystem fileSystem = null;
		GeneralFile file = null;
		MetaDataCondition conditionArray[] = null;
		MetaDataSelect selectArray[] = null;
		
		boolean queryForFileSystem = false;
		String fileName = null;
				
		// Determine query for file or filesystem
		// ??????? HOW ?
		// NOT USED 
		if(queryForFileSystem)
		{
			// File System from SRBConnection ?????
			fileName = getFileNameForQuery(matrixQuery.getMatrixQueryForClause());
		}
		else // query for file
		{
			fileName = getFileNameForQuery(matrixQuery.getMatrixQueryForClause());			
		}
		
		
		// Map Query Conditions
		conditionArray = mapMatrixConditionToSRBCondition(matrixQuery.getMatrixQueryWhereClause());
		
		// Map Selection Array
		selectArray =  mapMatrixReturnClauseToSRBSelection(matrixQuery.getMatrixQueryReturnClause());
		
		// Set SRB Query Wrapper
		srbQueryParameters = new SRBQueryWrapper();
		srbQueryParameters.setQueryFileNameParameter(fileName);
		srbQueryParameters.setGeneralFileSystem(fileSystem);
		srbQueryParameters.setGeneralFile(file);
		srbQueryParameters.setMetaDataCondition(conditionArray);
		srbQueryParameters.setMetaDataSelect(selectArray);
		srbQueryParameters.setQueryForFileSystem(queryForFileSystem);
		
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixQueryToSRBQuery():END",MatrixUtil.DEBUG_LOG);
		return (srbQueryParameters);
											
	}
	
	
	/*
	 * Return the file/directory name used for query
	 * Passed as a function parameter in matrix query request
	 */
	public String getFileNameForQuery(MatrixQueryForClause forClause)
	{
		MatrixUtil.toLog("SRBQueryMapping-getFileNameForQuery():BEGIN",MatrixUtil.DEBUG_LOG);
		String fileName = null;
		fileName = forClause.getFunctionParameter();
		MatrixUtil.toLog("SRBQueryMapping-getFileNameForQuery():fileName:"+fileName,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("SRBQueryMapping-getFileNameForQuery():END",MatrixUtil.DEBUG_LOG);
		return fileName;
	}
	
	/*
	 * To create Condition for BETWEEN and NOT BETWEEN operator
	 * ASSUMPTION - Parameter is int as the grammar currently 
	 * doesn't recognise Numeric Literal - Integer or Double
	 * 
	 * ASSUMPTION - Condition Value of Form (10 and 3000)
	 */	 
	private MetaDataCondition createConditionForBetweenOrNotBetweenOperator(String variableName, int srbOperator,String conditionValue)
	{
		MatrixUtil.toLog("SRBQueryMapping-createConditionForBetweenOrNotBetweenOperator():BEGIN",MatrixUtil.DEBUG_LOG);	
		MetaDataCondition betCond = null;
		String[] rangeValues = null;
		int condValue1 = -99999;
		int condValue2 = -99999;
		String tempCondValue = new String(conditionValue.substring(1,conditionValue.length()-1));
		
		// Parse Range Values
		MatrixUtil.toLog("SRBQueryMapping-createConditionForBetweenOrNotBetweenOperator():conditionValue:"+conditionValue,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("SRBQueryMapping-createConditionForBetweenOrNotBetweenOperator():tempCondValue:"+tempCondValue,MatrixUtil.DEBUG_LOG);
		
		rangeValues = tempCondValue.split("and");
		MatrixUtil.toLog("SRBQueryMapping-createConditionForBetweenOrNotBetweenOperator():rangeValues-length:"+rangeValues.length,MatrixUtil.DEBUG_LOG);
		for(int i=0;i<rangeValues.length;i++)
		{
			MatrixUtil.toLog("SRBQueryMapping-createConditionForBetweenOrNotBetweenOperator():rangeValues["+i+"]:"+rangeValues[i],MatrixUtil.DEBUG_LOG);
			int paramInt = Integer.parseInt(rangeValues[i]);
			if(i==0)
				condValue1 = paramInt;
			else if(i==1)
				condValue2 = paramInt;
		}
		
		betCond = MetaDataSet.newCondition(variableName, srbOperator, condValue1,condValue2 );
		MatrixUtil.toLog("SRBQueryMapping-createConditionForBetweenOrNotBetweenOperator():END",MatrixUtil.DEBUG_LOG);
		return(betCond);
	}
	
	/*
	 * Utility method to convert a comma delimited string to a string array
	 * For example if the input string is "Matrix-demo.sdsc,Matrix-demo.ucsd"
	 * the return value will be the string array
	 * {"Matrix-demo.sdsc","Matrix-demo.ucsd"}
	 */
	private String[] convertCommaDelimitedStringToStringArray(String valueString)
	{
		MatrixUtil.toLog("SRBQueryMapping-convertCommaDelimitedStringToStringArray():BEGIN",MatrixUtil.DEBUG_LOG);
		String[] valueList = null;
		StringTokenizer tokenizer = null;
				
		if(valueString!=null)
		{
			tokenizer = new StringTokenizer(valueString, ",");		
			if(tokenizer!=null && tokenizer.countTokens()>0)
			{
				valueList = new String[tokenizer.countTokens()];
				MatrixUtil.toLog("SRBQueryMapping-convertCommaDelimitedStringToStringArray():valueList - length"+valueList.length,MatrixUtil.DEBUG_LOG);
				int index = 0;
				while(tokenizer.hasMoreTokens())
				{
					valueList[index]= tokenizer.nextToken();
					MatrixUtil.toLog("SRBQueryMapping-convertCommaDelimitedStringToStringArray():valueList"+"["+index+"]"+valueList[index],MatrixUtil.DEBUG_LOG); 
					index++;
				}
			}
		}
			
		MatrixUtil.toLog("SRBQueryMapping-convertCommaDelimitedStringToStringArray():END",MatrixUtil.DEBUG_LOG);
		 return (valueList);
	}
	
	
	/*
	 * Method creates the list for System and User Defined Data
	 * Create temp list of System and User Defined conditions
	 * Joins the two lists to final list for the query
	 */
	public MetaDataCondition[] mapMatrixConditionToSRBCondition(MatrixQueryWhereClause whereClause)
	{
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixConditionToSRBCondition():BEGIN",MatrixUtil.DEBUG_LOG);
		MetaDataCondition[] srbConditions = null;
		MetaDataCondition tempCondition =null;
		MetaDataCondition betCondition =null;
		
		// Temp lists to hold system and user defined conditions
		// List contents will be copied to final MetaDataConditions[] list
		ArrayList srbUserDefinedConditionList = null;
		ArrayList srbSystemDefinedConditionList = null;
		
		SRBUserDefinedCondition  userDefCond = null;
		
		ArrayList conditionClauseList = whereClause.getConditionList();
		
		// For list of values - IN and NOT IN operators
		String srbValueList[] = null;
		
		
		//srbConditions =  new MetaDataCondition[conditionClauseList.size()];
		for(int i=0;i<conditionClauseList.size();i++)
		{
			MatrixQueryConditionClause conditionClause = (MatrixQueryConditionClause) conditionClauseList.get(i);
			SRBVariable srbFieldName = 	mapMatrixConditionVariable(conditionClause.getConditionVariable());
			
			
			int srbOperator = mapMatrixOperator(conditionClause.getOperator());
			
			if(srbOperator==MetaDataCondition.IN || srbOperator==MetaDataCondition.NOT_IN)
				srbValueList = convertCommaDelimitedStringToStringArray(conditionClause.getConditionValue().substring(1,conditionClause.getConditionValue().length()-1));
			else if(srbOperator==MetaDataCondition.BETWEEN || srbOperator==MetaDataCondition.NOT_BETWEEN)
				betCondition = createConditionForBetweenOrNotBetweenOperator(srbFieldName.variableName,srbOperator,conditionClause.getConditionValue()); 
			
			String srbValue = mapMatrixConditionValue(conditionClause.getConditionValue());
			if(srbFieldName.srbSystemVariable)
			{
				if(srbSystemDefinedConditionList==null)
					srbSystemDefinedConditionList = new ArrayList();
				
				if(srbOperator==MetaDataCondition.IN || srbOperator==MetaDataCondition.NOT_IN)
				{
					tempCondition = MetaDataSet.newCondition(srbFieldName.variableName, srbOperator, srbValueList );
				}
				else if(srbOperator==MetaDataCondition.BETWEEN || srbOperator==MetaDataCondition.NOT_BETWEEN)
				{
					tempCondition = betCondition;
				}
				else
				{
				
					/*
					 * This is being done temporarily as the grammar cannot tell
					 * whether the parameter is a String or Numeric Literal 
					 */
					try
					{
						int paramInt = Integer.parseInt(srbValue);
						tempCondition = MetaDataSet.newCondition(srbFieldName.variableName, srbOperator, paramInt );
						MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() -INT PARAM VALUE",MatrixUtil.DEBUG_LOG);  
					}
					catch(Exception e)
					{
						tempCondition = MetaDataSet.newCondition(srbFieldName.variableName, srbOperator, srbValue );	
						MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() -STRING PARAM VALUE",MatrixUtil.DEBUG_LOG);  
					}
					
				}
					
						
				srbSystemDefinedConditionList.add(tempCondition);
				MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() - System Condition:"+srbFieldName.variableName+conditionClause.getOperator()+srbValue,MatrixUtil.DEBUG_LOG);
			}
			else
			{
				// If User Defined Variable build metadata table
				if(srbUserDefinedConditionList==null)
					srbUserDefinedConditionList = new ArrayList();  
					
				if(srbOperator==MetaDataCondition.IN || srbOperator==MetaDataCondition.NOT_IN)
					userDefCond = new SRBUserDefinedCondition(srbFieldName.variableName,srbValueList, srbOperator);
				else	
					userDefCond = new SRBUserDefinedCondition(srbFieldName.variableName,srbValue, srbOperator);
				
				MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() - User Defined Condition-userDefVar:"+userDefCond.userDefVar,MatrixUtil.DEBUG_LOG);
				MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() - User Defined Condition-userDefValue:"+userDefCond.userDefValue,MatrixUtil.DEBUG_LOG);
				MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() - User Defined Condition-userDefOperator:"+userDefCond.userDefOperator,MatrixUtil.DEBUG_LOG);				
				
				srbUserDefinedConditionList.add(userDefCond);
				MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() - User Defined Condition:"+srbFieldName.variableName+conditionClause.getOperator()+srbValue,MatrixUtil.DEBUG_LOG);
			}			
			
		}
		
		// Copy contents from system and user defined conditions to final list
		// Determine Size of Array
		int finalConditionCount = 0;
		if(srbSystemDefinedConditionList!=null) // Append each condition one by one
			finalConditionCount +=srbSystemDefinedConditionList.size();
		if(srbUserDefinedConditionList!=null) // All conditions mapped to one table
			finalConditionCount +=1;
		srbConditions =  new MetaDataCondition[finalConditionCount];	
				
		// System Defined
		if(srbSystemDefinedConditionList!=null)
		{
			for(int index=0;index<srbSystemDefinedConditionList.size();index++)
			{
				tempCondition =(MetaDataCondition) srbSystemDefinedConditionList.get(index);
				srbConditions[index] = tempCondition;
			}
		}
		
		// User Defined
		if(srbUserDefinedConditionList!=null)
		{
			Object[][] definableMetaDataValues = new Object[srbUserDefinedConditionList.size()][2];
			MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() - User Defined Condition List Length:"+definableMetaDataValues.length,MatrixUtil.DEBUG_LOG);
			int[] operators = new int[definableMetaDataValues.length];
			for(int index=0;index<srbUserDefinedConditionList.size();index++)
			{
				// Condition
				SRBUserDefinedCondition userCond = (SRBUserDefinedCondition)srbUserDefinedConditionList.get(index);
				definableMetaDataValues[index][0] = userCond.userDefVar;
				definableMetaDataValues[index][1] = userCond.userDefValue;
				
				operators[index] = userCond.userDefOperator;
				
				MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() - User Defined Condition :("+index+")-userDefVar:"+userCond.userDefVar,MatrixUtil.DEBUG_LOG);
				MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() - User Defined Condition :("+index+"):-userDefValue:"+userCond.userDefValue,MatrixUtil.DEBUG_LOG);
				MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition() - User Defined Condition :("+index+"):-userDefOperator:"+userCond.userDefOperator,MatrixUtil.DEBUG_LOG);
				
			}
			MetaDataTable metaDataTable = new MetaDataTable( operators, definableMetaDataValues );
			tempCondition = MetaDataSet.newCondition(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES, metaDataTable );
			
			srbConditions[srbConditions.length-1] = tempCondition;
				
		}
		
		MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionToSRBCondition()-FINAL Condition List Length :"+srbConditions.length,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixConditionToSRBCondition():END",MatrixUtil.DEBUG_LOG);
		return srbConditions;
	}
	
	/*
	 * Maps the Matrix Request Condition Variable to JARGON/SRB Variable
	 * First parses the passed parameter to extract the actual variable
	 * and then finds the mapping
	 * 
	 * For Example, if the passed parameter was $coll/SystemMetadata/OFFSET
	 * parsed variables will be OFFSET
	 *  
	 * Final result value will be jargon/srb value - "offset"
	 */
	public SRBVariable mapMatrixConditionVariable(String variable)
	{
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixConditionVariable():BEGIN",MatrixUtil.DEBUG_LOG);
		SRBVariable srbConditionVar = null;
		String srbVariable = null;
		
		MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionVariable() - variable:"+variable,MatrixUtil.DEBUG_LOG);
		
		int indexOfLastFSlash = variable.lastIndexOf('/');
		srbVariable = variable.substring(indexOfLastFSlash+1,variable.length());
				
		MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionVariable() - srbVariable:"+srbVariable,MatrixUtil.DEBUG_LOG);
		
		srbConditionVar = mapMatrixVariableToSRBVariable(srbVariable);
		MatrixUtil.toLog("SRBQueryMapping - mapMatrixConditionVariable() - srbConditionVar:"+srbConditionVar.variableName,MatrixUtil.DEBUG_LOG);
		
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixConditionVariable():BEGIN",MatrixUtil.DEBUG_LOG);
		return srbConditionVar;
	}
	
	/*
	 * Maps the MAtrix Operator to JARGON/SRB Operator value
	 */
	public int mapMatrixOperator(String operator)
	{
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixOperator():BEGIN",MatrixUtil.DEBUG_LOG);
		int srbOperator =0 ;
		
		if (operator!=null) 
			operator = operator.trim();
		MatrixUtil.toLog("SRBQueryMapping - mapMatrixOperator() - operator:"+operator,MatrixUtil.DEBUG_LOG);	
		if(operator.equals("="))
			srbOperator =  MetaDataCondition.EQUAL;
		else if(operator.equals("!="))
			srbOperator = MetaDataCondition.NOT_EQUAL;	
		else if(operator.equals(">"))
			srbOperator = 	MetaDataCondition.GREATER_THAN ;	
		else if(operator.equals(">="))
			srbOperator = MetaDataCondition.GREATER_OR_EQUAL;		
		else if(operator.equals("<"))
			srbOperator = MetaDataCondition.LESS_THAN;
		else if(operator.equals("<="))
			srbOperator = 	MetaDataCondition.LESS_OR_EQUAL;
		
		// Added new supported operators on 19 August 2003 by Reena Mathew
		else if(operator.equals("LIKE"))
			srbOperator = 	MetaDataCondition.LIKE;	
		else if(operator.equals("NOT LIKE"))
			srbOperator = MetaDataCondition.NOT_LIKE;		
		else if(operator.equals("SOUNDS LIKE"))
			srbOperator = MetaDataCondition.SOUNDS_LIKE;
		else if(operator.equals("SOUNDS NOT LIKE"))
			srbOperator = 	MetaDataCondition.SOUNDS_NOT_LIKE;
		else if(operator.equals("IN"))
			srbOperator = MetaDataCondition.IN;
		else if(operator.equals("NOT IN"))
			srbOperator = 	MetaDataCondition.NOT_IN;
		else if(operator.equals("BETWEEN"))
			srbOperator = MetaDataCondition.BETWEEN;
		else if(operator.equals("NOT BETWEEN"))
			srbOperator = 	MetaDataCondition.NOT_BETWEEN;		
				
				 		
		
		MatrixUtil.toLog("SRBQueryMapping - mapMatrixOperator() - srbOperator:"+srbOperator,MatrixUtil.DEBUG_LOG);	
		
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixOperator():END",MatrixUtil.DEBUG_LOG);	
		return srbOperator;	
	}
	
	/*
	 * Extracts the string value for the condition check value
	 */
	public String mapMatrixConditionValue(String condValue)
	{
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixConditionValue():BEGIN",MatrixUtil.DEBUG_LOG);
		String srbCondValue = null;
		
		// What if int,float ??? - has to be resolved once the grammar can return String/Numeric Literal
		srbCondValue = condValue;
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixConditionValue():srbCondValue:"+srbCondValue,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixConditionValue():END",MatrixUtil.DEBUG_LOG);
		return srbCondValue;	
	}	
	
	/*
	 * Extract Matrix Query Selection fields from Matrix Query and
	 * map it to JARGON MetaDataSelect fields
	 * 
	 * For example 
	 * Return Expression =  return <Metadata><V1>{$coll/SystemMetaDataSet/OFFSET}</V1>
	 * 										 <V2>{$coll/UserDefined/color}</V2>
	 * 								</Metadata>
	 * 
	 * The return list will contain the JARGON equivalents for OFFSET and color
	 * depending on whether they are System Defined or User Defined Meta data
	 */
	public MetaDataSelect[] mapMatrixReturnClauseToSRBSelection(MatrixQueryReturnClause returnClause)
	{
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixReturnClauseToSRBSelection():BEGIN",MatrixUtil.DEBUG_LOG);
		
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixReturnClauseToSRBSelection():returnClause:"+returnClause.getReturnExpression(),MatrixUtil.DEBUG_LOG);
		
		queryReturnExpression = returnClause.getReturnExpression();
		
		MetaDataSelect selectArray[] =null;
		String[] selectFieldNames = null;
		ArrayList tempListOfSelectionForSystem = null; 
		ArrayList tempListOfSelectionForUserDefined = null;
		String fieldName = null;
		SRBVariable srbField = null;
		ArrayList fieldNamesForReturnClause = returnClause.getSelectionFieldNames();
		
		HashMap srbFieldNameToJargonFieldName = null;
		
		if(fieldNamesForReturnClause!=null)
		{
			
			for(int count=0;count<fieldNamesForReturnClause.size();count++)
			{
				fieldName = (String) fieldNamesForReturnClause.get(count);
				String matrixFieldName = new String(fieldName);
				int indexOfLastFSlash = fieldName.lastIndexOf('/');
				fieldName = fieldName.substring(indexOfLastFSlash+1,fieldName.length());
				
				
				
				
				// CHECK IF SELECTION IS FOR ALL AVU _PAIRS
				// A flag is set if one of the selection fields refers to User Defined Attributes
				// Once this flag is set, the results mapped for this query, will map the query results
				// specially for displaying all AVU-Pairs
				
				// Return Expression is also formatted for results mapping
				// For Example , if the reutrn expression templates was
				//	<MT><AttrName>{$coll/USER_DEFINED_ATTRIBUTE_NAMES}</AttrName></MT>
				// the formatted result template will now look like the following :
				// <MT><AttrName>{USER_DEFINED_ATTRIBUTE_NAMES}</AttrName></MT>
				
				 if(fieldName.equals("USER_DEFINED_ATTRIBUTES_NAMES") || fieldName.equals("USER_DEFINED_ATTRIBUTES_VALUES") || fieldName.equals("USER_DEFINED_ATTRIBUTES_UNITS"))
				 {
				 		 queryForAllAVUPairs = queryForAllAVUPairs || true;
				 		 
						 String resultTemplate = returnClause.getReturnExpression();
							MatrixUtil.toLog("SRBQueryMapping - mapMatrixReturnClauseToSRBSelection() - resultTemplate BEFORE:"+resultTemplate,MatrixUtil.DEBUG_LOG);
						 resultTemplate = replaceVariableWithValueInResultString(matrixFieldName,fieldName,resultTemplate);
						 returnClause.setReturnExpression(resultTemplate);
					MatrixUtil.toLog("SRBQueryMapping - mapMatrixReturnClauseToSRBSelection() - resultTemplate AFTER:"+resultTemplate,MatrixUtil.DEBUG_LOG);
				 		 
				 }		 
				
				 MatrixUtil.toLog("SRBQueryMapping - mapMatrixReturnClauseToSRBSelection() - ###### queryForAllAVUPairs :"+queryForAllAVUPairs +", fieldName:"+fieldName,MatrixUtil.DEBUG_LOG);							
				
				
				
				
				srbField = mapMatrixVariableToSRBVariable(fieldName);
									
				if(srbField.srbSystemVariable)
				{
					if(tempListOfSelectionForSystem==null)
						tempListOfSelectionForSystem = new ArrayList();
					tempListOfSelectionForSystem.add(srbField);					
				}
				else
				{
					if(tempListOfSelectionForUserDefined==null)
						tempListOfSelectionForUserDefined = new ArrayList();
					tempListOfSelectionForUserDefined.add(srbField);				
				}
				
				if(srbFieldNameToJargonFieldName==null)
					srbFieldNameToJargonFieldName = new HashMap();
				
				MatrixUtil.toLog("SRBQueryMapping - mapMatrixReturnClauseToSRBSelection() - srbField:"+srbField.variableName+", matrixFieldName:"+matrixFieldName,MatrixUtil.DEBUG_LOG);	
				srbFieldNameToJargonFieldName.put(srbField.variableName,matrixFieldName);	
								 	
			}
		}
		
		
		// Determine Size of Selection Field
		// Copy contents from system and user defined conditions to final list
		// Determine Size of Array
		int finalSelectionCount = 0;
		if(tempListOfSelectionForSystem!=null) // Append each condition one by one
			finalSelectionCount +=tempListOfSelectionForSystem.size();
		 if(tempListOfSelectionForUserDefined!=null) // All conditions mapped to one table
			finalSelectionCount +=1;
		selectFieldNames =  new String[finalSelectionCount];	
			
			
		// Copy System Fields
		if(tempListOfSelectionForSystem!=null)
		{
			for(int count =0 ;count<tempListOfSelectionForSystem.size();count++)
			{
				srbField = (SRBVariable) tempListOfSelectionForSystem.get(count);
				selectFieldNames[count] = new String(srbField.variableName);
				
				
			}		
		}
		
		// Set field for MetaData Table
		if(tempListOfSelectionForUserDefined!=null)
		{
			selectFieldNames[selectFieldNames.length-1] = new String(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES);
		}
		
							
		MatrixUtil.toLog("SRBQueryMapping - mapMatrixReturnClauseToSRBSelection - selection field count :"+selectFieldNames.length,MatrixUtil.DEBUG_LOG);			
		selectArray = MetaDataSet.newSelection( selectFieldNames );
		
		// To store the mappings of fieldNames
		srbFieldToMatrixFieldMapping = srbFieldNameToJargonFieldName;
		
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixReturnClauseToSRBSelection():END",MatrixUtil.DEBUG_LOG);
		return selectArray;
	}
	
	
	/*
	 * Determines whether a Matrix Variable is System or User Defined 
	 * and returns the appropriate mappings
	 * 
	 * For example,
	 * 	If System Defined Variable OFFSET is passed , the method will return mapping "offset"
	 * 	If User Defined Variable colorOfBackground is passed, the method will return the same value colorOfBackground
	 */
	public SRBVariable mapMatrixVariableToSRBVariable(String variable)
	{
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixVariableToSRBVariable():BEGIN",MatrixUtil.DEBUG_LOG);
		String srbVariableName = null;
		boolean variableMapped = false;
		MatrixUtil.toLog("SRBQueryMapping - mapMatrixVariableToSRBVariable() - variable:"+variable,MatrixUtil.DEBUG_LOG);
	
		for(int variableIndex=0;variableIndex<srbVariableMapping.length;variableIndex++)
		{
			if(srbVariableMapping[variableIndex][0].equals(variable))
			{
				srbVariableName = 	srbVariableMapping[variableIndex][1];
				variableMapped = true;
				break;	
			}
		}
			
		
		if(!variableMapped)
		{
			// Should we assume it is User Defined Meta Data
			
			// If User Defined Meta Data - should build MetaDataTable
			srbVariableName = variable;
		}
		SRBVariable srbVariable = new SRBVariable(srbVariableName,variableMapped);
	
		MatrixUtil.toLog("SRBQueryMapping - mapMatrixVariableToSRBVariable() - srbVariableName:"+srbVariableName,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("SRBQueryMapping-mapMatrixVariableToSRBVariable():END",MatrixUtil.DEBUG_LOG);
		return 	srbVariable;
	}	
	
	/*
	 * Maps the JARGON/SRB Query Results to Matrix Query Result Format
	 */
	public String mapSRBQueryResultToMatrixQueryResult(MetaDataRecordList[] rl ,MatrixQueryReturnClause returnClause)
	{
		MatrixUtil.toLog("SRBQueryMapping-mapSRBQueryResultToMatrixQueryResult():BEGIN",MatrixUtil.DEBUG_LOG);
		printQueryResults(rl);
		String resultString = new String("");
		String resultTemplate = returnClause.getReturnExpression();
		
		MatrixUtil.toLog("SRBQueryMapping-mapSRBQueryResultToMatrixQueryResult():resultTemplate:"+resultTemplate,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("SRBQueryMapping-mapSRBQueryResultToMatrixQueryResult():queryReturnExpression:"+queryReturnExpression,MatrixUtil.DEBUG_LOG);
		
		 try
		 {	
		 	
		 	// Test Code to see the values in SRB Field to Matrix Field Hashmap
			Set keySet = srbFieldToMatrixFieldMapping.keySet();
			
			if(keySet!=null)
			{
				Iterator keyIterator = keySet.iterator();
				if(keyIterator!=null)
				{
				 	while(keyIterator.hasNext())
					{
						String keyStr =(String) keyIterator.next();
						String value = (String) srbFieldToMatrixFieldMapping.get(keyStr);						
						MatrixUtil.toLog("SRBQueryMapping - mapSRBQueryResultToMatrixQueryResult()-Key :"+keyStr+", Value :"+value,MatrixUtil.DEBUG_LOG);		
					}
				}
				else
					MatrixUtil.toLog("SRBQueryMapping - mapSRBQueryResultToMatrixQueryResult()-keyIterator is NULL",MatrixUtil.DEBUG_LOG);	
			}
			else
			{
				MatrixUtil.toLog("SRBQueryMapping - mapSRBQueryResultToMatrixQueryResult()-keySet is NULL",MatrixUtil.DEBUG_LOG);
			}
			
					
			 if (rl != null) 
			  { 
			  	  // Iterate through result list and map each record to matrix result template	
				  for (int i=0;i<rl.length;i++)
				  {
				  	if(!queryForAllAVUPairs)
						resultString += mapResultForSingleResult(resultTemplate,rl[i]);
					else 
						resultString += mapResultForAllAVUPairs(resultTemplate,rl[i]);	
				  	  
				  }
			  }
			  else // No Results 
			  {
			  		resultString = "No Result matching the query";
			  }
			
			MatrixUtil.toLog("SRBQueryMapping - mapSRBQueryResultToMatrixQueryResult()-resultString:"+resultString,MatrixUtil.DEBUG_LOG);
			MatrixUtil.toLog("SRBQueryMapping - mapSRBQueryResultToMatrixQueryResult()-END",MatrixUtil.DEBUG_LOG);
						  
			 // return (resultString);
		 }
		 catch(Exception e)
		 {
			MatrixUtil.toLog("SRBQueryMapping - mapSRBQueryResultToMatrixQueryResult()-EXCEPTION:"+e,MatrixUtil.DEBUG_LOG);			
		 }
		return (resultString);
	}
	
	private String mapResultForAllAVUPairs(String resultStringTemplate,MetaDataRecordList resultRecord)
	{
		MatrixUtil.toLog("SRBQueryMapping-mapResultForAllAVUPairs():BEGIN",MatrixUtil.DEBUG_LOG);
		String resultStringForRecord = "";//resultStringTemplate;
		HashMap fieldMapping = srbFieldToMatrixFieldMapping;
		String userDefinedMetaDataValues [][]= null; 
		String template = null; //new String(resultStringTemplate);
		
		Object attrObject = null;
		Object valueObject = null;
		Object unitObject = null;
		
		String attrString =null;
		String valueString = null; 
		String unitString = null;
		
		
		for (int j=0;j<resultRecord.getRecordCount();j++)
		{
					// Extract user defined value
					// Map to temp table
					// Use table for final result creation	
				MatrixUtil.toLog("SRBQueryMapping-mapResultForAllAVUPairs():RECORD:"+j,MatrixUtil.DEBUG_LOG);		 	
		    	 if (resultRecord.getFieldType(j) == MetaDataField.TABLE) 
				 {
				      MatrixUtil.toLog("SRBQueryMapping-mapResultForAllAVUPairs():MetaDataField.TABLE",MatrixUtil.DEBUG_LOG);	
					  MetaDataTable table = resultRecord.getTableValue(j);
			
					  if(table!=null)
					  {
						if(table.getRowCount()>0)
							userDefinedMetaDataValues = new String[table.getRowCount()][table.getColumnCount()];	
					  }
						  
					  for (int k=0;k<table.getRowCount();k++) 
					  {
						  for (int l=0;l<table.getColumnCount();l++) 
						  {
							MatrixUtil.toLog("SRBQueryMapping-mapResultForAllAVUPairs():Table Value("+k+","+l+"):"+table.getStringValue( k, l )+"\t",MatrixUtil.DEBUG_LOG);
					  
							  //if((l==0)||(l==1))
							  {
									userDefinedMetaDataValues [k][l] = table.getStringValue( k, l );
							  }	
						  }
					  }
			  
					  // Go through list
					  if(userDefinedMetaDataValues!=null)
					  {
							for(int index=0;index<userDefinedMetaDataValues.length;index++)
							{
								Object matrixFieldObject =  srbFieldToMatrixFieldMapping.get(userDefinedMetaDataValues[index][0]);
								
								attrObject = userDefinedMetaDataValues[index][0];
								valueObject = userDefinedMetaDataValues[index][1];
								unitObject = userDefinedMetaDataValues[index][2];
								
								if(attrObject!=null)
									attrString = (String)attrObject;
								else
									attrString = "";	
								
								if(valueObject!=null)
									valueString = (String)valueObject;
								else
									valueString = "";		
									
								if(unitObject!=null)
									unitString = (String)unitObject;
								else
									unitString = "";				
								
								if(attrObject!=null || valueObject!=null || unitObject!=null)
								{
									template = new String(resultStringTemplate);	
									template = replaceVariableWithValueInResultString("{USER_DEFINED_ATTRIBUTES_NAMES}",attrString,template);
									template = replaceVariableWithValueInResultString("{USER_DEFINED_ATTRIBUTES_VALUES}",valueString,template);
									template = replaceVariableWithValueInResultString("{USER_DEFINED_ATTRIBUTES_UNITS}",unitString,template);				  	   
									resultStringForRecord+=	template;
								}	
							}
					  }
			  
			  
				  }
			 }
		
			//	Replace non-filled fields with empty space  
			//resultStringForRecord = processResultStringToRemoveUnfoundResults(resultStringForRecord);  	
			MatrixUtil.toLog("SRBQueryMapping - mapResultForAllAVUPairs()-resultStringForRecord:"+resultStringForRecord,MatrixUtil.DEBUG_LOG);			 	  
		
		
		
		
		MatrixUtil.toLog("SRBQueryMapping-mapResultForAllAVUPairs():END",MatrixUtil.DEBUG_LOG);
		return(resultStringForRecord);
	}
	
	/*
	 * Map a MetaDataRecord(JARGON/SRB) to Matrix Result format
	 */
	public String mapResultForSingleResult(String resultStringTemplate,MetaDataRecordList resultRecord)
	{
		MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult():BEGIN",MatrixUtil.DEBUG_LOG);
		
		String resultStringForRecord = resultStringTemplate;
		HashMap fieldMapping = srbFieldToMatrixFieldMapping;
		String userDefinedMetaDataValues [][]= null; 
		
		MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult():resultStringForRecord:"+resultStringForRecord,MatrixUtil.DEBUG_LOG);
		
		for (int j=0;j<resultRecord.getRecordCount();j++)
		 {
				// Extract user defined value
				// Map to temp table
				// Use table for final result creation	
			MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult():RECORD:"+j,MatrixUtil.DEBUG_LOG);		 	
		 	  if (resultRecord.getFieldType(j) == MetaDataField.TABLE) 
			  {
				MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult():MetaDataField.TABLE",MatrixUtil.DEBUG_LOG);	
				  MetaDataTable table = resultRecord.getTableValue(j);
				
				  if(table!=null)
				  {
				  	if(table.getRowCount()>0)
						userDefinedMetaDataValues = new String[table.getRowCount()][2];	
				  }
							  
				  for (int k=0;k<table.getRowCount();k++) 
				  {
			  		  for (int l=0;l<table.getColumnCount();l++) 
					  {
						MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult():Table Value("+k+","+l+"):"+table.getStringValue( k, l )+"\t",MatrixUtil.DEBUG_LOG);
						  
						  if((l==0)||(l==1))
						  {
								userDefinedMetaDataValues [k][l] = table.getStringValue( k, l );
						  }	
					  }
				  }
				  
				  // Go through list
				  if(userDefinedMetaDataValues!=null)
				  {
				  		for(int index=0;index<userDefinedMetaDataValues.length;index++)
				  		{
							Object matrixFieldObject =  srbFieldToMatrixFieldMapping.get(userDefinedMetaDataValues[index][0]);
							if(matrixFieldObject!=null)
							{
							   String matrixFieldName = (String) matrixFieldObject;
							   String matrixFieldValue = userDefinedMetaDataValues[index][1];
							   
							   //Since the resultTemplate has the variables embedded in Curly Braces {}
							   matrixFieldName = "{" + matrixFieldName + "}";
							   
							   resultStringForRecord = replaceVariableWithValueInResultString(matrixFieldName,matrixFieldValue,resultStringForRecord);				  	   
							}	
				  		}
				  }
				  
				  
			  }
			  else // System Level Data
			  {
				MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult():SYSTEM LEVEL METADATA",MatrixUtil.DEBUG_LOG);	
					// Field Name fro SRB
					String srbFieldName = resultRecord.getFieldName(j);
					MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult():srbFieldName:"+srbFieldName,MatrixUtil.DEBUG_LOG);	
					
					
					//	Mapping to Variable in Result Template
					Object matrixFieldObject =  srbFieldToMatrixFieldMapping.get(srbFieldName);
					if(matrixFieldObject!=null)
					{
					   MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult():SRB FIELD FOUND IN FIELD MAP",MatrixUtil.DEBUG_LOG);	
					   String matrixFieldName = (String) matrixFieldObject;
					   String matrixFieldValue = resultRecord.getStringValue(j);
					   
					   // Since the resultTemplate has the variables embedded in Curly Braces {}
					   matrixFieldName = "{" + matrixFieldName + "}";
					   
					   resultStringForRecord = replaceVariableWithValueInResultString(matrixFieldName,matrixFieldValue,resultStringForRecord);				  	   
					}
					else
					{
						MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult():SRB FIELD NOT FOUND IN FIELD MAP",MatrixUtil.DEBUG_LOG);		
					}
			  }
		  }
		 
		// Replace non-filled fields with empty space  
		resultStringForRecord = processResultStringToRemoveUnfoundResults(resultStringForRecord);  	
		MatrixUtil.toLog("SRBQueryMapping - mapResultForSingleResult()-resultStringForRecord:"+resultStringForRecord,MatrixUtil.DEBUG_LOG);	  
		
		MatrixUtil.toLog("SRBQueryMapping-mapResultForSingleResult()-END",MatrixUtil.DEBUG_LOG);	 	
		return(resultStringForRecord);
	}
	
	
	/*
	 * This method is to replace unfound variables with empty string
	 * 
	 * For example 
	 * 	if the return expression after mapping JARGON/SRB Results    
	 * 								<Metadata>
	 * 										<Filename>queryTestFile</Filename>
	 * 										<Color>{$coll/UserDefined/COLOR}</Color>
	 * 								 </Metadata> 
	 * then the value for Color should be empty string and not the selection field
	 * 
	 * This method will return the following string:
	 * 			<Metadata>
	 * 					<Filename>queryTestFile</Filename>
	 * 					<Color></Color>
	 * 			 </Metadata> 
	 */
	public String processResultStringToRemoveUnfoundResults(String resultString)
	{
		MatrixUtil.toLog("SRBQueryMapping - processResultStringToRemoveUnfoundResults()-BEGIN",MatrixUtil.DEBUG_LOG);	
		String formattedResultString = resultString;
		Set keySet = srbFieldToMatrixFieldMapping.keySet();
		if(keySet!=null)
		{
			Iterator keyIterator = keySet.iterator();
			if(keyIterator!=null)
			{
				while(keyIterator.hasNext())
				{
					String keyStr =(String) keyIterator.next();
					String value = (String) srbFieldToMatrixFieldMapping.get(keyStr);						
					MatrixUtil.toLog("SRBQueryMapping - processResultStringToRemoveUnfoundResults()-Key :"+keyStr+", Value :"+value,MatrixUtil.DEBUG_LOG);
					
					//Since the resultTemplate has the variables embedded in Curly Braces {}
					value = "{" + value + "}";
					
					
					formattedResultString = replaceVariableWithValueInResultString(value,"",formattedResultString);		
				}
			}
			else
				MatrixUtil.toLog("SRBQueryMapping - processResultStringToRemoveUnfoundResults()-keyIterator is NULL",MatrixUtil.DEBUG_LOG);		
		
		}
		MatrixUtil.toLog("SRBQueryMapping - processResultStringToRemoveUnfoundResults()-formattedResultString:"+formattedResultString,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("SRBQueryMapping - processResultStringToRemoveUnfoundResults()-END",MatrixUtil.DEBUG_LOG);
		return(formattedResultString);	
	}
	
	
	/*
	 * This would replace the resultVariable in the resultTemplate with the resultValue
	 * Ex resultVaraible = $coll/SystemMetadata/SIZE, resultValue = 1000 and
	 * resultTemplate = <Metadata><Size>$coll/SystemMetadata/SIZE</Size></Metadata>
	 * 
	 * Return String = <Metadata><Size>1000</Size></Metadata>
	 */
	public String replaceVariableWithValueInResultString(String resultVariable,String resultValue,String resultTemplate)
	{
		MatrixUtil.toLog("SRBQueryMapping-replaceVariableWithValueInResultString():BEGIN",MatrixUtil.DEBUG_LOG);
		
		MatrixUtil.toLog("SRBQueryMapping - replaceVariableWithValueInResultString()-resultVariable:"+resultVariable,MatrixUtil.DEBUG_LOG);			  
		MatrixUtil.toLog("SRBQueryMapping - replaceVariableWithValueInResultString()-resultValue:"+resultValue,MatrixUtil.DEBUG_LOG);			  
		MatrixUtil.toLog("SRBQueryMapping - replaceVariableWithValueInResultString()-resultTemplate:"+resultTemplate,MatrixUtil.DEBUG_LOG);
					  		  
		String resultString = resultTemplate;
		if (resultVariable == null || resultValue == null || resultTemplate == null) 
		{		
			resultString =  resultString;	
		}
		else
		{	
			int tokenPos = resultTemplate.indexOf(resultVariable);	
			if (tokenPos == -1) 
			{		
				resultString =  resultString;	
			}
			else
			{	
				String start = resultTemplate.substring(0,tokenPos);    
				String end = resultTemplate.substring(tokenPos + resultVariable.length());
				resultString = start + resultValue + end;
			}	
		}
		
		MatrixUtil.toLog("SRBQueryMapping - replaceVariableWithValueInResultString()-resultString:"+resultString,MatrixUtil.DEBUG_LOG);	
		MatrixUtil.toLog("SRBQueryMapping - replaceVariableWithValueInResultString()-END",MatrixUtil.DEBUG_LOG);
			  	        
		return resultString;
	}
	
	// Class for helping in determing whether a variable is System or User
	class SRBVariable
	{
		public SRBVariable()
		{
		}
		
		public SRBVariable(String varName,boolean sysVar)
		{
			variableName = varName;
			srbSystemVariable = sysVar;
		}
		public String variableName;
		public boolean srbSystemVariable;		
	}
	
	// Class for capturing user data to create MetaDataTable
	class SRBUserDefinedCondition
	{
		public SRBUserDefinedCondition()
		{
			
		}
		
		public SRBUserDefinedCondition(Object var,Object value, int opr)
		{
			userDefVar = var;
			userDefValue=value;
			userDefOperator=opr;			
		}
		
		public Object userDefVar;
		public Object userDefValue;
		public int userDefOperator;
	}
	
	/*
	 * Test Code to print query results
	 * Taken from Jargon MetaDataTest code
	 */
	public  void printQueryResults( MetaDataRecordList[] rl )
		{
			//print query results
			MatrixUtil.toLog("SRBQueryMapping - printQueryResults()-BEGIN",MatrixUtil.DEBUG_LOG);	
			if (rl != null) { //Nothing in the database matched the query
				MatrixUtil.toLog("\n",MatrixUtil.DEBUG_LOG);
				for (int i=0;i<rl.length;i++) {
					MatrixUtil.toLog("\n",MatrixUtil.DEBUG_LOG);
					for (int j=0;j<rl[i].getRecordCount();j++) {
						MatrixUtil.toLog(rl[i].getFieldName(j)+"\t",MatrixUtil.DEBUG_LOG);
					}
					System.out.print("\n");
					for (int j=0;j<rl[i].getRecordCount();j++) {
						if (rl[i].getFieldType(j) == MetaDataField.TABLE) {
							MetaDataTable table = rl[i].getTableValue(j);
							MatrixUtil.toLog("\ntable: ",MatrixUtil.DEBUG_LOG);
							for (int k=0;k<table.getRowCount();k++) {
								MatrixUtil.toLog("\n",MatrixUtil.DEBUG_LOG);
								for (int l=0;l<table.getColumnCount();l++) {
									MatrixUtil.toLog(table.getStringValue( k, l )+"\t",MatrixUtil.DEBUG_LOG);
								}
							}
						}
						else {
							MatrixUtil.toLog(rl[i].getStringValue(j)+"\t",MatrixUtil.DEBUG_LOG);
						}
					}
				}
			}
			else
			{
				MatrixUtil.toLog("SRBQueryMapping - printQueryResults()-NO RESULTS",MatrixUtil.DEBUG_LOG);
			}
			MatrixUtil.toLog("\n",MatrixUtil.DEBUG_LOG);
			MatrixUtil.toLog("SRBQueryMapping - printQueryResults()-END",MatrixUtil.DEBUG_LOG);	

		}
}