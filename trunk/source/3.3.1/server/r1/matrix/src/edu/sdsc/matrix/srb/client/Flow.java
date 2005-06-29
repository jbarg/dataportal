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
	* Flow.java
	*
	* Created on July 8, 2003, 7:02 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import edu.sdsc.matrix.srb.parser.impl.FlowImpl;

/**
	* Part of the Matrix client API.
	* Contains methods to create Flow objects.
	* @author  Allen Ding - SDSC
	*/
public class Flow extends edu.sdsc.matrix.srb.parser.impl.FlowImpl {

	public Flow(edu.sdsc.matrix.srb.parser.Flow flow)
	{

		super.setFlowID(flow.getFlowID());
		setFlowLogic(new AnyControlStructure(flow.getFlowLogic()));

		if(flow.getFlow() != null && flow.getFlow().size() != 0)
		{
			for(Iterator i = flow.getFlow().iterator(); i.hasNext();)
			{ this.addChild(new Flow((edu.sdsc.matrix.srb.parser.Flow)i.next())); }
		}
		else if(flow.getStep() != null)
		{
				for(Iterator i = flow.getStep().iterator(); i.hasNext();)
			{
					this.addChild(new Step((edu.sdsc.matrix.srb.parser.Step)i.next())); }
		}

		if(flow.getVariables() != null)
			this.setVariables(new TVariables(flow.getVariables()));
	}

	/**
		* Creates a flow object with no initial name
		* Steps or Flows must be set later!
		*/
	protected Flow(String flowID) {
		super.setFlowID(flowID);
					this.setVariables(new TVariables());
	}

		/**
		* Add the given flow as a child of this Flow
		* @param flow Flow
		*/
	public void addChild(Flow flow)
	{
		if(this.getStep().size() > 0)
			throw new RuntimeException("Flow cannot contain children of both types Flow and Step");
		this.getFlow().add(flow);
	}

	/**
		* Add the given step as a child of this Flow
		* @param step Step
		*/
	public void addChild(Step step)
	{
		if(this.getFlow().size() > 0)
			throw new RuntimeException("Flow cannot contain children of both types Flow and Step");
		this.getStep().add(step);
	}

	public void removeChild(Flow child)
	{ this.getFlow().remove(child);
	}

	public void removeChild(Step child)
	{ this.getStep().remove(child); }

		/**
		* Returns a list containing the Steps or Flows that are the children of this
		* Flow
		* @return Object
		*/
	public java.util.List getChildren()
	{
		List list = new ArrayList();

		if(this.getFlow().size() != 0)
			list.addAll(this.getFlow());
		else if(this.getStep().size() != 0)
			list.addAll(this.getStep());

		return list;
	}

	/**
		* Method for other classes in package to get a single level
		* ArrayList accumulation of SOAP AttachmentPart's from all the steps
		* that are in this Flow.
		*/
	protected ArrayList getAttachments() {
		ArrayList returnList = new ArrayList();
		if ( (super.getFlow() == null) || (super.getFlow().size() == 0)) {
			//search for attachments for all steps in this flow
			for (Iterator i = super.getStep().iterator();
					i.hasNext(); ) {
				Step step = (Step) i.next();
				//if the step has attachments, add them to the return list
				if (step.getAttachments() != null) {
					//extract each attachmentpart and store in new list
					for (Iterator x = step.getAttachments().
							iterator(); x.hasNext(); ) {
						returnList.add(x.next());
					}
				}
			}
		} else {
			for (Iterator i = super.getFlow().iterator(); i.hasNext(); ) {
				Flow flow = (Flow) i.next();
				//if the step has attachments, add them to the return list
				if (flow.getAttachments() != null) {
					//extract each attachmentpart and store in new list
					for (Iterator x = flow.getAttachments().
							iterator(); x.hasNext(); ) {
						returnList.add(x.next());
					}
				}
			}
		}
		return returnList;
	}

			/**
			* Adds the TPart as a variable of this flow
			* @param variable TPart
			*/
			public void addVariable(TPart variable)
			{
				// if no variable container exists, create a new one
				if(this.getVariables() == null)
						this.setVariables(new TVariables());


				// just add the variable (this doesnt check for duplicates)
				this.getVariables().getPart().add(variable);
			}

			/**
			* Gets the TPart with the given name
			* @param name String
			* @return TPart
			*/
			public TPart getVariable(String name)
			{
				if(this.getVariables() == null)
					return null;

				for(Iterator i = this.getVariables().getPart().iterator(); i.hasNext();)
				{
					TPart part = (TPart)i.next();
					if(part.getName().equals(name))
						return part;
				}

				return null;
			}
			/**
			*
			* @param name String
			* @param value String
			*/
			public void addVariable(String name, String value)
			{ this.addVariable(new TPart(name, value)); }

			/**
			*
			* @param name String
			* @param value StringSet
			*/
			public void addVariable(String name, StringSet value)
			{ this.addVariable(new TPart(name, value)); }

			/**
			*
			* @param name String
			* @param value BigDecimal
			*/
			public void addVariable(String name, BigDecimal value)
			{ this.addVariable(new TPart(name, value)); }

                        /**
                                * Add a variable of type int
                                * @param name String
                                * @param value int
                                */
                        public void addVariable(String name, int value){
                                                this.addVariable(name, new BigDecimal(new Integer(value).doubleValue()));
                        }

			/**
			*
			* @param name String
			* @param value StreamData
			*/
			public void addVariable(String name, StreamData value)
			{ this.addVariable(new TPart(name, value)); }

			/**
			*
			* @param name String
			* @param value DataSet
			*/
			public void addVariable(String name, DataSet value)
			{ this.addVariable(new TPart(name, value)); }

			/**
			*
			* @param name String
			* @param value CollectionList
			*/
			public void addVariable(String name, CollectionList value)
			{ this.addVariable(new TPart(name, value)); }

			/**
			*
			* @param name String
			* @param value Collection
			*/
			public void addVariable(String name, Collection value)
			{ this.addVariable(new TPart(name, value)); }

			/**
			* Adds the given rule
			* @param rule TRule
			*/
			public void addRule(TRule rule)
			{
				super.getFlowLogic().getUserDefinedRules().add(rule);
			}

		/**
			* Returns a list of rules (TRule) that are associated with this FLow
			* @return List of TRules
			*/
			public List getRules(){
			return super.getFlowLogic().getUserDefinedRules();
		}

			/**
			* Adds an action that always executes the given actionString either
			* beforeEntry or afterExit depending on the given "beforeEntry" parameter
			* @param beforeEntry boolean
			* @param simpleCondition String
			*/
			public void addAction(boolean beforeEntry, String actionString)
			{
				TRule rule = new TRule(beforeEntry, "true");
				TAction action = new TAction("true", actionString);
				rule.addAction(action);
				this.addRule(rule);
			}
}
