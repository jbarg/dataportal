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

	/* Developer Log
		*  Changed licence Dec 22, 2004 - Arun
		*/

package edu.sdsc.matrix.srb.client;

import java.util.Iterator;

/**
	* Contains methods to create a ControlStructure. As of DGL 3.0 we support ForLoop, ParallelSplit,
	* WhileLoop and the traditional Sequential Split.
	*
	* @author  Arun Jagatheesan
	* @since 3.0
	*/
public class AnyControlStructure extends edu.sdsc.matrix.srb.parser.impl.AnyControlStructureImpl {

	public AnyControlStructure(edu.sdsc.matrix.srb.parser.AnyControlStructure controlStruct)
	{
		if(controlStruct.getForLoop() != null)
			super.setForLoop(new TForLoop(controlStruct.getForLoop()));

		else if(controlStruct.getParallel() != null)
			super.setParallel(new TParallel(controlStruct.getParallel()));

		else if(controlStruct.getSequential() != null)
			super.setSequential(new TSequential(controlStruct.getSequential()));

		else if(controlStruct.getWhileLoop() != null)
			super.setWhileLoop(new TWhileLoop(controlStruct.getWhileLoop()));

		else if(controlStruct.getSwitchBlock() != null)
			super.setSwitchBlock(new TSwitch(controlStruct.getSwitchBlock()));

		else if(controlStruct.getForEachLoop() != null)
			super.setForEachLoop(new TForEachLoop(controlStruct.getForEachLoop()));

		for (Iterator iter = controlStruct.getUserDefinedRules().iterator(); iter.hasNext(); )
		{
		super.getUserDefinedRules().add(
					new TRule((edu.sdsc.matrix.srb.parser.TRule) iter.next()));
		}
	}

	public AnyControlStructure(TForLoop value) {
		this.setForLoop(value);
	}

	public AnyControlStructure(TParallel value) {
		this.setParallel(value);
	}

	public AnyControlStructure(TSequential value) {
		this.setSequential(value);
	}

	public AnyControlStructure(TWhileLoop value) {
		this.setWhileLoop(value);
	}

			public AnyControlStructure(TForEachLoop value) {
		this.setForEachLoop(value);
	}

			public AnyControlStructure(TSwitch value) {
		this.setSwitchBlock(value);
	}
}
