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

package edu.sdsc.matrix.srb.operations;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.net.URL;
import java.util.Iterator;
import java.util.List;

import javax.naming.OperationNotSupportedException;

import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.ExecuteProcessOp;
import edu.sdsc.matrix.srb.parser.ExecuteProcessParams;
import edu.sdsc.matrix.srb.parser.ExecuteProcessResult;
import edu.sdsc.matrix.srb.parser.OpResult;
import edu.sdsc.matrix.srb.parser.StdDataSet;
import edu.sdsc.matrix.srb.parser.StdStreamData;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.parser.TPart;
import edu.sdsc.matrix.srb.util.CompileClassLoader;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import org.griphyn.vdl.classes.LFN;
import org.griphyn.vdl.dax.ADAG;
import org.griphyn.vdl.dax.Filename;
import org.griphyn.vdl.dax.Job;
import org.griphyn.vdl.dax.PseudoText;

import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBRandomAccessFile;

// Created by arun = Feb 05
/**
	* This class will allow execution of a pr0cess
	*
	* @author Arun swaran Jagatheesan (arun@sdsc.edu)
	*
	*/
public class ExecuteProcessOperation extends MatrixAgent{
			public ExecuteProcessOperation(Step step, FlowThread parent,
					ThreadGroup group){
						super(step, parent, group);
			}

			/**
				* The method that executes the process
				*
				* @throws java.lang.Exception
				*/
			public void typeSpecificExecute() throws Exception{
						toLog("ExecuteProcess- starting()", MatrixUtil.DEBUG_LOG);
						// get input variables
						ExecuteProcessOp exeOprn = getOperation().getExecuteProcessOp();
						ExecuteProcessParams exeParams = exeOprn.getStdParams();
						String uri = evaluateExpressionToString(exeParams.getExeURI().trim());
						String exeName = null;
						String exeMethod = "main";
						if (exeParams.getExeName() != null)
									exeName = evaluateExpressionToString(exeParams.getExeName());
						if (exeParams.getExeMethod() != null)
									exeMethod = evaluateExpressionToString(exeParams.getExeMethod());
						List inputlist = exeParams.getInput();
						String outputStr = "";
						File file = null;
						String errorString = null;
						// find out what the filename is
						String fileName = null;
						//if there is no exeName, then the filename is the name of the executable
						if (exeName == null || exeName.trim().equals(""))
									fileName = uri.substring(uri.lastIndexOf('/') + 1);
						else
									fileName = exeName.trim();
									// get some information about the type of file this is
						boolean isClass = fileName.endsWith(".class");
						boolean isJava = uri.endsWith(".java");
						// if this is an http uri
						if (uri.startsWith("http:")){
									URL url = new URL(uri);
									BufferedInputStream bis = new BufferedInputStream(url.openStream());
									file = new File(fileName);
									FileOutputStream fos = new FileOutputStream(file);
									byte[] bytebuffer = new byte[4096];
									int length = 0;
									while ((length = bis.read(bytebuffer)) != -1){
												fos.write(bytebuffer, 0, length);
									}
									bis.close();
									fos.close();
						}
						// add any other protocols here, e.g. ftp:, srb:

						// execute the command as an SRB Proxy command
						if (file == null){
									// get connection to the SRB
									SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
									SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
									// create a string that represents the arguments - a proxy command
									// can only accept arguments in string format
									StringBuffer args = new StringBuffer();
									//ARUN perfection: this firstParam is not needed for the logic
									//ARUn removed firstParam and made necessary changes
									//boolean firstParam = true;
									for (Iterator i = inputlist.iterator(); i.hasNext(); ){
												// get the value of the variable
												TPart part = (TPart) i.next();
												Object value = MatrixUtil.extractVariable(part);
												// if the variable has a value, then make a String out of it
												if (value != null){
															// add a space if this isnt the first parameter
															//if (!firstParam)	args.append(" ");
															String arg = this.evaluateExpressionToString(value.toString());
															args.append(arg +" ");
															//firstParam = false;
												}
									}
									args.setLength(args.length()-1);
									// execute the command and grab the output
									InputStream in = srbFileSystem.executeProxyCommand(fileName,
											args.toString());
									// if the user specified a stdOut, then record the output
									if (exeParams.getStdOut() != null){
												// make a string out of the output
												StringBuffer outputBuffer = new StringBuffer();
												int c;
												while ((c = in.read()) != -1){
															outputBuffer.append((char) c);
												}
												// get the stdOut that the user specified
												StdStreamData stream = this.resolvePossibleReference(exeParams.
														getStdOut());
												// if the stdOut is to a variable
												if (stream.getVariable() != null){
															// get the variable we are supposed to stream the output to
															TPart variable = this.getExpressionEvaluator().getVarValue(
																	stream.getVariable());
															// we only support string here
															// if we are to stream to a String
															if (variable.getString() != null){
																		// remove any old values just in case (though this SHOULDNT be necessary)
																		MatrixUtil.clearTPart(variable);
																		// place the output into the variable
																		variable.setString(outputBuffer.toString());
															}
															// update the variable in the store
															this.getStore().setVarValue(this.getMatrixThreadId(), variable);
												}
												// if the stdOut is to a dataSet, we need to write to a file
												else if (stream.getDataSet() != null){
															StdDataSet dataSet = this.resolvePossibleReference(stream.
																	getDataSet());
															SRBFile outputFile = new SRBFile(srbFileSystem,
																	dataSet.getDataIdentifier());
															outputFile.createNewFile(); // make sure the file exists
															SRBRandomAccessFile srbRandomAccessFile = new
																	SRBRandomAccessFile(outputFile, "rw");
															// write the string to file
															srbRandomAccessFile.write(outputBuffer.toString());
															srbRandomAccessFile.close();
												}
									}
									in.close();
						} else{ // if the file exists, then we downloaded the executable
									if (isJava){
												//needs to be compiled first
												CompileClassLoader ccl = new CompileClassLoader();
												if (!ccl.compile(file.getAbsolutePath()))
															errorString = "Java file could not be compiled properly";
												else
															isClass = true; //will be compiled in the next step
									}
									if (isClass){
												// eventually, the user will want to pass in variables that can be of
												// any type, we have to examine the type and translate it to a java
												// type or something
												String[] inputStr = new String[inputlist.size()];
												CompileClassLoader ccl = new CompileClassLoader();
												String classFile = file.getName().substring(0,
														file.getName().lastIndexOf(".class"));
												toLog("ExecuteProcess - attempting to loadClass() " + classFile,
														MatrixUtil.DEBUG_LOG);
												Class executableClass = ccl.loadClass(classFile, false);
												Class[] parameterTypes = new Class[inputStr.length];
												for (int i = 0; i < parameterTypes.length; i++){
															inputStr[i] = this.evaluateExpressionToString(inputStr[i]);
															parameterTypes[i] = inputStr[i].getClass();
												}
												toLog("ExecuteProcess- creating new instance()" +
														file.getAbsolutePath(), MatrixUtil.DEBUG_LOG);
												//for now using the default constructor
												Object objectToexecute = executableClass.newInstance();
												Method newMethod = executableClass.getMethod(exeMethod,
														parameterTypes);
												outputStr = (String) newMethod.invoke(objectToexecute,
														(Object[]) inputStr);
												toLog("ExecuteProcess - result (now a string)=" + outputStr,
														MatrixUtil.DEBUG_LOG);
												file.delete();
									}
									//add code to support any ohter new executatble here
									toLog("ExecuteProcessOperation: store result", MatrixUtil.DEBUG_LOG);
									ExecuteProcessResult exeResult = getObjectFactory().
											createExecuteProcessResult();
									exeResult.getOutput().add(outputStr);
									OpResult opResult = getObjectFactory().createOpResult();
									opResult.setExecuteProcessResult(exeResult);
									setResponse(opResult);
									toLog("ExectuteProcess() -END", MatrixUtil.DEBUG_LOG);
						}

			} //run

			/**
				* Get Operation Name
				* @returns the name of the operation encapsulated by this agent
				*/
			public String getOperationName(){
						return "ExecuteProcessOperation";
			} //getOperationName

			/**
				* Converts this operation to a org.griphyn.vdl.dax.Job
				* @return Job
				*/
			private Job toDAXJob(){
						// first extract the job parameters from the DGL
						ExecuteProcessOp dglJob = (ExecuteProcessOp)this.getOperation().
								getExecuteProcessOp();
						ExecuteProcessParams jobParams = dglJob.getStdParams();
						// create a DAX Job: not really sure what the implications of these
						// parameters are yet
						Job daxJob = new Job("namespace",
								this.evaluateExpressionToString(jobParams.getExeURI()),
								"version",
								this.getMatrixThreadId());
						//// set the used files (input and output files) -
						//// Note: how come we can't we set the link and register attributes?
						///// Are they derived from the filename list in the ADAG?
						if (jobParams.getStdIn() != null){
									// set input file
									StdStreamData stream = resolvePossibleReference(jobParams.getStdIn());
									StdDataSet dataSet = resolvePossibleReference(stream.getDataSet());
									daxJob.setStdin(new Filename(this.evaluateExpressionToString(dataSet.
											getDataIdentifier())));
						}
						if (jobParams.getStdOut() != null){
									// set output file
									StdStreamData stream = resolvePossibleReference(jobParams.getStdOut());
									StdDataSet dataSet = resolvePossibleReference(stream.getDataSet());
									daxJob.setStdout(new Filename(this.evaluateExpressionToString(dataSet.
											getDataIdentifier())));
						}
						// add the command line arguments
						java.util.Collection args = dglJob.getStdParams().getInput();
						if (args != null){
									for (Iterator i = dglJob.getStdParams().getInput().iterator();
														i.hasNext(); ){
												TPart param = (TPart) i.next();
												// DAX currently only supports Strings and filenames...
												if (param.getString() != null){
															daxJob.addArgument(new PseudoText(this.
																	evaluateExpressionToString(param.getString())));
												} else if (param.getDataSet() != null){
															StdDataSet dataSet = resolvePossibleReference(param.getDataSet());
															String filename = this.evaluateExpressionToString(dataSet.
																	getDataIdentifier());
															daxJob.addArgument(new Filename(filename));
												}
									}
						}
						return daxJob;
			}

			public ADAG toDAX() throws OperationNotSupportedException{
						// create an adag and add this step as a job
						ADAG adag = new ADAG();
						adag.addJob(this.toDAXJob());
						////// register the input/output files //////////
						// first extract the job parameters from the DGL
						ExecuteProcessOp dglJob = (ExecuteProcessOp)this.getOperation().
								getExecuteProcessOp();
						ExecuteProcessParams jobParams = dglJob.getStdParams();
						// params are: logical filename, isInput, physical location [optional],
						// dontRegister, dontTransfer
						if (jobParams.getStdIn() != null){
									StdStreamData stream = resolvePossibleReference(jobParams.getStdIn());
									StdDataSet dataSet = resolvePossibleReference(stream.getDataSet());
									adag.addFilename(this.evaluateExpressionToString(dataSet.
											getDataIdentifier()),
											true,
											dataSet.getPhysicalLocation(),
											true, LFN.XFER_OPTIONAL);
						}
						if (jobParams.getStdOut() != null){
									StdStreamData stream = resolvePossibleReference(jobParams.getStdOut());
									StdDataSet dataSet = resolvePossibleReference(stream.getDataSet());
									adag.addFilename(this.evaluateExpressionToString(dataSet.
											getDataIdentifier()),
											false,
											dataSet.getPhysicalLocation(),
											true, LFN.XFER_OPTIONAL);
						}
						return adag;
			}

} //class