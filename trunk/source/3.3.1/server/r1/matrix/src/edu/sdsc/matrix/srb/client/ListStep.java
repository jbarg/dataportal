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


/**
	* Creates a step to list the contents (data sets) of a collection or a container.
	*
	*
	* <p>Title: ListStep.java </p>
	* <p>Description: </p>
	* @author Arun Jagatheesan
	* @version 1.0
	*/

	/*
		* original author : Jon Weinberg
		* Added Recursiveness, info level, java docs, licence 1/29/05 Arun
		*/

public class ListStep extends Step{

			public ListStep(String id, Collection collection){
						super(id, new Operation(new ListOp(collection)));
			}

			/**
				* Creates a Step to list a colleciton recursively
				* <p> Currently we support only recursiveness -1 used with -1 extended info level.
				* This (collName, -1) gives list of all datasets in a collection (recusively). It only
				* provides the datasets name (without any additional info). Note that a call to this
				* method currently adds -1 for info level to be used.
				* </p>
				*
				* @param id Identifier for the step
				* @param collection The collection to list
				* @param recursiveLevel Recursive Level to list ( -1 = fully recursive)
				*/
			public ListStep(String id, Collection collection, int recursiveLevel){
						this(id,collection, recursiveLevel, -1);
			}



			/**
				* Creates a Step to list a colleciton
				* <p> Currently we support only recursiveness -1 used with -1 extended info level.
				* This (collName, -1, -1) gives list of all datasets in a collection (recusively). It only
				* provides the datasets name (without any additional info).
				* </p>
				*
				* @param id Identifier for the step
				* @param collection The collection to list
				* @param recursiveLevel Recursive Level to list ( -1 = fully recursive)
				* @param infoLevel Level of info needed in listing ( -1 = all data sets inside)
				*/
			public ListStep(String id, Collection collection, int recursiveLevel, int infoLevel){
						super(id, new Operation(new ListOp(collection, recursiveLevel, infoLevel)));
			}

			public ListStep(String id, Container container){
						super(id, new Operation(new ListOp(container)));
			}

			public ListStep(String id, ParamList flexParams){
						super(id, new Operation(new ListOp(flexParams)));
			}

			public ListStep(String id, ListParams stdParams){
						super(id, new Operation(new ListOp(stdParams)));
			}

}