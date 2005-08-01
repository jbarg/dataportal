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
	* Resource.java
	*
	* Created on July 8, 2003, 4:32 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Contains methods to create Resource objects.
	* @author  Allen Ding - SDSC
	*/
public class Resource extends edu.sdsc.matrix.srb.parser.impl.ResourceImpl {
	/**
		* Creates a Resouce object with the <b>required</b> logicalName.
		* @param logicalName The name of the resource.
		*/
	public Resource(String logicalName) {
		super.setLogicalName(logicalName);
	}

	/**
		* Internal method used to wrap a parser interface Resource object into this
		* class' representation. This method should not normally be used by the user.
		* @param resource The parser.Resource to wrap.
		*/
	protected Resource(edu.sdsc.matrix.srb.parser.Resource resource) {
		super.setLogicalName(resource.getLogicalName());
		if (resource.getResourceType() != null) {
			super.setResourceType(new AnyType(resource.getResourceType()));
		}
		super.setPhysicalLocation(resource.getPhysicalLocation());
		super.setNetURI(resource.getNetURI());
		super.setMaxObjSize(resource.getMaxObjSize());
		super.setLatency(resource.getLatency());
		super.setApproxBandwidth(resource.getApproxBandwidth());
		super.setResoureHierarchy(resource.getResoureHierarchy());
		super.setCapacity(resource.getCapacity());
	}
}
