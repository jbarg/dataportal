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

public class TIterator extends edu.sdsc.matrix.srb.parser.impl.TIteratorImpl
{
		//============================= constructors ============================//
		public TIterator (edu.sdsc.matrix.srb.parser.TIterator iterator)
		{
				this.setIterationVariableName(iterator.getIterationVariableName());

				if(iterator.getExternalURL() != null)
						this.setExternalURL(iterator.getExternalURL());
				if(iterator.getExternalUser() != null)
						this.setExternalUser(new UserInfo(iterator.getExternalUser()));


				if(iterator.getCollection() != null)
						this.setIterationSet(new Collection(iterator.getCollection()));
				if(iterator.getCollectionListing() != null)
						this.setIterationSet(new CollectionList(iterator.getCollectionListing()));
				if(iterator.getDataSet() != null)
						this.setDataSet(new DataSet(iterator.getDataSet()));
				if(iterator.getStringSet() != null)
						this.setStringSet(new StringSet(iterator.getStringSet()));



		}

		public TIterator(String iterationVariableName)
		{ this.setIterationVariableName(iterationVariableName); }
		public TIterator(String iterationVariableName, Collection collection)
		{
				this.setCollection(collection);
				this.setIterationVariableName(iterationVariableName);
		}
		public TIterator(String iterationVariableName, CollectionList collectionList)
		{
				this.setCollectionListing(collectionList);
				this.setIterationVariableName(iterationVariableName);
		}
		public TIterator(String iterationVariableName, DataSet dataSet)
		{
				this.setDataSet(dataSet);
				this.setIterationVariableName(iterationVariableName);
		}
		public TIterator(String iterationVariableName, String externalURL)
		{
				this.setExternalURL(externalURL);
				this.setIterationVariableName(iterationVariableName);
		}
		public TIterator(String iterationVariableName, UserInfo externalUser)
		{
				this.setExternalUser(externalUser);
				this.setIterationVariableName(iterationVariableName);
		}
		public TIterator(String iterationVariableName, StringSet s)
		{
				this.setStringSet(s);
				this.setIterationVariableName(iterationVariableName);
		}


		//========================= accessor methods ===============================//
		public void setIterationSet(Collection collection)
		{
				clearIterationSets();
				this.setCollection(collection);
		}

		public void setIterationSet(CollectionList collectionList)
		{
				clearIterationSets();
				this.setCollectionListing(collectionList);
		}

		public void setIterationSet(DataSet dataSet)
		{
				clearIterationSets();
				this.setDataSet(dataSet);
		}

		public void setIterationSet(String externalURL)
		{
				clearIterationSets();
				this.setExternalURL(externalURL);
		}

		public void setIterationSet(UserInfo userInfo)
		{
				clearIterationSets();
				this.setExternalUser(userInfo);
		}

		public void setIterationSet(StringSet stringSet)
		{
				clearIterationSets();
				this.setStringSet(stringSet);
		}

		public void clearIterationSets()
		{
				this.setCollection(null);
				this.setCollectionListing(null);
				this.setDataSet(null);
				this.setExternalURL(null);
				this.setExternalUser(null);
				this.setStringSet(null);
		}

}
