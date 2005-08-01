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

import java.math.BigInteger;
import java.lang.IllegalStateException;

/**
	* Creates a Flow of type ForEachLoop control structure.
	*
	* @ author Jon Weinberg
	*/

/* Developer Log
	* Original code Jon & Arun
	* This developer log, formatting 2/2/05 Arun
	* Added some checks to setParallelIncrement 2/2/05 Arun
	*/
public class ForEachFlow extends Flow{
			public ForEachFlow(edu.sdsc.matrix.srb.parser.Flow flow){
						super(flow);
			}

			/**
				* Creates a Flow with this ID and a parallel increment of 1
				* @param flowID String
				*/
			public ForEachFlow(String flowID){
						super(flowID);
						TForEachLoop logic = new TForEachLoop();
						logic.setParallelIncrement(new BigInteger("1"));
						AnyControlStructure control = new AnyControlStructure(logic);
						super.setFlowLogic(control);
			}

			public ForEachFlow(String flowID, String iteratorName, Collection value){
						super(flowID);
						TForEachLoop logic = new TForEachLoop();
						logic.setParallelIncrement(new BigInteger("1"));
						AnyControlStructure control = new AnyControlStructure(logic);
						logic.addIterator(new TIterator(iteratorName, value));
						super.setFlowLogic(control);
			}

			public ForEachFlow(String flowID, String iteratorName, CollectionList value){
						this(flowID, iteratorName, value, 1);
			}

			public ForEachFlow(String flowID, String iteratorName, CollectionList value,
					int parallelIncrement){
						super(flowID);
						TForEachLoop logic = new TForEachLoop();
						logic.setParallelIncrement(new BigInteger("" + parallelIncrement));
						AnyControlStructure control = new AnyControlStructure(logic);
						logic.addIterator(new TIterator(iteratorName, value));
						super.setFlowLogic(control);
			}

			public ForEachFlow(String flowID, String iteratorName, String externalURL){
						super(flowID);
						TForEachLoop logic = new TForEachLoop();
						logic.setParallelIncrement(new BigInteger("1"));
						AnyControlStructure control = new AnyControlStructure(logic);
						logic.addIterator(new TIterator(iteratorName, externalURL));
						super.setFlowLogic(control);
			}

			public ForEachFlow(String flowID, String iteratorName, DataSet value){
						super(flowID);
						TForEachLoop logic = new TForEachLoop();
						logic.setParallelIncrement(new BigInteger("1"));
						AnyControlStructure control = new AnyControlStructure(logic);
						logic.addIterator(new TIterator(iteratorName, value));
						super.setFlowLogic(control);
			}

			public ForEachFlow(String flowID, String iteratorName, StringSet value){
						super(flowID);
						TForEachLoop logic = new TForEachLoop();
						logic.setParallelIncrement(new BigInteger("1"));
						AnyControlStructure control = new AnyControlStructure(logic);
						logic.addIterator(new TIterator(iteratorName, value));
						super.setFlowLogic(control);
			}

			/**
				* Creates a Flow with given ID and parallelIncrement
				* @param flowID String
				* @param parallelIncrement int the number of iterations of the forEachLoop to
				* execute in parallel
				*/
			public ForEachFlow(String flowID, int parallelIncrement){
						super(flowID);
						TForEachLoop logic = new TForEachLoop(new BigInteger(parallelIncrement + ""));
						logic.setParallelIncrement(new BigInteger("1"));
						super.setFlowLogic(new AnyControlStructure(logic));
			}

			public void setParallelIncrement(int parallelIncrement){
						if ((this.getFlowLogic() != null) && (this.getFlowLogic().getForEachLoop() != null)){
									this.getFlowLogic().getForEachLoop().setParallelIncrement(new BigInteger(
											parallelIncrement + ""));
						} else{
									throw new IllegalStateException("ParallelIncrementNotSet");
						}
			}

			public int getParallelIncrement(){
						if ((this.getFlowLogic() != null) && (this.getFlowLogic().getForEachLoop() != null)){
									return this.getFlowLogic().getForEachLoop().getParallelIncrement().intValue();
						} else{
									throw new IllegalStateException("ForEachLoopNotSet");
						}
			}

			public void addIterator(TIterator iterator){
						this.getFlowLogic().getForEachLoop().getIterationSet().add(iterator);
			}

			/**
				* Set the given flow as a child of this ForEachFlow
				* @param flow Flow
				*/
			public void addChild(Flow flow){
						if ((this.getFlow().size() + this.getStep().size()) > 0)
									throw new RuntimeException("Cannot add a second child to a ForEachFlow");
						this.getFlow().add(flow);
			}

			/**
				* Set the given step as a child of this ForEachFlow
				* @param step Step
				*/
			public void addChild(Step step){
						if ((this.getFlow().size() + this.getStep().size()) > 0)
									throw new RuntimeException("Cannot add a second child to a ForEachFlow");
						this.getStep().add(step);
			}

			/**
				* Returns the Step or Flow that is the child of this Flow or null
				* if this flow has no children
				* @return Object
				*/
			public Object getChild(){
						if (this.getFlow().size() != 0)
									return this.getFlow().get(0);
						if (this.getStep().size() != 0)
									return this.getStep().get(0);
						return null;
			}
}