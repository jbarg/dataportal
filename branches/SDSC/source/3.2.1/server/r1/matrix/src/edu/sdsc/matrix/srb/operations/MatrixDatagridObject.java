/* ====================================================================
 * SDSC Matrix Software License, Version 1.0
 *
 * Copyright (c) 2003 San Diego Supercomputer Center (SDSC), San Diego, USA.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that ALL the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        SDSC Matrix Project (http://www.npaci.edu/DICE/SRB/matrix/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "SDSC" and "SDSC Matrix" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact arun@sdsc.edu or srb-chat@sdsc.edu.
 *
 * 5. Products derived from this software may not be called "SDSC",
 *    nor may "SDSC" appear in their name, without prior written
 *    permission of the SDSC.
 *
 * 6. License is not granted for commercial resale, in whole or in part, 
 *	  without the prior written permission from SDSC.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL SDSC OR
 * ITS CONTRIBUTORS (SDSC Matrix Team) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software is intended to consist of voluntary contributions made by many
 * individuals for the SDSC Matrix Project.
 *
 * SDSC Matrix.  
 * Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
 *
 * Authors (Credits 02/SEP/2003):
 * Allen Ding (alding@sdsc.edu)
 * Arun Jagatheesan (arun@sdsc.edu)
 * Reena Mathew	(rmathew@sdsc.edu)
 * Erik Vandekieft (evk@sdsc.edu)
 */



package edu.sdsc.matrix.srb.operations;

/**
 * This class is responsible for storing details  
 * of :<br> 
 * - Container<br>
 * - Collection<br>
 * - Data Set<br>
 *  
 * @author rmathew
 * 
 */

public class MatrixDatagridObject
{
	/**
	 *  Grid Object Types - Container, Collection or DataSet
	 */
	private static String[] gridObjectTypes = {"Container","Collection","DataSet"};
	
	/**
	 * Constant value for Container
	 */
	public static int GRIDTYPE_CONTAINER = 0;
	
	/**
	 * Constant value for Collection
	 */
	public static int GRIDTYPE_COLLECTION = 1;
	
	/**
     * Constant value for Data Set
	 */
	public static int GRIDTYPE_DATASET = 2;
	
	/**
	 * Grid Object Type represnted by this object
	 */
	private int gridObjectType;
	
	/**
	 *  Grid Path for grid object
	 */
	private String gridpath;
	
	/**
	 * Public Constructor
	 * @param gridType Grid Type
	 * @param path Path for Grid Object
	 */
	public MatrixDatagridObject(int gridType,String path)
	{
		gridObjectType = gridType;
		gridpath = path;
		
	}
	
	/**
	 * Set Grid Object Type
	 * @param gridType Grid Type
	 */
	public void setGridObjectType(int gridType)
	{
		gridObjectType = gridType;
	}
	
	/**
	 * Return Grid Object Type
	 * @return Grid Object Type
	 */
	public int getGridObjectType()
	{
		return(gridObjectType);
	}
	
	/**
	 * Set Grid Path
	 * @param path Path for grid object
	 */
	public void setGridPath(String path)
	{
		gridpath = path;
	}
	
	/**
	 * Return Grid Path
	 * @return Path for Grid Object
	 */
	public String getGridPath()
	{
			return(gridpath);
	}
	
	/**
	 * Return grid object type
	 * @param gridObjectType Grid Object Type
	 * @return Grid Object Type Name
	 */
	public String getGridObjectType(int gridObjectType)
	{
		return(gridObjectTypes[gridObjectType]);
	}
	
	/**
	 * Check if grid object is Container
	 * @return Flag to check if grid object is container
	 */
	public boolean isContainer()
	{
		return(gridObjectType==GRIDTYPE_CONTAINER);
	}
	
	/**
	 * Check if grid object is Collection
	 * @return Flag to check if grid object is collection
	 */
	public boolean isCollection()
	{
		return(gridObjectType==GRIDTYPE_COLLECTION);
	}
	
	/**
	 * Check if grid object is Data Set
	 * @return Flag to check if grid object is data set
	 */
	public boolean isDataSet()
	{
		return(gridObjectType==GRIDTYPE_DATASET);
	}
	
	
	
}