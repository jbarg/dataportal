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

package edu.sdsc.matrix.srb.client;

import edu.sdsc.matrix.srb.parser.CollectionList;

/**
	* <p>Contains methods to create TPart. TPart is used to create variables as name-value pairs.
	* Variables can be associated with any Matrix Thread (Transaction, Flow or Step).
	* <i>(e.g):</i> A flow can have a variable name <i>color</i> which could be mapped to an
	* object of type string with value "green".</p>
	*
	* <p>Theoretically <i>Data Grid Language (DGL) </i> is supposed to have any type of object as
	* metadata variable in the name-value pair combination. But, due to implementation constraints,
	* from the java technologies we are using now, we have currently restricted the types to String,
	* Number (java.math.Big Decimal or xs:integer) and dgl:ListResult - we will have this expanded to
	* any object soon.
	*
	*  @author  Arun Jagatheesan
	*/
public class TPart extends edu.sdsc.matrix.srb.parser.impl.TPartImpl {
	protected TPart(edu.sdsc.matrix.srb.parser.TPart tp) {
		super.setName(tp.getName());
		super.setNumber(tp.getNumber());
		super.setCollectionList(tp.getCollectionList());
		super.setString(tp.getString());
										super.setStringSet(tp.getStringSet());
										super.setCollection(tp.getCollection());
										super.setDataSet(tp.getDataSet());
										super.setStreamData(tp.getStreamData());
	}

	public TPart() { super(); }

	public TPart (String name, String value) {
		super.setName(name);
		super.setString(value);
	}

	public TPart (String name, java.math.BigDecimal number) {
		super.setName(name);
		super.setNumber(number);
	}

	public TPart (String name, CollectionList collectionList) {
		super.setName(name);
		super.setCollectionList(collectionList);
	}

					public TPart (String name, Collection collection) {
							super.setName(name);
							super.setCollection(collection);
					}

					public TPart (String name, StringSet set) {
							super.setName(name);
							super.setStringSet(set);
					}

					public TPart (String name, DataSet dataSet) {
							super.setName(name);
							super.setDataSet(dataSet);
					}

					public TPart (String name, StreamData streamData) {
							super.setName(name);
							super.setStreamData(streamData);
					}

}
