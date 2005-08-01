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

import edu.sdsc.matrix.srb.util.MatrixUtil;

public class ExecuteProcessStep extends Step
{
		public ExecuteProcessStep(edu.sdsc.matrix.srb.parser.Step step)
		{
				super(step.getStepID(), new Operation(step.getOperation()));
				if(step.getOperation().getExecuteProcessOp() == null)
				{
						throw new RuntimeException("Cannot construct an ExecuteProcessStep from "+
																																"a Step whose executeProcessOp is null");
				}
		}

		/**
			* Creates an ExecuteProcess step to execute the file at the given uri
			* @param id String
			* @param uri String
			*/
		public ExecuteProcessStep(String id, String uri)
		{ super(id, new Operation(new ExecuteProcessOp(new ExecuteProcessParams(uri)))); }

		public ExecuteProcessStep(String id, String uri, String methodName)
		{
				super(id, new Operation(new ExecuteProcessOp(new ExecuteProcessParams(uri, methodName))));
		}

		/**
			* Adds a parameter that will be evaluated as an expression during runtime
			* @param expression String
			*/
		public void addParameterAsExpression(String expression)
		{
				this.addParameterAsObject(expression);
		}

				/**
			* Adds a parameter used to invoke the function.  If the parameter type is not
			* supported by DGL (tPart), then the parameter is added as a String using it's
			* toString() method.  String parameters added in this way are not subject to
			* DGL expression evaluation
			* @param param Object
			*/
		public void addParameter(Object param)
		{
				// if this is a string, we need to wrap it in quotes again so that the
				// expression evaluator doesn't try to evaluate it
				if(param instanceof String)
						param = "\""+param+"\"";

				addParameterAsObject(param);
		}

		/**
			* Removes the parameter with the given value
			* @param param Object
			*/
		public void removeParameter(Object param)
		{
				try
				{
						TPart part = new TPart(MatrixUtil.createTPart(param));
						this.getOperation().getExecuteProcessOp().getStdParams().getInput().remove(part);
				}
				catch(javax.xml.bind.JAXBException jbe)
				{ throw new RuntimeException("Cannot remove variable: "+param, jbe);}
		}

		// internal method simply creates a TPart out of the given parameter and adds
		// it to the input parameters
		private void addParameterAsObject(Object param)
		{
				try
				{
						TPart part = new TPart(MatrixUtil.createTPart(param));
						part.setName(param.toString());
						this.getOperation().getExecuteProcessOp().getStdParams().getInput().add(part);
				}
				catch(javax.xml.bind.JAXBException jbe)
				{throw new RuntimeException("Cannot add variable: "+param, jbe);}
		}

		//====================== generic accessor methods ==========================//

		public String getExeURI()
		{ return this.getOperation().getExecuteProcessOp().getStdParams().getExeURI(); }

		public void setExeURI(String uri)
		{ this.getOperation().getExecuteProcessOp().getStdParams().setExeURI(uri); }

		public String getExeMethod()
		{ return this.getOperation().getExecuteProcessOp().getStdParams().getExeMethod(); }

		public void setExeMethod(String method)
		{ this.getOperation().getExecuteProcessOp().getStdParams().setExeMethod(method); }

		public String getExeName()
		{ return this.getOperation().getExecuteProcessOp().getStdParams().getExeName(); }

		public void setExeName(String name)
		{ this.getOperation().getExecuteProcessOp().getStdParams().setExeName(name); }


		////// accessors for stdOut //////
		public void setStdOut(StreamData stream)
		{ this.getOperation().getExecuteProcessOp().getStdParams().setStdOut(stream); }

		public void setStdOut(DataSet dataSet)
			{ this.getOperation().getExecuteProcessOp().getStdParams().setStdOut(new StreamData(dataSet)); }

		public StreamData getStdOutStream()
		{ return
								new StreamData(this.getOperation().getExecuteProcessOp().getStdParams().getStdOut()); }


		////// accessors for stdIn //////
			public void setStdIn(StreamData stream)
		{ this.getOperation().getExecuteProcessOp().getStdParams().setStdIn(stream); }

		public void setStdIn(DataSet dataSet)
			{ this.getOperation().getExecuteProcessOp().getStdParams().setStdIn(new StreamData(dataSet)); }

		public StreamData getStdInStream()
		{ return
								new StreamData(this.getOperation().getExecuteProcessOp().getStdParams().getStdIn()); }


		////// accessors for stdErr //////
			public void setStdErr(StreamData stream)
		{ this.getOperation().getExecuteProcessOp().getStdParams().setStdErr(stream); }

		public void setStdErr(DataSet dataSet)
			{ this.getOperation().getExecuteProcessOp().getStdParams().setStdErr(new StreamData(dataSet)); }

		public StreamData getStdErrStream()
		{ return
								new StreamData(this.getOperation().getExecuteProcessOp().getStdParams().getStdErr()); }

}
