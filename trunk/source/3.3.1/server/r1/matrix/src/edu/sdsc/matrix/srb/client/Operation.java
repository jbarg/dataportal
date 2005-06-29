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
	* Operation.java
	*
	* Created on July 8, 2003, 6:57 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.util.*;

/**
	* Part of the Matrix client API.
	* Contains methods to create Operation objects.
	* @author  Allen Ding - SDSC
	*/
public class Operation extends edu.sdsc.matrix.srb.parser.impl.OperationImpl {

		public Operation (edu.sdsc.matrix.srb.parser.Operation op)
		{
				if (op.getChangePermissionOp() != null)
		this.setChangePermissionOp(new ChangePermissionOp(op.getChangePermissionOp()));
				else if (op.getCopyOp() != null)
		this.setCopyOp(new CopyOp(op.getCopyOp()));
				else if (op.getCreateOp() != null)
		this.setCreateOp(new CreateOp(op.getCreateOp()));
				else if (op.getDeleteOp() != null)
		this.setDeleteOp(new DeleteOp(op.getDeleteOp()));
				else if (op.getDownloadDataSetOp() != null)
		this.setDownloadDataSetOp(new DownloadDataSetOp(op.getDownloadDataSetOp()));
				else if (op.getExecuteProcessOp() != null)
		this.setExecuteProcessOp(new ExecuteProcessOp(op.getExecuteProcessOp()));
				else if (op.getGetPhysicalLocationOp() != null)
		this.setGetPhysicalLocationOp(new GetPhysicalLocationOp(op.getGetPhysicalLocationOp()));
				else if (op.getIngestMetadataOp() != null)
		this.setIngestMetadataOp(op.getIngestMetadataOp());
				else if (op.getIngestOp() != null)
		this.setIngestOp(op.getIngestOp());
				else if (op.getListOp() != null)
		this.setListOp(op.getListOp());
				else if (op.getMetadataQueryOp() != null)
		this.setMetadataQueryOp(op.getMetadataQueryOp());
				else if (op.getPrepareTicketOp() != null)
		this.setPrepareTicketOp(op.getPrepareTicketOp());
				else if (op.getReDirectStatusOp() != null)
		this.setReDirectStatusOp(op.getReDirectStatusOp());
				else if (op.getRenameOp() != null)
		this.setRenameOp(op.getRenameOp());
				else if (op.getReplicateOp() != null)
		this.setReplicateOp(op.getReplicateOp());
				else if (op.getSeekNReadOp() != null)
		this.setSeekNReadOp(op.getSeekNReadOp());
				else if (op.getSeekNWriteOp() != null)
		this.setSeekNWriteOp(op.getSeekNWriteOp());
		}

		/**
		* Creates a new Operation object with a CopyOp
		*/
	public Operation(CopyOp copyOp) {
		super.setCopyOp(copyOp);
	}

	/**
		* Creates a new Operation object with a CreateOp
		*/
	public Operation(CreateOp createOp) {
		super.setCreateOp(createOp);
	}

	/**
		* Creates a new Operation object with a ChangePermissionOp
		*/
	public Operation(ChangePermissionOp changePermissionOp) {
		super.setChangePermissionOp(changePermissionOp);
	}

	/**
		* Creates a new Operation object with a DeleteOp
		*/
	public Operation(DeleteOp deleteOp) {
		super.setDeleteOp(deleteOp);
	}

	/**
		* Creates a new Operation object with a RenameOp
		*/
	public Operation(RenameOp renameOp) {
		super.setRenameOp(renameOp);
	}

	/**
		* Creates a new Operation object with a IngestOp
		*/
	public Operation(IngestDataSetOp ingestOp) {
		super.setIngestOp(ingestOp);
	}

	/**
		* Creates a new Operation object with a PrepareTicketOp
		*/
	public Operation(PrepareTicketOp ptOp) {
		super.setPrepareTicketOp(ptOp);
	}

	/**
		* Creates a new Operation object with a DownloadDataSetOp
		*/
	public Operation(DownloadDataSetOp ddsOp) {
		super.setDownloadDataSetOp(ddsOp);
	}

	/**
		* Creates a new Operation object with a ReplicateOp
		*/
	public Operation(ReplicateOp replicateOp) {
		super.setReplicateOp(replicateOp);
	}

	/**
		* Creates a new Operation object with a ListOp
		*/
	public Operation(ListOp listOp) {
		super.setListOp(listOp);
	}

	/**
		* Creates a new Operation object with a SeekNReadOp
		*/
	public Operation(SeekNReadOp snrOp) {
		super.setSeekNReadOp(snrOp);
	}

	/**
		* Creates a new Operation object with a SeekNWriteOp
		*/
	public Operation(SeekNWriteOp snwOp) {
		super.setSeekNWriteOp(snwOp);
	}

	/**
		* Creates a new Operation object with a MetadataQueryOp
		*/
	public Operation(MetadataQueryOp mdqOp) {
		super.setMetadataQueryOp(mdqOp);
	}

	public Operation(IngestMetadataOp imOp) {
		super.setIngestMetadataOp(imOp);
	}

					public Operation(GetPhysicalLocationOp op) {
							super.setGetPhysicalLocationOp(op);
					}

					public Operation(ExecuteProcessOp op) {
							super.setExecuteProcessOp(op);
					}

	protected ArrayList getAttachments() {
		//Which specific op is it that has attachments
		//find the op and return it
		if (super.getIngestOp() != null) {
			return ( (IngestDataSetOp)super.getIngestOp()).getAttachments();
		} else if (super.getSeekNWriteOp() != null) {
			return ( (SeekNWriteOp)super.getSeekNWriteOp()).getAttachments();
		} else {
			return null;
		}
	}
}
