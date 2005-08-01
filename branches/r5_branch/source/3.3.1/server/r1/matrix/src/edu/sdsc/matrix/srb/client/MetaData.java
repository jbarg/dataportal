/*
	* 	SDSC Matrix. A Gridflow Management System for data grids and digtal libraries
	*  Copyright (C) 2004 SDSC Matrix Project
	*
	* This library is free software; you can redistribute it and/or
	* modify it under the terms of the GNU Lesser General Public
	* License as published by the Free Software Foundation; either
	* version 2.1 of the License, or (at your option) any later version.
	*
	* This library is distributed in the hope that it will be useful,
	*	but WITHOUT ANY WARRANTY; without even the implied warranty of
	*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	*	Lesser General Public License for more details.
	*
	*	You should have received a copy of the GNU Lesser General Public
	*	License along with this library; if not, write to the Free Software
	*	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	*
	* ====================================================================
	*
	* This software consists of voluntary contributions from the developers of the SDSC
	* Matrix Project. We request that all redistribution and software made using this code
	* acknowledge their use of the “SDSC Matrix project” in their end-user documentation or
	* in their website.
	*
	* SDSC Matrix Project (list of developers as of Dec 2005)
	*	Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
	* 9500 Gilman Drive, MC0505
	* San Diego Supercomputer Center
	* University of California
	* La Jolla CA 92093
	*
	* Allen Ding (alding@sdsc.edu)
	* Lucas Gilbert (iktome@sdsc.edu)
	* Arun Jagatheesan (arun@sdsc.edu)
	* Reena Mathew	(rmathew@sdsc.edu)
	* Daniel Moore (mixx@umail.ucsb.edu)
	* Erik Vandekieft (evk@sdsc.edu)
	* Jonathan Weinberg (jonw@sdsc.edu)
	*
	*/

/*
	* MetaData.java
	*
	* Created on July 7, 2003, 3:19 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.util.*;

/**
	* Part of the Matrix client API.
	* Contains methods to create MetaData objects.
	* MetaData objects contain descriptive information  about other objects
	* in the DGL.
	*
	* Non optional-parameter changes to the object should be made using
	* useXXX methods that are available. A bad configuration of set fields
	* may result in an invalid content tree.
	* @author  Allen Ding - SDSC
	*/
public class MetaData extends edu.sdsc.matrix.srb.parser.impl.MetaDataImpl {
	/**
		* Internal method used to wrap a parser interface MetaData object into this
		* class' representation. This method should not normally be used by the user.
		* @param metadata The parser.MetaData to wrap.
		*/
	protected MetaData(edu.sdsc.matrix.srb.parser.MetaData metadata) {
		for (Iterator i = metadata.getAVUPairs().iterator(); i.hasNext(); ) {
			edu.sdsc.matrix.srb.parser.AVUpair tempAVUpair =
						(edu.sdsc.matrix.srb.parser.AVUpair) i.next();
			AVUpair avupair = new AVUpair(tempAVUpair);
			super.getAVUPairs().add(avupair);
		}
		if (metadata.getUserDefined() != null) {
			super.setUserDefined(new UserDefinedSchema(metadata.getUserDefined()));
		}
		if (metadata.getSystemMeta() != null) {
			super.setSystemMeta(new SystemMetaData(metadata.getSystemMeta()));
		}
	}

	/**
		* Default do nothing constructor for a MetaData Object.
		*/
	public MetaData() {
		super();
	}

	/**
		* Constructor that takes an initial attribute and value for the
		* Metadata object.
		* @param attribute Initial attribute.
		* @param value Initial value of the attribute.
		*/
	public MetaData(String attribute, String value) {
		addAVUPair(attribute, value);
	}

	/**
		* Adds an attribute value pair to the MetaData object.
		* @param attribute The attribute to add.
		* @param value The value of the attribute being added.
		*/
	public void addAVUPair(String attribute, String value) {
		super.getAVUPairs().add(new AVUpair(attribute, value));
	}

	/**
		* Constructs a MetaData object with an initial AVUpair object.
		* A MetaData object can hold an unlimited sequence of AVU pair objects.
		* @param avuPair The initial AVUpair element.
		*/
	public MetaData(AVUpair avuPair) {
		super.getAVUPairs().add(avuPair);
	}

	/**
		* Constructs a MetaData object with an AnyType parameter.
		* @param userDefined Any AnyType object?
		*/
	public MetaData(AnyType userDefined) {
	//super.setUserDefined(userDefined);
	}

	/**
		* Clears the objects AVUpair list and restarts with a new
		* one if for some reason you need it.
		* Using this method clobbers any previously set UserDefined
		* parameters.
		* @param avuPair The AVUpair to restart with.
		*/
	public void useAVUPair(AVUpair avuPair) {
		super.setUserDefined(null);
		super.getAVUPairs().clear();
		super.getAVUPairs().add(avuPair);
	}

	/**
		* Adds an AVUPair to an existing MetaData object.
		* Using this method clobbers any previously set UserDefined
		* parameters.
		* @param avuPair The AVUpair to add.
		*/
	public void addAVUPair(AVUpair avuPair) {
		super.setUserDefined(null);
		super.getAVUPairs().add(avuPair);
	}

	/**
		* Makes the MetaData object use an specified userDefined information.
		* Using this method clears the objects AVUPair list.
		* @param userDefined A user defined anyType.
		*/
	public void useUserDefined(AnyType userDefined) {
		super.getAVUPairs().clear();
		//super.setUserDefined(userDefined);
	}
}
