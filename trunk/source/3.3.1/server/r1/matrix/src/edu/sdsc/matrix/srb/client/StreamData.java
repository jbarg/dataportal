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

public class StreamData extends edu.sdsc.matrix.srb.parser.impl.StreamDataImpl
{
		public StreamData(edu.sdsc.matrix.srb.parser.StreamData streamData)
		{
				this.setStdStreamData(streamData.getStdStreamData());
				this.setStreamDataReference(streamData.getStreamDataReference());
		}

		/**
			* Creates a new StreamData reference.  Same as calling new StreamData(reference, true)
			* @param reference String
			*/
		public StreamData(String reference)
		{
			this(reference, true);
		}

		/**
			* If isReference, then this creates a new StreamData reference to the given varName
			* If not isReference, then this creates a new StreamData with the "variable" field set
			* to the given varName.
			*
			* E.G. -
			* new StreamData("$var", false) streams the data to whatever $var resolves to
			* new StreamData("$var", true) creates a StreamData as a reference type to $var
			*
			*
			* @param reference String
			* @param isStreamData boolean
			*/
		public StreamData (String varName, boolean isReference)
		{
				if(isReference)
				{
						VariableReference var = new VariableReference(varName);
						this.setStreamDataReference(var);
				}
				else
						this.setStdStreamData(new StdStreamData(varName));
		}

		public StreamData (StdStreamData std)
		{
				this.setStdStreamData(std);
		}

		public StreamData(DataSet dataSet)
		{ this.setStdStreamData(new StdStreamData(dataSet)); }

}
