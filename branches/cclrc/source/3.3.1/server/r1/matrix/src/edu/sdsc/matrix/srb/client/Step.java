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
	* Step.java
	*
	* Created on July 8, 2003, 6:59 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import edu.sdsc.matrix.srb.parser.impl.StepImpl;

/**
	* Part of the Matrix client API.
	* Contains methods to create Step objects.
	* @author  Allen Ding - SDSC
	*/
public class Step extends edu.sdsc.matrix.srb.parser.impl.StepImpl{

			public Step(edu.sdsc.matrix.srb.parser.Step step){
						// set the id and variables
						this.setStepID(step.getStepID());
						if (step.getVariables() != null)
									this.setVariables(new TVariables(step.getVariables()));
						this.setOperation(new Operation(step.getOperation()));
			}

			/**
				* Creates a Step object with the <b>required</b> stepID and
				* operation parameters.
				* @param stepID The step ID.
				* @param operation The operation to perform.
				*/
			public Step(String stepID, Operation operation){
						super.setStepID(stepID);
						super.setOperation(operation);
			}

			protected ArrayList getAttachments(){
						return ((Operation)super.getOperation()).getAttachments();
			}

			/**
				* Adds the TPart as a variable of this step
				* @param variable TPart
				*/
			public void addVariable(TPart variable){
						// if no variable container exists, create a new one
						if (this.getVariables() == null)	this.setVariables(new TVariables());
						// just add the variable (this doesnt check for duplicates)
						this.getVariables().getPart().add(variable);
			}

			/**
				* Gets the TPart with the given name
				* @param name String
				* @return TPart
				*/
			public TPart getVariable(String name){
						if (this.getVariables() == null)
									return null;
						for (Iterator i = this.getVariables().getPart().iterator(); i.hasNext(); ){
									TPart part = (TPart) i.next();
									if (part.getName().equals(name))
												return part;
						}
						return null;
			}

			/**
				* Add a variable of type String
				* @param name String
				* @param value String
				*/
			public void addVariable(String name, String value){
						this.addVariable(new TPart(name, value));
			}

			/**
				* Add a variable of type StringSet (collection of strings)
				* @param name String
				* @param value StringSet
				*/
			public void addVariable(String name, StringSet value){
						this.addVariable(new TPart(name, value));
			}

			/**
				* Add a variable of type decimal (big decimal)
				* @param name String
				* @param value BigDecimal
				*/
			public void addVariable(String name, BigDecimal value){
						this.addVariable(new TPart(name, value));
			}

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
			public void addVariable(String name, StreamData value){
						this.addVariable(new TPart(name, value));
			}

			/**
				*
				* @param name String
				* @param value DataSet
				*/
			public void addVariable(String name, DataSet value){
						this.addVariable(new TPart(name, value));
			}

			/**
				*
				* @param name String
				* @param value CollectionList
				*/
			public void addVariable(String name, CollectionList value){
						this.addVariable(new TPart(name, value));
			}

			/**
				*
				* @param name String
				* @param value Collection
				*/
			public void addVariable(String name, Collection value){
						this.addVariable(new TPart(name, value));
			}

			/**
				* Adds the given rule
				* @param rule TRule
				*/
			public void addRule(TRule rule){
						super.getUserDefinedRules().add(rule);
			}

			/**
				* Same as getUserDefinedRules
				* @return list of Userdefined rules associated with this step
				*/
			public List getRules(){
						return super.getUserDefinedRules();
			}

			/**
				* Adds an action that always executes the given actionString either
				* beforeEntry or afterExit depending on the given "beforeEntry" parameter
				* Refer Matrix Userguide for documentaion on beforeEntry and afterExit
				* @param beforeEntry boolean true="beforeEntry", false="afterExit"
				* @param simpleCondition String
				*/
			public void addAction(boolean beforeEntry, String actionString){
						TRule rule = new TRule(beforeEntry, "true");
						TAction action = new TAction("true", actionString);
						rule.addAction(action);
						this.addRule(rule);
			}
}
