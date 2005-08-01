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
	* StdDatagridObject.java
	*
	* Created on July 7, 2003, 5:03 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Part of the Matrix client API.
	* Contains methods to create StdDataGrid objects.
	*
	* @author  Allen Ding - SDSC
	*/
public class StdDatagridObject extends edu.sdsc.matrix.srb.parser.impl.
			StdDatagridObjectImpl {

	public StdDatagridObject(edu.sdsc.matrix.srb.parser.StdDatagridObject obj)
	{
			if(obj.getActiveDataSetNow() != null)
								this.setActiveDataSetNow(new ActiveDatagridDataSet(obj.getActiveDataSetNow()));
			if(obj.getADCnow() != null)
								this.setADCnow(new ActiveDatagridCollection(obj.getADCnow()));
			if(obj.getCollectionNow() != null)
								this.setCollectionNow(new Collection(obj.getCollectionNow()));
			if(obj.getContainerNow() != null)
								this.setContainerNow(new Container(obj.getContainerNow()));
			if(obj.getDataSetNow() != null)
								this.setDataSetNow(new DataSet(obj.getDataSetNow()));
	}

	/**
		* Creates a StdDatagridObject using the specified Collection
		* object.
		* @param collectionNow The Collection object to use.
		*/
	public StdDatagridObject(Collection collectionNow) {
		super.setCollectionNow(collectionNow);
	}

	/**
		* Creates a StdDatagridObject using the specified DataSet object.
		* @param datasetNow The DataSet to use.
		*/
	public StdDatagridObject(DataSet datasetNow) {
		super.setDataSetNow(datasetNow);
	}

	/**
		* Creates a StdDatagridObject using the specified Container object.
		* @param containerNow The container to use.
		*/
	public StdDatagridObject(Container containerNow) {
		super.setContainerNow(containerNow);
	}

	/**
		* Creates a StdDatagridObject using the specified
		* ActiveDatagridCollection object.
		* @param adcNow The ActiveDatagridCollection to use.
		*/
	public StdDatagridObject(ActiveDatagridCollection adcNow) {
		super.setADCnow(adcNow);
	}

	/**
		* Creates a StdDatagridObject using the specified
		* ActiveDatagridDataSet object.
		* @param activeDataSetNow The ActiveDatagridDataSet to use.
		*/
	public StdDatagridObject(ActiveDatagridDataSet activeDataSetNow) {
		super.setActiveDataSetNow(activeDataSetNow);
	}

	/**
		* Makes the StdDatagridObject use the specified Collection.
		* This method clobbers any DataSet, Container, ActiveDatagridCollection,
		* and ActiveDatagridDataSet parameters.
		*/
	public void useCollection(Collection collectionNow) {
		super.setDataSetNow(null);
		super.setContainerNow(null);
		super.setADCnow(null);
		super.setActiveDataSetNow(null);
		super.setCollectionNow(collectionNow);
	}

	/**
		* Makes the StdDatagridObject use the specified DataSet.
		* This method clobbers any Collection, Container, ActiveDatagridCollection,
		* and ActiveDatagridDataSet parameters.
		*/
	public void useDataSet(DataSet dataSetNow) {
		super.setContainerNow(null);
		super.setADCnow(null);
		super.setActiveDataSetNow(null);
		super.setCollectionNow(null);
		super.setDataSetNow(dataSetNow);
	}

	/**
		* Makes the StdDatagridObject use the specified Container.
		* This method clobbers any DataSet, Collection, ActiveDatagridCollection,
		* and ActiveDatagridDataSet parameters.
		*/
	public void useContainer(Container containerNow) {
		super.setDataSetNow(null);
		super.setADCnow(null);
		super.setActiveDataSetNow(null);
		super.setCollectionNow(null);
		super.setContainerNow(containerNow);
	}

	/**
		* Makes the StdDatagridObject use the specified ActiveDatagridCollection.
		* This method clobbers any DataSet, Container, Collection,
		* and ActiveDatagridDataSet parameters.
		*/
	public void useActiveDatagridCollection(ActiveDatagridCollection adcNow) {
		super.setDataSetNow(null);
		super.setContainerNow(null);
		super.setActiveDataSetNow(null);
		super.setCollectionNow(null);
		super.setADCnow(adcNow);
	}

	/**
		* Makes the StdDatagridObject use the specified ActiveDatagridCollection.
		* This method clobbers any DataSet, Container, Collection,
		* and ActiveDatagridCollection parameters.
		*/
	public void useActiveDatagridDataSet(ActiveDatagridDataSet activeDataSetNow) {
		super.setDataSetNow(null);
		super.setContainerNow(null);
		super.setADCnow(null);
		super.setCollectionNow(null);
		super.setActiveDataSetNow(activeDataSetNow);
	}
}
