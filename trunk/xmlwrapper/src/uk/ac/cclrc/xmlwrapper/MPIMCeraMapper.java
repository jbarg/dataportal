package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;

import java.sql.* ; //jdbc

import org.apache.log4j.* ;

public class MPIMCeraMapper implements CeraMapper
{
   public String getKeys(String query)
   {
      //need some special case code in here at the moment for the ALL scenario

      //get values needed for session ;
      
      SessionSingleton ss = SessionSingleton.getInstance() ;

      Logger log = ss.getLogger() ;

      log.debug("where:\t" + query) ;

      Statement s = ss.getStatement() ;
      ResultSet r = ss.getResultSet() ;

      DBHelper dbh = ss.getDBHelper() ;

      ParseQuery pq = ss.getParseQuery() ;

      String dbs = ss.getDbs() ;

      // now fill in the search criteria
      pq.populateConditionalCriteria(query) ;

      //list of Criteria to find:

      StringBuffer sql = new StringBuffer() ;

      String discipline = pq.getFromSearchList("Discipline") ;

      if(discipline.toLowerCase().compareTo("all") != 0 )
      { //if not in test mode then carry on gettting the correct entries

         String subject = pq.getFromSearchList("Subject") ;
         String studyname = pq.getFromSearchList("StudyName")  ;
         String studyid = pq.getFromSearchList("StudyID") ;
         //String dminstitution = pq.getFromSearchList("DMInstitutionName") ; //DMInstitutionName
         String dminstitution = "" ; //as the data archive does not contain a reference to BADC
	                             //this is stopping valid records from being returned 
         String startdate = pq.getFromSearchList("StartDate") ; // Date format YYYYMMDD
         String enddate = pq.getFromSearchList("EndDate") ;
	 String temp_str = null ;

	 if(discipline.compareTo("") != 0)
         {
            // for some reason jdbc did not like  brackets around the following - strange.
            temp_str = ("SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE ENTRY_ID IN " +
                        "(SELECT ENTRY_ID FROM " + dbs + "PARAMETER WHERE TOPIC_ID IN " +
                        "(SELECT TOPIC_ID FROM " + dbs + "TOPIC WHERE lower(TOPIC_NAME) LIKE '%" + discipline + "%'))") ;
	    discipline = temp_str ;
	 }

	 if(subject.compareTo("") != 0)
         {
            temp_str = ("SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE ENTRY_ID IN " +
                        "(SELECT ENTRY_ID FROM " + dbs + "LOCATION_CONNECT WHERE LOCATION_ID IN " +
                        "(SELECT LOCATION_ID FROM " + dbs + "LOCATION WHERE lower(LOCATION_NAME) LIKE '%" + subject + "%'))") ;
	    subject = temp_str ;
	 }

	 if(studyname.compareTo("") != 0)
         {
            //could we need 2 different strings one holding enclosing brackets and one without
	    //i.e. former when other params (don't there have to be) and other when there are not
	    //otherwise who know may give a 0ra-01009 missing parameter like it did with discipline
	    
	    //needed for checking on disciplines with out having to have a discipline selected
	    StringTokenizer st = new StringTokenizer(studyname,".") ;
            String dis_cp = "" ;
            if(st.hasMoreTokens())
	    {
	       st.nextToken() ;
	    }
	    if(st.hasMoreTokens())
	    {
	       dis_cp = st.nextToken() ;
	    }
            
	    temp_str = ("(SELECT ENTRY_ID FROM " + dbs + "ENTRY " +
		        "WHERE lower(ENTRY_NAME) LIKE '%" + studyname + "%' " +
			 "UNION " +
                         "SELECT ENTRY_ID FROM " + dbs + "CAMPAIGN WHERE PROJECT_ID IN " +
                          "(SELECT PROJECT_ID FROM " + dbs + "PROJECT WHERE lower(PROJECT_NAME) LIKE '%" + studyname + "%') " +
			")") ;

	    studyname = temp_str ;
	 }

	 if(studyid.compareTo("") != 0)
         {
            // as above note - should this have 2 versions
            temp_str = ("(SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE lower(ENTRY_ACRONYM) LIKE '%" + studyid + "%' " +
			 "UNION " +
                         "SELECT ENTRY_ID FROM " + dbs + "CAMPAIGN WHERE PROJECT_ID IN " +
                          "(SELECT PROJECT_ID FROM " + dbs + "PROJECT WHERE lower(PROJECT_ACRONYM) LIKE '%" + studyid + "%') " +
			")") ;

	    studyid = temp_str ;
	 }

	 if(dminstitution.compareTo("") != 0)
         {
            temp_str = ("SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE ENTRY_ID IN " +
                        "(SELECT ENTRY_ID FROM " + dbs + "CONTACT WHERE INSTITUTE_ID IN " +
                       "(SELECT INSTITUTE_ID FROM " + dbs + "INSTITUTE WHERE lower(INSTITUTE_NAME) LIKE '%" + dminstitution + "%'))") ;
	    dminstitution = temp_str ;
	 }

	 //remove the day element from the startdate and enddate as this date is not in
	 //temporal coverage in the database i.e. the YYYYMM portion of the YYYYMMDD date format

	 if(startdate.compareTo("") != 0)
         {
            temp_str = ("SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE ENTRY_ID IN " +
                        "(SELECT ENTRY_ID FROM " + dbs + "COVERAGE WHERE TEMPORAL_COVERAGE_ID IN " +
                        "(SELECT TEMPORAL_COVERAGE_ID FROM " + dbs + "TEMPORAL_COVERAGE " +
			"WHERE (START_YEAR||lpad(START_MONTH,2,0)||START_DAY) LIKE '%" + startdate.substring(0,6) + "%'))") ;
	    startdate = temp_str ;
	 }

	 if(enddate.compareTo("") != 0)
         {
            temp_str = ("SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE ENTRY_ID IN " +
                        "(SELECT ENTRY_ID FROM " + dbs + "COVERAGE WHERE TEMPORAL_COVERAGE_ID IN " +
                        "(SELECT TEMPORAL_COVERAGE_ID FROM " + dbs + "TEMPORAL_COVERAGE " +
			"WHERE (STOP_YEAR||lpad(STOP_MONTH,2,0)||STOP_DAY) LIKE '%" + enddate.substring(0,6) + "%'))") ;
	    enddate = temp_str ;
	 }

	 String jc = " INTERSECT " ; //items AND'ed together would be UNION if they were OR'ed
	                             //jc - join condition

	 boolean first_cond = false ;

	 if(discipline.compareTo("") != 0)
         {
            first_cond = true ;
	    sql.append(discipline) ;
	 }

	 if(subject.compareTo("") != 0)
         {
            if(first_cond == true)
            {
               sql.append(jc) ;
	       sql.append(subject) ;
	    }
	    else
            {
               first_cond = true ;
               sql.append(subject) ;
	    }
         }
         
	 if(studyname.compareTo("") != 0)
         {
            if(first_cond == true)
            {
               sql.append(jc) ;
	       sql.append(studyname) ;
	    }
	    else
            {
               first_cond = true ;
               sql.append(studyname.substring(1,studyname.length()-1)) ;
	    }
         }

	 if(studyid.compareTo("") != 0)
	 {
	    if(first_cond == true)
	    {
	       sql.append(jc) ;
	       sql.append(studyid) ;
	    }
	    else
	    {
	       first_cond = true ;
	       //remove the enclosing braces if this is the only/first select item
	       sql.append(studyid.substring(1,studyid.length()-1)) ;
	    }
	 }

	 if(dminstitution.compareTo("") != 0)
         {
            if(first_cond == true)
            {
               sql.append(jc) ;
	       sql.append(dminstitution) ;
	    }
	    else
            {
               first_cond = true ;
               sql.append(dminstitution) ;
	    }
         }

	 if(startdate.compareTo("") != 0)
         {
            if(first_cond == true)
            {
               sql.append(jc) ;
	       sql.append(startdate) ;
	    }
	    else
            {
               first_cond = true ;
               sql.append(startdate) ;
	    }
         }

	 if(enddate.compareTo("") != 0)
         {
            if(first_cond == true)
            {
               sql.append(jc) ;
	       sql.append(enddate) ;
	    }
	    else
            {
               first_cond = true ;
               sql.append(enddate) ;
	    }
         }
      }
      else
      {
         sql.append("SELECT ENTRY_ID FROM " + dbs + "ENTRY") ;
         //sql.append("SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE ENTRY_ID IN ('20144')") ;
      }

      //remove the entries that we do not need - i.e. the ones refering to Grib messages
      //now we only want entries whic are complete Studys back up the hierarchy I guess

      sql.append(" minus select entry_id from " + dbs + "entry where entry_type_id in " +
                 " (select entry_type_id from " + dbs + "entry_type where entry_type not like '%experiment%')") ;

      //now for the actual selection of values and building up the comma
      //space separated list of valid entry_id's

      // now apply the sql to get the qualifying entries	 

      boolean entries_extracted = false ;

      StringBuffer sb = new StringBuffer() ;

      while(entries_extracted == false)
      {

         if(sb.length() != 0 )
         {
            sb.delete(0,sb.length()) ;
         }

         try
         {
            log.debug("Here is the SQL:\n" + sql.toString()) ;
		 
            r = s.executeQuery(sql.toString()) ;

            while(r.next())
            {
               r.getRow() ;
               sb.append(r.getInt("ENTRY_ID")) ;
               sb.append(", ") ;
            }

            r.close() ;
 
            log.debug("contents upto now:\t" + sb.toString()) ;

            entries_extracted = true ;
         }
         catch (SQLException se)
         {
            se.printStackTrace() ;
            entries_extracted = false ;
         }

         if(entries_extracted == false)
         {
            dbh.connectToDB() ;
         }

      } // end of while 

      //
      //now do the same again but for datasets and then find the parent experiments of these
      //datasets and add them to the list of valid experiments returned containing these topics
      //

      sql.delete(sql.length()-15, sql.length()) ;
      sql.append("'%dataset%')") ;

      StringBuffer sbex = new StringBuffer() ;
      StringBuffer sqlec = new StringBuffer() ;

      entries_extracted = false ;

      while(entries_extracted == false)
      {

         if(sbex.length() != 0 )
         {
            sbex.delete(0,sbex.length()) ;
         }
	 if(sqlec.length() != 0)
	 {
	    sqlec.delete(0, sqlec.length()) ;
	 }

         try
         {

	    sqlec.append("SELECT DISTINCT GEN_ENTRY_ID FROM ") ;
	    sqlec.append(dbs) ;
	    sqlec.append("ENTRY_CONNECT WHERE SPEC_ENTRY_ID IN (") ;
	    sqlec.append(sql.toString()) ;
	    sqlec.append(")") ;
		 
            log.debug("Here is the SQL:\n" + sql.toString()) ;

            r = s.executeQuery(sqlec.toString()) ;

            while(r.next())
            {
               r.getRow() ;
               sbex.append(r.getInt("GEN_ENTRY_ID")) ;
               sbex.append(", ") ;
            }

            r.close() ;
 
            entries_extracted = true ;
         }
         catch (SQLException se)
         {
            se.printStackTrace() ;
            entries_extracted = false ;
         }

         if(entries_extracted == false)
         {
            dbh.connectToDB() ;
         }

      } // end of while

      //
      //find all experiments from qualifying datasets
      //
      
      //append the two strings
      sb.append(sbex.toString()) ;
            
      log.debug("contents:\t" + sb.toString()) ;

      return sb.toString() ;
    
   }

   //
   //
   //
   //

   public void buildMetadataRecord(String key, StringBuffer sbr) throws SQLException
   {
      //avoid accumulation of results due to successive calls to this function
      if(sbr.length() != 0 )
      {
         sbr.delete(0,sbr.length()) ;
      }

      //dhl - dataholding location
      //some local ones are needed otherwise it gets recursive
      StringBuffer sbr_dhl = new StringBuffer() ;
      StringBuffer sbr_dhl_files = new StringBuffer() ;

      //get local values needed from SessionSingleton
      SessionSingleton ss = SessionSingleton.getInstance() ;

      StringReplace sr=ss.getStringReplace() ;
      XmlText xt=ss.getXmlText() ;

      Logger log = ss.getLogger() ;

      Statement s = ss.getStatement() ;
      ResultSet r = ss.getResultSet() ;

      String place_holder = ss.place_holder ;
      int i_place_holder = ss.i_place_holder ;
      int indent = ss.indent ;

      //retrieve the database name
      String dbs = ss.getDbs() ;
      
      //using an indent of 3

      //Entry specific processing will be done based on what type of entry this is
      //therefore the entry_type is extracted now
      
      String entry_type = place_holder ;
      
      r = s.executeQuery("select entry_type from " + dbs + "entry_type where entry_type_id in " +
	                 "(select entry_type_id from " + dbs + "entry where key = '" + key + "')") ;
      if(r.next())
      {
	 r.getRow() ;
	 entry_type = sr.LitWithEnt(xt.makeValid(r.getString("ENTRY_TYPE") ));
      }

      String entry_acronym = place_holder ;

      r = s.executeQuery("select entry_acronym from " + dbs + "entry where key = '" + key + "'") ;
      if(r.next())
      {
	 r.getRow() ;
	 entry_acronym = sr.LitWithEnt(xt.makeValid(r.getString("ENTRY_ACRONYM") ));
      }

      //sbr.append("   <MetadataRecord metadataID=\"badc" + key + "\">\n");
      sbr.append("   <MetadataRecord metadataID=\"" + entry_acronym + "\">\n");

      r.close() ;

      // lookup Topic

      sbr.append("      <Topic>\n") ;

      r = s.executeQuery("select topic_name from " + dbs + "topic where topic_id in" +
                        "(select distinct topic_id  from " + dbs + "parameter where entry_id='" + key + "')" ) ;
     
      // I hate strings - but should one make this more verbose and messy ?
      // as this won't go out of scope (eligible for garbage collection) and then we can map multiple
      // references to this one instance

      String topic_name = place_holder ;

      if(r.next())
      {
         r.getRow() ;
         topic_name = sr.LitWithEnt(xt.makeValid(r.getString("TOPIC_NAME") ));
      }

      sbr.append("         <Discipline>" + topic_name + "</Discipline>\n") ;
     

      // following should the resultset handle
      r.close() ;

      r = s.executeQuery("SELECT GENERAL_KEY FROM " + dbs + "GENERAL_KEY WHERE GENERAL_KEY_ID IN " +
                        "(SELECT DISTINCT GENERAL_KEY_ID FROM " + dbs + "KEY_CONNECT WHERE " +
                        " ENTRY_ID='" + key + "' )") ;

      //as there can be multiple ones (note sure about in CERA though)

      String subject = place_holder ;

      while(r.next())
      {
         r.getRow() ;
         subject = sr.LitWithEnt(xt.makeValid(r.getString("GENERAL_KEY") ));
         sbr.append("         <Subject>" + sr.LitWithEnt(xt.makeValid(r.getString("GENERAL_KEY"))) + "</Subject>\n") ;
      }

      // duplication needed as there maybe multiple values
      if(subject.compareTo(place_holder) == 0 )
      {
         sbr.append("         <Subject>" + place_holder + "</Subject>\n") ;
      }

      r.close() ;

      sbr.append("      </Topic>\n") ;

      // end of Topic

      // start of measurement - now Experiment

      // needed here as entry_names are a bit funny

      r = s.executeQuery("SELECT PROJECT_NAME FROM " + dbs + "PROJECT WHERE PROJECT_ID "+
                       " IN ( SELECT PROJECT_ID FROM " + dbs + "CAMPAIGN WHERE ENTRY_ID='" + key + "' )") ;

      String project_name = place_holder ;

      if(r.next())
      {
         project_name = sr.LitWithEnt(xt.makeValid(r.getString("PROJECT_NAME") ));
      }
 
      r.close() ;

      sbr.append("      <Experiment>\n") ;

      r = s.executeQuery("SELECT ENTRY_NAME, CREATION_DATE FROM " + dbs + "ENTRY WHERE ENTRY_ID='" + key + "'") ;

      String entry_name = place_holder ;
      String creation_date = place_holder ;

      if(r.next())
      {
         r.getRow() ;
         entry_name = sr.LitWithEnt(xt.makeValid(r.getString("ENTRY_NAME") ));
         creation_date = r.getTimestamp("CREATION_DATE").toString() ;
      }

      // - no need for this after re-load on 10/01/02 entry_name = project_name + key ;
      // - i.e strange characters removed
      

      r.close() ;

      // as need to capture the result of this select before we can do another query
      // maybe we coud use two statement handles but this should do the job

      r.close() ;

      r = s.executeQuery("SELECT STREET, PLACE, POBOX_POSTAL_CODE, COUNTRY, " +
                       "INSTITUTE_NAME, INSTITUTE_ID FROM " + dbs + "INSTITUTE WHERE INSTITUTE_ID IN " +
                       "(SELECT INSTITUTE_ID FROM " + dbs + "CONTACT WHERE ENTRY_ID='" + key + "'" +
                           " AND CONTACT_TYPE_ID IN ( SELECT CONTACT_TYPE_ID FROM " + dbs + "CONTACT_TYPE " +
                           " WHERE CONTACT_TYPE like '%inhouse%' or CONTACT_TYPE like '%investigator%' " +
		           "or CONTACT_TYPE like '%originator%') )" ) ;

      int institute_id = i_place_holder ;
      String institute_name = place_holder ;
      String i_street = place_holder ;
      String i_place = place_holder ;
      String i_pobox_postal_code = place_holder ;
      String i_country = place_holder ;

      if(r.next())
      {
         r.getRow() ;   
         institute_name = sr.LitWithEnt(xt.makeValid(r.getString("INSTITUTE_NAME") ));
         institute_id = r.getInt("INSTITUTE_ID") ;
         i_street = sr.LitWithEnt(xt.makeValid(r.getString("STREET") ));
         i_place = sr.LitWithEnt(xt.makeValid(r.getString("PLACE") ));
         i_pobox_postal_code = sr.LitWithEnt(xt.makeValid(r.getString("POBOX_POSTAL_CODE") ));
         i_country = sr.LitWithEnt(xt.makeValid(r.getString("COUNTRY") ));
      }

      //sbr.append("         <StudyName>" + entry_name + "." + topic_name + ".cera" + key + "</StudyName>\n" ) ;
      
      sbr.append("         <StudyName>" + entry_name + "</StudyName>\n" ) ;
     

      if(project_name.compareTo(place_holder) != 0 )
      {
         if(institute_name.compareTo(place_holder) != 0)
         {
            sbr.append("         <StudyID studyid=\"" + entry_acronym + "\" institutionref=\"I" + institute_id + "\"/>\n") ;
         }
         else
         {
            sbr.append("         <StudyID studyid=\"" + entry_acronym + "\"/>\n") ;
         }
      }
      else
      {
         sbr.append("         <StudyID studyid=\"" + place_holder + "\" institutionref=\"" + place_holder + "\"/>\n") ;
      }

      r.close() ;

      //Investigator

      sbr.append("         <Investigator>\n") ;

      //will look for the investigator
      r = s.executeQuery("SELECT TITLE, FIRST_NAME, SECOND_NAME, LAST_NAME, " +
                       "TELEPHONE, EMAIL, FAX FROM " + dbs + "PERSON WHERE" +
                       " PERSON_ID IN ( SELECT PERSON_ID FROM " + dbs + "CONTACT WHERE ENTRY_ID='" + key + "'" +
                       " AND CONTACT_TYPE_ID IN (SELECT CONTACT_TYPE_ID FROM " + dbs + "CONTACT_TYPE WHERE " +
                       "CONTACT_TYPE like '%inhouse%' or CONTACT_TYPE like '%investigator%' " +
		       "or CONTACT_TYPE like '%originator%') )") ;

      //The investigator could be one of: investigator, inhouse or originator
      //The data manager could be one of: distributor or owner

      String last_name = place_holder ;
      String first_name = place_holder ;
      String second_name = place_holder ;
      String title = place_holder ;
      String telephone = place_holder ;
      String fax = place_holder ;
      String email = place_holder ;

      // read the values in and then pop the xml

      if(r.next())
      {
         r.getRow() ;

         last_name = sr.LitWithEnt(xt.makeValid(r.getString("LAST_NAME") ));
         first_name = sr.LitWithEnt(xt.makeValid(r.getString("FIRST_NAME") ));
         second_name = sr.LitWithEnt(xt.makeValid(r.getString("SECOND_NAME") ));
         title = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));
         telephone = sr.LitWithEnt(xt.makeValid(r.getString("TELEPHONE") ));
         fax = sr.LitWithEnt(xt.makeValid(r.getString("FAX") ));
         email = sr.LitWithEnt(xt.makeValid(r.getString("EMAIL") ));
      }

      sbr.append("            <Name>\n") ;
      sbr.append("               <Surname>" + last_name + "</Surname>\n") ;
      sbr.append("               <Initials>" + first_name + " " + second_name +
                                  "</Initials>\n") ;
      sbr.append("             </Name>\n") ;
      //perhaps not - sbr.append("             <Status>" + title + "</Status>\n") ;
      sbr.append("             <Status>" + "inhouse" + "</Status>\n") ;
      sbr.append("             <Institution>" + institute_name + "</Institution>\n") ;

      sbr.append("             <ContactDetails>\n") ;
      sbr.append("                <Address>\n") ;
      sbr.append("                   <Addressline1>" + i_street + "</Addressline1>\n") ;
      sbr.append("                   <Town>" + i_place + "</Town>\n") ;
      sbr.append("                   <Country>" + i_country + "</Country>\n") ;
      sbr.append("                </Address>\n") ; 
      sbr.append("                <Telephone>" + telephone + "</Telephone>\n") ;
      sbr.append("                <Email>" + email + "</Email>\n") ;
      sbr.append("                <Fax>" + fax + "</Fax>\n") ;
      sbr.append("             </ContactDetails>\n") ;
       
      r.close() ;

      // The Role
      r = s.executeQuery("SELECT CONTACT_TYPE FROM " + dbs + "CONTACT_TYPE WHERE CONTACT_TYPE_ID IN " +
                        "(SELECT CONTACT_TYPE_ID FROM " + dbs + "CONTACT WHERE ENTRY_ID='" + key + "')" ) ;
  
      String contact_type = place_holder ;

      if(r.next())
      {
         r.getRow() ;
         contact_type = sr.LitWithEnt(xt.makeValid(r.getString("CONTACT_TYPE") ));
      }

      sbr.append("             <Role>" + contact_type + "</Role>\n") ;

      r.close() ;

      sbr.append("          </Investigator>\n") ;

      // onto the StudyInformation
      // may have to fill a section in with place holders

      sbr.append("          <StudyInformation>\n") ;
      sbr.append("             <Funding>" + place_holder + "</Funding>\n") ;
      sbr.append("             <TimePeriod>\n") ;
      sbr.append("                <StartDate>\n") ;
      sbr.append("                   <Date>"+ creation_date + "</Date>\n") ;
      sbr.append("                </StartDate>\n") ;
      sbr.append("             </TimePeriod>\n") ;
      sbr.append("             <Purpose>\n") ;
      
      String summary = place_holder ;
      r = s.executeQuery("SELECT SUMMARY FROM " + dbs + "SUMMARY WHERE SUMMARY_ID IN " +
	                 "(SELECT SUMMARY_ID FROM " + dbs + "ENTRY WHERE ENTRY_ID='" + key + "')" ) ;
      if(r.next()) 
      {
	 r.getRow() ;
	 summary = sr.LitWithEnt(xt.makeValid(r.getString("SUMMARY") ));
      }
      r.close() ;
      
      sbr.append("                <Abstract>" + summary + "</Abstract>\n") ;
      
      sbr.append("             </Purpose>\n") ;
      sbr.append("             <StudyStatus>" + place_holder + "</StudyStatus>\n") ;
      sbr.append("             <Resources>"+ place_holder + "</Resources>\n") ;
      sbr.append("          </StudyInformation>\n") ;

      // where the CONTACT_TYPE is "inhouse" this is the researcher
      // where the CONTACT_TYPE is "distributor" this is the data manager effectively

      //DataManager 
 
      //will look for the distributor i.e. datamanager
      r = s.executeQuery("SELECT TITLE, FIRST_NAME, SECOND_NAME, LAST_NAME, " +
                         "TELEPHONE, EMAIL, FAX FROM " + dbs + "PERSON WHERE" +
                         " PERSON_ID IN ( SELECT PERSON_ID FROM " + dbs + "CONTACT WHERE ENTRY_ID='" + key + "'" +
                         " AND CONTACT_TYPE_ID IN (SELECT CONTACT_TYPE_ID FROM " + dbs + "CONTACT_TYPE WHERE " +
                         "CONTACT_TYPE like '%distributor%' or CONTACT_TYPE like '%owner%') )") ;

      last_name = place_holder ;
      first_name = place_holder ;
      second_name = place_holder ;
      title = place_holder ;
      telephone = place_holder ;
      fax = place_holder ;
      email = place_holder ;

      // need to read the values in and then pop the xml

      if(r.next())
      {
         r.getRow() ;

         last_name = sr.LitWithEnt(xt.makeValid(r.getString("LAST_NAME") ));
         first_name = sr.LitWithEnt(xt.makeValid(r.getString("FIRST_NAME") ));
         second_name = sr.LitWithEnt(xt.makeValid(r.getString("SECOND_NAME") ));
         title = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));
         telephone = sr.LitWithEnt(xt.makeValid(r.getString("TELEPHONE") ));
         fax = sr.LitWithEnt(xt.makeValid(r.getString("FAX") ));
         email = sr.LitWithEnt(xt.makeValid(r.getString("EMAIL") ));
      }

      r.close() ;

      sbr.append("          <DataManager>\n") ;
      sbr.append("             <Institution>" + place_holder + "</Institution>\n") ; // need to lookup
                                                                                    // via contact type
      sbr.append("             <Contact>\n") ;
      sbr.append("                <Name>\n") ;
      sbr.append("                   <Surname>" +  last_name + "</Surname>\n") ;
      sbr.append("                   <Initials>" + first_name + " " + second_name + "</Initials>\n") ;
      sbr.append("                </Name>\n") ;
      sbr.append("                <Status>" + "data distributor" + "</Status>\n") ;
      // following seems like a repeat - perhaps used elsewhere
      sbr.append("                <Institution>" + place_holder + "</Institution>\n") ; 
    
    
      r = s.executeQuery("SELECT STREET, PLACE, POBOX_POSTAL_CODE, COUNTRY, " +
                         "INSTITUTE_NAME, INSTITUTE_ID FROM " + dbs + "INSTITUTE WHERE INSTITUTE_ID IN " +
                         "(SELECT INSTITUTE_ID FROM " + dbs + "CONTACT WHERE ENTRY_ID='" + key + "'" +
                           " AND CONTACT_TYPE_ID IN ( SELECT CONTACT_TYPE_ID FROM " + dbs + "CONTACT_TYPE " +
                           " WHERE CONTACT_TYPE like '%distributor%' or CONTACT_TYPE like '%owner%') )" ) ;

      institute_id = i_place_holder ;
      institute_name = place_holder ;
      i_street = place_holder ;
      i_place = place_holder ;
      i_pobox_postal_code = place_holder ;
      i_country = place_holder ;

      if(r.next())
      {
         r.getRow() ;   
         institute_name = sr.LitWithEnt(xt.makeValid(r.getString("INSTITUTE_NAME") ));
         institute_id = r.getInt("INSTITUTE_ID") ;
         i_street = sr.LitWithEnt(xt.makeValid(r.getString("STREET") ));
         i_place = sr.LitWithEnt(xt.makeValid(r.getString("PLACE") ));
         i_pobox_postal_code = sr.LitWithEnt(xt.makeValid(r.getString("POBOX_POSTAL_CODE") ));
         i_country = sr.LitWithEnt(xt.makeValid(r.getString("COUNTRY") ));
      }

      r.close() ;
     
      sbr.append("                <ContactDetails>\n") ;
      sbr.append("                   <Address>\n") ;
      sbr.append("                      <Addressline1>" + i_street + "</Addressline1>\n") ;
      sbr.append("                      <Town>" + i_place + "</Town>\n") ;
      sbr.append("                      <Country>" + i_country + "</Country>\n") ;
      sbr.append("                   </Address>\n") ;
      sbr.append("                   <Telephone>" + telephone + "</Telephone>\n") ;
      sbr.append("                </ContactDetails>\n") ;
      sbr.append("             </Contact>\n") ;
      sbr.append("          </DataManager>\n") ;

      //Instrument

      sbr.append("          <Instrument>" + place_holder + "</Instrument>\n") ;

      appendConditionsForEntry(key, 9, sbr) ; // 9 = 3 spaces per level - and 3 level 3x3

 
      // end of measurement now measurement
      sbr.append("       </Experiment>\n") ;

      String access_cond = place_holder ;

      r = s.executeQuery("SELECT CONSTRAINT_DESCR FROM " + dbs + " ACCESS_CONSTRAINT WHERE ACCESS_CONSTRAINT_ID IN " +
                           "(SELECT ACCESS_CONSTRAINT_ID FROM " + dbs + "DISTRIBUTION WHERE ENTRY_ID='" + key + "')") ;

      if(r.next())
      {
         r.getRow() ;
         access_cond = sr.LitWithEnt(xt.makeValid(r.getString("CONSTRAINT_DESCR") ));
      }

      sbr.append("       <AccessConditions>" + access_cond + "</AccessConditions>\n") ;

      //DataHolding

      //Each experiment for this Study will be the name of a Data Holding
      // really need a loop for this in case there are more that one experiment - save re-implementation
      // later

      List experiments = new LinkedList() ;
      List datasets = new LinkedList() ;
      List datafiles = new LinkedList() ;
      
      //List rascollections = new LinkedList() ;

      //r = s.executeQuery("SELECT DISTINCT SPEC_ENTRY_ID FROM " + dbs + "ENTRY_CONNECT WHERE GEN_ENTRY_ID = '" + key +
      //                 "' INTERSECT SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE ENTRY_TYPE_ID IN " +
      //		 "(SELECT ENTRY_TYPE_ID FROM " + dbs + "ENTRY_TYPE WHERE ENTRY_TYPE LIKE '%EXPERIMENT%')") ;

      //as the entry is the experiment we will just add the key to the following list of experiment

      //while(r.next())
      //{
      //   r.getRow() ;
      //   experiments.add(Integer.toString(r.getInt("SPEC_ENTRY_ID"))) ;
      //}
        
      experiments.add(key) ;

      //r.close() ;

      //and now for each entry

      Iterator e = experiments.iterator() ;

      String exp_id = place_holder ;
      
      while(e.hasNext())
      {
	 exp_id = (String) e.next() ;

	 String dataid = place_holder ;
	 String dataname = place_holder ;
	 String typeofdata = "EXPERIMENT" ; //could replace with a search

	 r = s.executeQuery("SELECT ENTRY_NAME, ENTRY_ACRONYM FROM " + dbs + "ENTRY WHERE ENTRY_ID = '" + exp_id + "'") ;
	 if(r.next())
	 {
	    r.getRow() ;
	    dataid = sr.LitWithEnt(xt.makeValid(r.getString("ENTRY_ACRONYM") ));
	    dataname = sr.LitWithEnt(xt.makeValid(r.getString("ENTRY_NAME") ));
	 }
	 r.close() ;
	 
         sbr.append("       <DataHolding dataid=\"" + dataid + "\">\n") ;
	 sbr.append("          <DataName>" + dataname + "</DataName>\n") ;
	 sbr.append("          <TypeOfData>" + typeofdata + "</TypeOfData>\n") ;

	 // the following is a candidate for functionisation

	 r = s.executeQuery("SELECT PROGRESS_DESCR FROM " + dbs + "PROGRESS WHERE PROGRESS_ID IN " +
                           "(SELECT PROGRESS_ID FROM " + dbs + "ENTRY WHERE ENTRY_ID='" + exp_id + "')") ;

         String progress = place_holder;
	 
         if(r.next())
         {
            r.getRow() ;
            progress = sr.LitWithEnt(xt.makeValid(r.getString("PROGRESS_DESCR") ));
         }
         r.close() ;

	 sbr.append("          <Status>" + progress + "</Status>\n") ;

	 // fill in the dataholdinglocation details - save uneccessary calls later
	 //////

	 sbr_dhl.append("      <DataHoldingLocator dataidref=\"" + dataid + "\">\n") ;
         sbr_dhl.append("         <DataName>" + dataname + "</DataName>\n") ;
	 sbr_dhl.append("         <Locator type=\"absolute\">\n") ;
         sbr_dhl.append("            <URL>" + getStoragePath(exp_id, 1) + "</URL>\n") ;

         String data_access_cond = place_holder ;
         r = s.executeQuery("SELECT CONSTRAINT_DESCR FROM " + dbs + " ACCESS_CONSTRAINT WHERE ACCESS_CONSTRAINT_ID IN " +
                           "(SELECT ACCESS_CONSTRAINT_ID FROM " + dbs + "DISTRIBUTION WHERE ENTRY_ID='" + exp_id + "')") ;
         if(r.next())
         {
            r.getRow() ;
            data_access_cond = sr.LitWithEnt(xt.makeValid(r.getString("CONSTRAINT_DESCR") ));
         }
         sbr_dhl.append("            <DataSourceAccess>" + data_access_cond + "</DataSourceAccess>\n") ;
	 sbr_dhl.append("         </Locator>\n") ;
	 
         //////
	 // now select the datasets associated with this experiment

	 log.debug("ENTRY_ID at this point is: " + exp_id) ;

         r = s.executeQuery("SELECT DISTINCT SPEC_ENTRY_ID FROM " + dbs + "ENTRY_CONNECT WHERE GEN_ENTRY_ID = '" + exp_id +
	                    "' INTERSECT SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE ENTRY_TYPE_ID IN " +
	  		    "(SELECT ENTRY_TYPE_ID FROM " + dbs + "ENTRY_TYPE WHERE ENTRY_TYPE LIKE '%dataset%')") ;

         while(r.next())
         {
	    r.getRow() ;
	    datasets.add(Integer.toString(r.getInt("SPEC_ENTRY_ID"))) ;
         }
         r.close() ;

	 //now will in the rascollection list for this experiment
	 //perhaps this should be done on a per dataset basis (depending on the insertion method) 
	 //but here we use the experiment/dataholding as the unit for which these things are selected
	 // now below 

         //and now for each entry
         Iterator g = datasets.iterator() ;
         String dataset_key = place_holder ;
      
         while(g.hasNext())
         {
	    dataset_key = (String) g.next() ;

	    // fill in the ras_collection from this list
            //r = s.executeQuery("SELECT DISTINCT RAS_COLLECTION FROM " + dbs + "RAS_CONNECT WHERE ENTRY_ID = '" 
	    //                    + dataset_key + "'") ;
	    //while(r.next())
	    //{
	    //   r.getRow() ;
	    //   rascollections.add(sr.LitWithEnt(xt.makeValid(r.getString("RAS_COLLECTION")) ));
            //}
	    //r.close() ;
	    //

	    dataid = place_holder ;
	    dataname = place_holder ;
	    typeofdata = "DATA_SET" ; //could replace with a search

	    r = s.executeQuery("SELECT ENTRY_NAME, ENTRY_ACRONYM FROM " + dbs + "ENTRY WHERE ENTRY_ID = '" + dataset_key + "'") ;
	    if(r.next())
	    {
	       r.getRow() ;
	       dataid = sr.LitWithEnt(xt.makeValid(r.getString("ENTRY_ACRONYM") ));
	       dataname = sr.LitWithEnt(xt.makeValid(r.getString("ENTRY_NAME") ));
	    }
	    r.close() ;
	 
            sbr.append("          <DataSet dataid=\"" + dataid + "\">\n") ;
	    sbr.append("             <DataName>" + dataname + "</DataName>\n") ;
	    sbr.append("             <TypeOfData>" + typeofdata + "</TypeOfData>\n") ;

	    // the following is a candidate for functionisation

	    r = s.executeQuery("SELECT PROGRESS_DESCR FROM " + dbs + "PROGRESS WHERE PROGRESS_ID IN " +
                              "(SELECT PROGRESS_ID FROM " + dbs + "ENTRY WHERE ENTRY_ID='" + dataset_key + "')") ;

            progress = place_holder;
	 
            if(r.next())
            {
               r.getRow() ;
               progress = sr.LitWithEnt(xt.makeValid(r.getString("PROGRESS_DESCR") ));
            }
            r.close() ;

	    sbr.append("             <Status>" + progress + "</Status>\n") ;

	    sbr.append("             <LogicalDescription>\n") ;

            appendConditionsForEntry(dataset_key, 15, sbr) ; // 5 levels in

	    sbr.append("             </LogicalDescription>\n") ;


	    // now select the data_files associated with this data_set


            r = s.executeQuery("SELECT DISTINCT SPEC_ENTRY_ID FROM " + dbs + "ENTRY_CONNECT WHERE GEN_ENTRY_ID = '" + dataset_key +
	                    "' INTERSECT SELECT ENTRY_ID FROM " + dbs + "ENTRY WHERE ENTRY_TYPE_ID IN " +
	  		    "(SELECT ENTRY_TYPE_ID FROM " + dbs + "ENTRY_TYPE WHERE ENTRY_TYPE LIKE '%DATA_FILE%')") ;

            while(r.next())
            {
	       r.getRow() ;
	       datafiles.add(Integer.toString(r.getInt("SPEC_ENTRY_ID"))) ;
            }
            r.close() ;

            //and now for each entry
            Iterator ro = datafiles.iterator() ;
            String datafile_key = place_holder ;

	    String data_file_entry_name = place_holder ;
      
	    //put in the child references for files
            while(ro.hasNext())
            {
	       datafile_key = (String) ro.next() ;
	       
	       r = s.executeQuery("SELECT ENTRY_NAME FROM " + dbs + "ENTRY WHERE ENTRY_ID = '" + datafile_key + "'") ;
	       if(r.next())
	       {
		  r.getRow() ;
		  data_file_entry_name = sr.LitWithEnt(xt.makeValid(r.getString("ENTRY_NAME") ));
	       }
	       r.close() ;

	       sbr.append("            <File dataid=\"" + data_file_entry_name + "\">\n") ;
	       sbr.append("               <DataName>" + data_file_entry_name + "</DataName>\n") ;
	       //sbr.append("               <URI>" + getStoragePath(datafile_key, 3) + "</URI>\n") ;
	       sbr.append("               <URI>" + data_file_entry_name + "</URI>\n") ;
	       sbr.append("            </File>\n") ;

	       //add dataholdinglocation info to sbr_dhl
	       //////
	       sbr_dhl_files.append("         <FileLocator dataidref=\"" + data_file_entry_name + "\">\n") ;
               sbr_dhl_files.append("            <URI>" + data_file_entry_name + "</URI>\n") ;
	       sbr_dhl_files.append("            <Locator type=\"absolute\">\n") ;
               sbr_dhl_files.append("               <URL>" + getStoragePath(datafile_key, 3) + "</URL>\n") ;
	       sbr_dhl_files.append("            </Locator>\n") ;
	       sbr_dhl_files.append("         </FileLocator>\n") ;
	       //////
	    }

	    //put in child references for ras_collections
	    //Iterator i = rascollections.iterator() ;
	    //while(i.hasNext())
	    //{
	    //   sbr.append("            <ChildDataSetRef>" + (String) i.next() + "</ChildDataSetRef>\n") ;
	    //}   

	    sbr.append("            </DataSet>\n") ;

	    // catch the dataholdinglocator information for datasets also
	    //////
            sbr_dhl.append("         <DataSetLocator dataidref=\"" + dataname + "\">\n") ;
            sbr_dhl.append("            <DataName>" + dataname + "</DataName>\n") ;
	    sbr_dhl.append("            <Locator type=\"absolute\">\n") ;
	    //log.debug("We are populating the data sets e_name= " + e_name + " e_id= " + e_id ) ;
            sbr_dhl.append("               <URL>" + getStoragePath(dataset_key, 3) + "</URL>\n") ;
	    sbr_dhl.append("            </Locator>\n") ;
	    sbr_dhl.append("         </DataSetLocator>\n") ;
	    //////

	    //put in the RAS_COLLECTIONS
	    //i = rascollections.iterator() ;
	    //String ras_col = place_holder ;

	    //topic_name = place_holder ;
	    //String unit_name = place_holder ;

	    //while(i.hasNext())
	    //{
	    //   ras_col = (String)i.next() ;

            //   sbr.append("            <DataSet dataid=\"" + ras_col + "\">\n") ;
	    //   sbr.append("               <DataName>" + ras_col + "</DataName>\n") ;

	       //extract the topic_name for this rasdaman collection
            //   r = s.executeQuery("select distinct topic_name from " + dbs + "topic where topic_id in " + 
            //                      "(select topic_id from " + dbs + "parameter where key in " +
            //                      "(select key from " + dbs + "entry where entry_type_id in " + 
            //                      "(select entry_type_id from " + dbs + "entry_type where entry_type like '%Grib message%') " + 
            //                      "intersect " +
            //                      "select distinct key from " + dbs + "ras_connect where " +
	    //			  "ras_collection like '%" + ras_col + "%') )" ) ;
	    //   if(r.next())
	    //   {
	    //	  r.getRow() ;
	    //	  topic_name = sr.LitWithEnt(xt.makeValid(r.getString("TOPIC_NAME") ));
	    //   }
	    //   r.close() ;

	       //extract the units for the parameter stored in this rasdaman collection
	    //   r = s.executeQuery("select distinct unit_acronym from " + dbs + "unit where unit_id in " + 
            //                      "(select unit_id from " + dbs + "parameter where key in " +
            //                      "(select key from " + dbs + "entry where entry_type_id in " + 
            //                      "(select entry_type_id from " + dbs + "entry_type where entry_type like '%Grib message%') " + 
            //                      "intersect " +
            //                      "select distinct key from " + dbs + "ras_connect where " +
	    //			  "ras_collection like '%" + ras_col + "%') )" ) ;
	    //   if(r.next())
	    //   {
	    //	  r.getRow() ;
	    //	  unit_name = sr.LitWithEnt(xt.makeValid(r.getString("UNIT_ACRONYM") ));
	    //   }
	    //   r.close() ;
	       
	    //   sbr.append("               <TypeOfData>RasDaMan-Collection(MDD)</TypeOfData>\n") ;
	    //   sbr.append("               <Status>complete</Status>\n") ;
	    //   sbr.append("               <LogicalDescription>\n") ;
	    //   sbr.append("                  <Parameter>\n") ;
	    //   sbr.append("                     <ParamName>" + topic_name + "</ParamName>\n") ;
	    //   sbr.append("                     <Units>" + unit_name + "</Units>\n") ;
	    //   sbr.append("                  </Parameter>\n") ;
	    //   sbr.append("               </LogicalDescription>\n") ;
	    //   sbr.append("               <File dataid=\"" + ras_col + "\">\n") ;
	    //   sbr.append("                  <DataName>" + ras_col + "</DataName>\n") ;
	    //   sbr.append("                  <URI>" + getRasPath(ras_col) + "</URI>\n") ;
	    //   sbr.append("               </File>\n") ;
	    //   sbr.append("               <ParentDataSetRef>" + dataname + "</ParentDataSetRef>\n") ;
	    //   sbr.append("            </DataSet>\n") ;

	       // fill in the dataholdinglocation information for the ras collections
               //////
	    //   sbr_dhl.append("         <DataSetLocator dataidref=\"" + ras_col + "\">\n") ;
            //   sbr_dhl.append("         <DataName>" + ras_col + "</DataName>\n") ;
            //   sbr_dhl.append("         <Locator type=\"absolute\">\n") ;
            //   sbr_dhl.append("            <URL>" + getRasPath(ras_col) + "</URL>\n") ;
            //   sbr_dhl.append("         </Locator>\n") ;
            //   sbr_dhl.append("         </DataSetLocator>\n") ; 
               //////
	    //} //filled in the ras collections

	 } //filled in the data sets

         sbr.append("      </DataHolding>\n") ;

      } //filled in the experiments

      // and now for the datalocations - need to have an overall location and
      // then the different dataset - aka directory names

      //changed for equality with ISIS&SR to DataHoldingLocator
      //sbr.append("      <DataLocation>\n") ;
      //This is a HERE limitation as we can now only have one
      //experiment (i.e. dataholding) per study whereas we need multiple
      //dataholding for the MPIM as this is essentially one per experiment
      //with many experiments in one project

      //experiments now in sbr_dhl

      //data sets now in sbr_dhl
      
      //rasdaman collections now in sbr_dhl
      //for(Iterator i = rascollections.iterator() ; i.hasNext() ; ras_col = (String)i.next())
      
      //data files now in sbr_dhl

      ////////////// - following moved to experiments bit so get the id attribute
      //sbr.append("      <DataHoldingLocator>\n") ;

      //add collected holding location data
      sbr.append(sbr_dhl.toString()) ;
      //as FileLocators needed after DataSetLocatos
      sbr.append(sbr_dhl_files.toString()) ; 
      sbr.append("      </DataHoldingLocator>\n") ;
      //////////////
      

      //fill out the child dataset (which are really just going to contain one file (or perhaps the file and)
      //the rascollection with they are in - or perhaps that should be in a datalocation
	         	 
      ///////////////////////////////////////////////////////////////////
      //////////  HERE above added some below may need changing ///////////////
      ///////////////////////////////////////////////////////////////////


      //The Grib datafile
      //
      //sbr.append("          <DataSet dataid=\"grib" + key + "\">\n") ;
      //sbr.append("             <DataName>" + entry_name + ".cera" + key + "</DataName>\n") ;
      //sbr.append("             <TypeOfData>GRIB-File</TypeOfData>\n") ;
      //sbr.append("             <Status>" + progress + "</Status>\n") ;
      //sbr.append("          </DataSet>\n") ; 
      
      //The RasDaMan DataSet
                 
      //sbr.append("          <DataSet dataid=\"rasdaman" + key + "\">\n") ;
      //sbr.append("             <DataName>" + ras_collection + ".cera" + key + "</DataName>\n") ;
      //sbr.append("             <TypeOfData>RasDaMan:RAS_COLLECTION</TypeOfData>\n") ;
      //sbr.append("             <Status>" + progress + "</Status>\n") ;
      //sbr.append("          </DataSet>\n") ; 

      sbr.append("   </MetadataRecord>\n") ;

      return ;

   }
   //
   //
   //

   void appendConditionsForEntry(String entry_id, int num_spaces, StringBuffer sbr) throws SQLException
   {
      //
      //set session variables
      //

      SessionSingleton ss = SessionSingleton.getInstance() ;

      StringReplace sr=ss.getStringReplace() ;
      XmlText xt=ss.getXmlText() ;

      Statement s = ss.getStatement() ;
      ResultSet r = ss.getResultSet() ;

      Logger log = ss.getLogger() ;

      String place_holder = ss.place_holder ;
      int i_place_holder = ss.i_place_holder ;
      float f_place_holder = ss.f_place_holder ;

      //retrieve the database name
      String dbs = ss.getDbs() ;
	   
      //adds the conditions for the given entry_id to the current state
      //of the string buffer sbr.

      StringBuffer spcs = new StringBuffer() ;

      for(int i = 0 ; i < num_spaces ; i++ )
      {
	 spcs.append(" ") ;
      }

      String spaces = spcs.toString() ;
      
      //Condition

      // FIrstly setting the names

      String min_lat_n = "MIN_LAT" ;
      String max_lat_n = "MAX_LAT" ;
      String min_lon_n = "MIN_LON" ;
      String max_lon_n = "MAX_LON" ;
      String max_alt_n = "MAX_ALTITUDE" ;
      String min_alt_n = "MIN_ALTITUDE" ;
      String start_year_n = "START_YEAR" ;
      String start_month_n = "START_MONTH" ;
      String start_day_n = "START_DAY" ;
      String stop_year_n = "STOP_YEAR" ;
      String stop_month_n = "STOP_MONTH" ;
      String stop_day_n = "STOP_DAY" ;

      // Secondly getting there values
 
      r = s.executeQuery("SELECT MIN_LAT, MAX_LAT, MIN_LON, MAX_LON, MIN_ALTITUDE, MAX_ALTITUDE " +
                         "FROM " + dbs + "SPATIAL_COVERAGE WHERE SPATIAL_COVERAGE_ID IN " +
                         "(SELECT SPATIAL_COVERAGE_ID FROM " + dbs + "COVERAGE WHERE ENTRY_ID='" + entry_id + "')") ;

      float min_lat = f_place_holder ;
      float max_lat = f_place_holder ;
      float min_lon = f_place_holder ;
      float max_lon = f_place_holder ;
      float min_alt = f_place_holder ;
      float max_alt = f_place_holder ;
      
      //HERE 

      try {

         if(r.next())
         {
            r.getRow() ;

            min_lat = r.getFloat("MIN_LAT") ;
            max_lat = r.getFloat("MAX_LAT") ;
            min_lon = r.getFloat("MIN_LON") ;
            max_lon = r.getFloat("MAX_LON") ;
            min_alt = r.getFloat("MIN_ALTITUDE") ;
            max_alt = r.getFloat("MAX_ALTITUDE") ;
         }

         r.close() ;
      }
      catch (Exception e) {
	      log.debug(sbr.toString()) ;
      }

      r = s.executeQuery("SELECT START_YEAR, START_MONTH, START_DAY, STOP_YEAR, STOP_MONTH, STOP_DAY " +
                         "FROM " + dbs + "TEMPORAL_COVERAGE WHERE TEMPORAL_COVERAGE_ID IN " +
                         "(SELECT TEMPORAL_COVERAGE_ID FROM " + dbs + "COVERAGE WHERE ENTRY_ID='" + entry_id + "')") ;

      int start_year  = i_place_holder ;
      int start_month = i_place_holder ;
      int start_day   = i_place_holder ;
      int stop_year   = i_place_holder ;
      int stop_month  = i_place_holder ;
      int stop_day    = i_place_holder ;

      if(r.next())
      {
         r.getRow() ;

         start_year = r.getInt("START_YEAR") ;
         start_month = r.getInt("START_MONTH") ;
         start_day = r.getInt("START_DAY") ;
         stop_year = r.getInt("STOP_YEAR") ;
         stop_month = r.getInt("STOP_MONTH") ;
         stop_day = r.getInt("STOP_DAY") ;
      }

      r.close() ;

      //Thirdly we need to get the Units of the values in the conditions

      //some are not set in CERA and some are hence some by query and others
      //are set constant

      String min_alt_unit = place_holder ;
      String max_alt_unit = place_holder ;

      //String long_lat = "DEGREES" ;
      String long_lat = "MILLIDEGREES" ;
      String year = "YEARS" ;
      String month = "MONTHS" ;
      String day = "DAYS" ;

      r = s.executeQuery("SELECT UNIT_NAME FROM " + dbs + "UNIT WHERE UNIT_ID IN " +
                         "(SELECT MIN_ALT_UNIT_ID FROM " + dbs + "SPATIAL_COVERAGE WHERE SPATIAL_COVERAGE_ID IN " +
                            "(SELECT SPATIAL_COVERAGE_ID FROM " + dbs + "COVERAGE WHERE ENTRY_ID='" + entry_id + "')" +
                         ")") ;

      if(r.next())
      {
         r.getRow() ;
         min_alt_unit = sr.LitWithEnt(xt.makeValid(r.getString("UNIT_NAME") ));
      }

      r.close() ;

      r = s.executeQuery("SELECT UNIT_NAME FROM " + dbs + "UNIT WHERE UNIT_ID IN " +
                       "(SELECT MAX_ALT_UNIT_ID FROM " + dbs + "SPATIAL_COVERAGE WHERE SPATIAL_COVERAGE_ID IN " +
                          "(SELECT SPATIAL_COVERAGE_ID FROM " + dbs + "COVERAGE WHERE ENTRY_ID='" + entry_id + "')" +
                       ")") ;

      if(r.next())
      {
         r.getRow() ;
         max_alt_unit = sr.LitWithEnt(xt.makeValid(r.getString("UNIT_NAME") ));
      }

      r.close() ;

      // v- value, n- name
      String location_name_v = place_holder ;
      String location_name_n = "LOCATION_NAME" ;

      r = s.executeQuery("SELECT LOCATION_NAME FROM " + dbs + "LOCATION WHERE LOCATION_ID IN " +
                         "(SELECT LOCATION_ID FROM " + dbs + "LOCATION_CONNECT WHERE ENTRY_ID='" + entry_id + "')") ;
      if(r.next())
      {
         r.getRow() ;
         location_name_v = sr.LitWithEnt(xt.makeValid(r.getString("LOCATION_NAME") ));
      }
      r.close() ;

      //CLRC-MD note - see no reason why all paramter triples cant be enclosed inside
      // a just one <Condition/> block - perhaps change name to Conditions

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + min_lat_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + long_lat + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + min_lat + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + max_lat_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + long_lat + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + max_lat + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + min_lon_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + long_lat + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + min_lon + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + max_lon_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + long_lat + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + max_lon + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + min_alt_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + min_alt_unit + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + min_alt + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + max_alt_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + max_alt_unit + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + max_alt + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + location_name_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <ParamValue>" + location_name_v + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + start_year_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + year + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + start_year + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + start_month_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + month + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + start_month + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + start_day_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + day + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + start_day + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + stop_year_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + year + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + stop_year + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;
  
      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + stop_month_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + month + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + stop_month + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>" + stop_day_n + "</ParamName>\n") ;
      sbr.append(spaces + "   <Units>" + day + "</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + stop_day + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      

      // addtional information that is not in the normal module that is in
      // ERA Module DATA_ORGANIZATION, Version 1.3
      // additional item is the time that the reading was taken

      r = s.executeQuery("select year||lpad(month,2,0)||lpad(day,2,0)||lpad(hour,2,0)||" +
                         "lpad(minute,2,0)||lpad(second,2,0) \"TIME\", UTC_DIFF_HOURS \"TZ\" " +
                         "from " + dbs + "moment where moment_id in " +
                         "(select moment_id from " + dbs + "time_connect where time_id in " +
                         "(select time_id from " + dbs + "data_org where data_org_id in " +
                         "(select data_org_id from " + dbs + "parameter where entry_id='" + entry_id + "') ) ) ") ;

      String mtime = place_holder ;
      int mtz = i_place_holder ;
      StringBuffer tr = new StringBuffer() ;

      if(r.next())
      {
         r.getRow() ;
         mtime = sr.LitWithEnt(xt.makeValid(r.getString("TIME") ));
         mtz = r.getInt("TZ") ;
      }

      tr.append(mtime) ;

      if(mtz < 0 )
      {
         tr.append("-") ;
         if(mtz > -10 )
         {
            tr.append("0") ;
         }

         // remove the leading - sign
         tr.append(Math.abs(mtz)) ;
      }
      else
      {
         tr.append("+") ;
         if(mtz < 10)
         {
            tr.append("0") ;
         }
         
         tr.append(mtz) ;
      }

      sbr.append(spaces + "<Condition>\n") ;
      sbr.append(spaces + "   <ParamName>MOMENT</ParamName>\n") ;
      sbr.append(spaces + "   <Units>YYYYMMDD24HHMISSTTT</Units>\n") ;
      sbr.append(spaces + "   <ParamValue>" + tr.toString() + "</ParamValue>\n") ;
      sbr.append(spaces + "</Condition>\n") ;

      return ;
   }

   //
   //
   //

   String getStoragePath(String entry_id, int path_depth) throws SQLException
   {
      //queries the storage table for this particular entry 
      //retrieving the required number of items
      //1 - storage1
      //2 - storage 1 & 2
      //3 - storage 1 & 2 & 3
      //4 - storage 1 & 2 & 3 & 4
      //note the items are concatenated together

      SessionSingleton ss = SessionSingleton.getInstance() ;
      Logger log = ss.getLogger() ;
      int i_place_holder = ss.i_place_holder ;
      String place_holder = ss.place_holder ;

      Statement s = ss.getStatement() ;
      ResultSet r = ss.getResultSet() ;

      StringReplace sr = ss.getStringReplace() ;
      XmlText xt = ss.getXmlText() ;

      //retrieve the database name
      String dbs = ss.getDbs() ;

      log.debug("in the beginning the path_depth = " + path_depth) ;
      
      StringBuffer result = new StringBuffer() ;
      

      //The 4 storage id need specifying as the dataset held in the dataholding

      r = s.executeQuery("SELECT STORAGE1_ID, STORAGE2_ID, STORAGE3_ID, STORAGE4_ID FROM " + dbs + "DATA_ACCESS " +
                         " WHERE DATA_ACCESS_ID IN (SELECT DATA_ACCESS_ID FROM " + dbs + "PARAMETER WHERE " + 
                           "ENTRY_ID='" + entry_id + "')") ;

      int stor1 = i_place_holder ;
      int stor2 = i_place_holder ;
      int stor3 = i_place_holder ;
      int stor4 = i_place_holder ;

      if(r.next())
      {
         r.getRow() ;
         stor1 = r.getInt("STORAGE1_ID") ;
         stor2 = r.getInt("STORAGE2_ID") ;
         stor3 = r.getInt("STORAGE3_ID") ;
         stor4 = r.getInt("STORAGE4_ID") ;
      }
 
      r.close() ;

      //get the names

      String stor1_n = place_holder ;
      String stor2_n = place_holder ;
      String stor3_n = place_holder ;
      String stor4_n = place_holder ;
      String ras_collection = place_holder ;

      r = s.executeQuery("SELECT STORAGE_NAME FROM " + dbs + "STORAGE WHERE STORAGE_ID='" + stor1 + "'") ;
      if(r.next())
      {
         r.getRow() ;
         stor1_n = sr.LitWithEnt(xt.makeValid(r.getString("STORAGE_NAME") ));
      }
      r.close() ;

      r = s.executeQuery("SELECT STORAGE_NAME FROM " + dbs + "STORAGE WHERE STORAGE_ID='" + stor2 + "'") ;
      if(r.next())
      {
         r.getRow() ;
         stor2_n = sr.LitWithEnt(xt.makeValid(r.getString("STORAGE_NAME") ));
      }
      r.close() ;

      r = s.executeQuery("SELECT STORAGE_NAME FROM " + dbs + "STORAGE WHERE STORAGE_ID='" + stor3 + "'") ;
      if(r.next())
      {
         r.getRow() ;
         stor3_n = sr.LitWithEnt(xt.makeValid(r.getString("STORAGE_NAME") ));
      }
      r.close() ;
                                                
      r = s.executeQuery("SELECT STORAGE_NAME FROM " + dbs + "STORAGE WHERE STORAGE_ID='" + stor4 + "'") ;
      if(r.next())
      {
         r.getRow() ;
         stor4_n = sr.LitWithEnt(xt.makeValid(r.getString("STORAGE_NAME") ));
      }
      r.close() ;

      if(path_depth > 0)
      {
	 //result.append("http://www.") ;
	 result.append(stor1_n) ;
      }
      
      if(path_depth > 1)
      {
	 result.append(":") ;
	 result.append(stor2_n) ;
      }
      
      log.debug("entry_id = " + entry_id +  "; path_depth = " + path_depth) ;

      if(path_depth > 2)
      {
	 //result.append("/") ;
	 result.append(":") ;
	 result.append(stor3_n) ;
      }

      if(path_depth > 3)
      {
	 result.append(":") ;
	 result.append(stor4_n) ;
      }
  
      return result.toString() ;
   }

  //
  //
  //
  //

}
