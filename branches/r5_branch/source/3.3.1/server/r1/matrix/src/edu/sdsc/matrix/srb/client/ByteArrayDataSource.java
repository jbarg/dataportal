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
	* The Apache Software License, Version 1.1
	*
	*
	* Copyright (c) 2000 The Apache Software Foundation.  All rights
	* reserved.
	*
	* Redistribution and use in source and binary forms, with or without
	* modification, are permitted provided that the following conditions
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
	*        Apache Software Foundation (http://www.apache.org/)."
	*    Alternately, this acknowledgment may appear in the software itself,
	*    if and wherever such third-party acknowledgments normally appear.
	*
	* 4. The names "SOAP" and "Apache Software Foundation" must
	*    not be used to endorse or promote products derived from this
	*    software without prior written permission. For written
	*    permission, please contact apache@apache.org.
	*
	* 5. Products derived from this software may not be called "Apache",
	*    nor may "Apache" appear in their name, without prior written
	*    permission of the Apache Software Foundation.
	*
	* THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
	* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	* DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
	* ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
	* USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
	* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
	* SUCH DAMAGE.
	* ====================================================================
	*
	* This software consists of voluntary contributions made by many
	* individuals on behalf of the Apache Software Foundation and was
	* originally based on software copyright (c) 2000, International
	* Business Machines, Inc., http://www.apache.org.  For more
	* information on the Apache Software Foundation, please see
	* <http://www.apache.org/>.
	*/
/*
	* Modified from the original Apache SOAP project source to suit the
	* Matrix Client API needs.
	* Allen Ding
	*/
package edu.sdsc.matrix.srb.client;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import javax.activation.DataSource;
import javax.activation.FileTypeMap;

/**
	* Represents a DataSource from a stream of bytes.
	*
	* It has been modified for Matrix Client API use from the original
	* source code found in the Apache Web Services project.
	*
	* @author Wouter Cloetens (wcloeten@raleigh.ibm.com)
	*/
public class ByteArrayDataSource implements DataSource {
	private byte[] data; // data
	private String type; // content-type
	/**
		* Create a datasource from a File. If the Content-Type parameter is null,
		* the type will be derived from the filename extension.
		*
		* @param f File object
		* @param type Content-Type
		*/
	public ByteArrayDataSource(File f, String type) throws IOException {
		this(new FileInputStream(f), type);
		if (this.type == null) {
			this.type = FileTypeMap.getDefaultFileTypeMap().getContentType(f);
		}
	}

	/**
		* Create a datasource from an input stream.
		*
		* @param is InputStream
		* @param type Content-Type
		*/
	public ByteArrayDataSource(InputStream is, String type) throws IOException {
		this.type = type;
		ByteArrayOutputStream os = new ByteArrayOutputStream(4096);
		byte buf[] = new byte[4096];
		int len;
		while (true) {
			len = is.read(buf);
			if (len < 0) {
				break;
			}
			os.write(buf, 0, len);
		}
		data = os.toByteArray();
	}

	/**
		* Create a datasource from a byte array.
		*
		* @param data byte array
		* @param type Content-Type
		*/
	public ByteArrayDataSource(byte[] data, String type) {
		this.type = type;
		this.data = data;
	}

	/**
		* Create a datasource from a String. This method defaults to
		* a String encoding of iso-8859-1. For a different encoding,
		* specify a Mime "charset" in the Content-Type parameter.
		*
		* @param data byte array
		* @param type Content-Type
		*/
	public ByteArrayDataSource(String data, String type) {
		this.type = type;
		this.data = data.getBytes();
	}

	/**
		* Return an InputStream to read the content.
		*
		* @return an InputStream with the content
		*/
	public InputStream getInputStream() throws IOException {
		if (data == null) {
			throw new IOException("No data.");
		}
		return new ByteArrayInputStream(data);
	}

	/**
		* This DataSource cannot return an OutputStream, so this method is not
		* implemented.
		*/
	public OutputStream getOutputStream() throws IOException {
		throw new IOException("getOutputStream() not supported.");
	}

	/**
		* Get the content type.
		*
		* @return Content-Type string
		*/
	public String getContentType() {
		return type;
	}

	/**
		* Set the content type.
		*
		* @param type Content-Type string
		*/
	public void setContentType(String type) {
		this.type = type;
	}

	/**
		* getName() is not implemented.
		*/
	public String getName() {
		return "";
	}

	/**
		* Write the content to an OutputStream.
		*
		* @param os OutputStream to write the entire content to
		*/
	public void writeTo(OutputStream os) throws IOException {
		os.write(data);
	}

	/**
		* Return the content as a byte array.
		*
		* @return byte array with the content
		*/
	public byte[] toByteArray() {
		return data;
	}

	/**
		* Return the number of bytes in the content.
		*
		* @return size of the byte array, or -1 if not set.
		*/
	public int getSize() {
		if (data == null) {
			return -1;
		} else {
			return data.length;
		}
	}

	/**
		* Return the content as a String. The Content-Type "charset" parameter
		* will be used to determine the encoding, and if that's not available or
		* invalid, "iso-8859-1".
		*
		* @return a String with the content
		*/
	public String getText() {
		return new String(data);
	}
}