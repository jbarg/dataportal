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



/* Developer Log : UserInfo.java
	*
	* Licence, more constructors and java docs on Dec 3, 2004
	* Converted to 3.0 DGL and added setSRBGridUser() on Dec 15, 2003
	* Created on July 3, 2003, 3:59 AM
	*/

package edu.sdsc.matrix.srb.client;

/**
	* Contains methods to create UserInfo object in DGL.
	*
	* It is recommended that any non optional changes to the object be made using
	* useXXX methods that are available. A bad configuration of set fields
	* may result in an invalid content tree.
	*
	* Although optional parameters, most times doing anything requires authentication
	* so the setChallengeResponse, setHomeDirectory, and setDefaultResource methods should
	* be used in conjuntion with the UserInfo constructors.
	*
	* @author  Allen Ding, Arun swaran Jagatheesan
	*/

public class UserInfo extends edu.sdsc.matrix.srb.parser.impl.UserInfoImpl{

			/**
				* Internal method used to wrap a parser interface UserInfo object into this
						* class' representation. This method should not normally be used by the user.
				*
				* @param ui The parser.USerInfo to wrap.
				*/
			protected UserInfo(edu.sdsc.matrix.srb.parser.UserInfo ui){
						if (ui.getGridTicket() != null){
									super.setGridTicket(ui.getGridTicket());
						}
						super.setUserID(ui.getUserID());
						if (ui.getOrganization() != null){
									super.setOrganization(ui.getOrganization());
						}
						super.setChallengeResponse(ui.getChallengeResponse());
						super.setHomeDirectory(ui.getHomeDirectory());
						super.setDefaultStorageResource(ui.getDefaultStorageResource());
						super.setPhoneNumber(ui.getPhoneNumber());
						super.setEMail(ui.getEMail());
			}

			/**
				* Creates a UserInfo object for DGL using the provided parameters
				*
				* @param userID UserID used in data grid or digital library
				* @param organizationID The organization ID (e.g) SDSC
				* @param homeDirectory User's home directory
				* @param defaultStorageRes User's default starge resrouce (refer SRB documents)
				* @param password The password used int he challenge response protocol for SRB
				*/
			public UserInfo(String userID, String organizationID, String homeDirectory,
					String defaultStorageRes, String password){
						super.setUserID(userID);
						super.setOrganization(new OrganizationInfo(organizationID));
						super.setChallengeResponse(password);
						super.setHomeDirectory(homeDirectory);
						super.setDefaultStorageResource(defaultStorageRes);
			}

			/**
				* Creates a UserInfo object with the specified userID and organization.
				*
				* @param userID The user ID.
				* @param organization The user organization.
				*/
			public UserInfo(String userID, OrganizationInfo organization){
						super.setUserID(userID);
						super.setOrganization(organization);
			}

			/**
				* Creates a UserInfo object with the specified userID and organization.
				*
				* <i> Warning: </i>
				* Although optional parameters, most times doing anything requires authentication
				* so the setChallengeResponse, setHomeDirectory, and setDefaultResource methods should
				* be used in conjuntion with the UserInfo constructors.
				* @param userID The user ID.
				* @param organization The user organization.
				*/
			public UserInfo(String userID, String organizationName){
						super.setUserID(userID);
						super.setOrganization(new OrganizationInfo(organizationName));
			}

			public UserInfo(String ticketID){
						this.useGridTicket(new GridTicketInfo(ticketID));
			}

			/**
				* Creates a UserInfo object with the specified gridTicketInfo.
				*
				* <i> Warning: </i>
				* Although optional parameters, most times doing anything requires authentication
				* so the setChallengeResponse, setHomeDirectory, and setDefaultResource methods should
				* be used in conjuntion with the UserInfo constructors.
				* @param gridTicketInfo The ticket to use.
				*/
			public UserInfo(GridTicketInfo gridTicketInfo){
						super.setGridTicket(gridTicketInfo);
			}

			/**
				* Makes the UserInfo use the specified gridTicketInfo. Using this method
				* clobbers any userID and organization information.
				* <i> Warning: </i>
				* Although optional parameters, most times doing anything requires authentication
				* so the setChallengeResponse, setHomeDirectory, and setDefaultResource methods should
				* be used in conjuntion with the UserInfo constructors.
				* @param gridTicketInfo The ticket to use.
				*/
			public void useGridTicket(GridTicketInfo gridTicketInfo){
						super.setUserID(null);
						super.setOrganization(null);
						super.setGridTicket(gridTicketInfo);
			}

			/**
				* Makes the UserInfo use the specified userID and organization. Using
				* this method clobbers any GridTicket information.
				*
				* <i> Warning: </i>
				* Although optional parameters, most times doing anything requires authentication
				* so the setChallengeResponse, setHomeDirectory, and setDefaultResource methods should
				* be used in conjuntion with the UserInfo constructors.
				* @param userID The user ID.
				* @param organization The user organization.
				*/
			public void useUserIDAndOrganizationInfo(String userID,
					OrganizationInfo organization){
						super.setGridTicket(null);
						super.setUserID(userID);
						super.setOrganization(organization);
			}

			public void setSRBGridUser(String userID, OrganizationInfo org, String pass,
					String homeDir, String defaultResource){
						this.useUserIDAndOrganizationInfo(userID, org);
						super.setChallengeResponse(pass);
						super.setHomeDirectory(homeDir);
						super.setDefaultStorageResource(defaultResource);
			}
}