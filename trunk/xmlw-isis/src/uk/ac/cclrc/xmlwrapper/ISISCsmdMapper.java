//package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;

import java.sql.* ; //jdbc

import org.apache.log4j.* ;

public class ISISCsmdMapper implements CsmdMapper
{

   // local variables
   //get local values needed from SessionSingleton - set in constructor - to avoid unnecessary repitition
   SessionSingleton ss  = null ;
   //data masaging functions so it is xml text compliant
   StringReplace    sr  = null ;
   XmlText          xt  = null ;
   //log4j logger
   Logger           log = null ;
   //jdbc handles
   Statement s = null ;
   ResultSet r = null ;
   //default values when none are available
   String place_holder = null ;
   int i_place_holder = 0 ;
   //indentation level for layout reasons (ti - tree indent - indent level for subtrees)
   String ti = null ;
   int    level_indent = 0 ;
   String li = null ;
   //retrieve the database schema name if their is a need to prepend this before the table names
   String dbs = null ;

   public ISISCsmdMapper()
   {
      //setup wrapper name
      ss.setWrapperName("ISIS") ; // perhaps this duplicates the facility name - but they could be different
      //setup these local variables as it would be too verbose to put them at the start of each function in this class
      //get local values needed from SessionSingleton
      this.ss = SessionSingleton.getInstance() ;
      this.r=ss.getStringReplace() ;
      this.xt=ss.getXmlText() ;
      //setup logger
      ss.setLogger(ISISCsmdMapper.class.getName() + ".class" ) ;
      this.log = ss.getLogger() ;
      this.place_holder = ss.place_holder ;
      this.i_place_holder = ss.i_place_holder ;
      this.ti = indentToStr(ss.indent) ;
      this.level_indent = ss.indent ;
      this.li = indentToStr(levell_ident) ; 
      //retrieve the database name
      this.dbs = ss.getDbs() ;

      try
      {
         ss.SetDbConnectionInfo() ;
      }
      catch (IOException e)
      {
         log.fatal("Cannot load" + ss.getPropFile() + "\n\t Exiting ....") ;
         System.exit(-1) ;
      }

      //connect to the database
      DBHelper dbh = ss.getRelDBHelper() ;
      dbh.connectToDB() ;

      //note have to remember to re-set the local handles from the SessionSingleton if we have disconnected
      //as the local handles will be stale
      this.s = ss.getStatement() ;
      this.r = ss.getResultSet() ;

   }

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
            temp_str = ("SELECT STUDY_ID FROM " + dbs + "STUDY WHERE STUDY_ID IN " +
                        "(SELECT STUDY_ID FROM " + dbs + "STUDY_TOPIC WHERE TOPIC_ID IN " +
                        "(SELECT TOPIC_ID FROM " + dbs + "TOPIC WHERE lower(TOPIC) LIKE '%" + discipline + "%'))") ;
	    discipline = temp_str ;
	 }

	 if(subject.compareTo("") != 0)
         {
            temp_str = ("SELECT DISTINCT STUDY_ID FROM " + dbs + "STUDY WHERE STUDY_ID IN " +
                        "(SELECT STUDY_ID FROM " + dbs + "INVESTIGATION WHERE INVESTIGATION_ID IN " +
                        "(SELECT INVESTIGATION_ID FROM " + dbs + "DATA WHERE DATA_ID IN " +
                        "(SELECT DATA_ID FROM " + dbs + "PARAMETER WHERE lower(NAME) LIKE '%" + subject + "%')))") ;
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
            
	    temp_str = ("SELECT STUDY_ID FROM " + dbs + "STUDY " +
		        "WHERE lower(NAME) LIKE '%" + studyname + "%' ") ; 

	    studyname = temp_str ;
	 }

	 if(studyid.compareTo("") != 0)
         {
            // as above note - should this have 2 versions
            temp_str = ("SELECT STUDY_ID FROM " + dbs + "STUDY WHERE lower(ENTRY_ACRONYM) LIKE '%" + studyid + "%' ") ; 

	    studyid = temp_str ;
	 }

	 if(dminstitution.compareTo("") != 0)
         {
            temp_str = ("SELECT STUDY_ID FROM " + dbs + "STUDY WHERE STUDY_ID IN " +
                        "(SELECT STUDY_ID FROM " + dbs + "STUDY_INSTITUTION WHERE INSTITUTE_ID IN " +
                       "(SELECT INSTITUTE_ID FROM " + dbs + "INSTITUTION WHERE lower(NAME) LIKE '%" + dminstitution + "%'))") ;
	    dminstitution = temp_str ;
	 }

	 //remove the day element from the startdate and enddate as this date is not in
	 //temporal coverage in the database i.e. the YYYYMM portion of the YYYYMMDD date format

	 if(startdate.compareTo("") != 0)
         {
            // may have to mod this based on timezone info and nls_date_format/timstamp issues
            temp_str = ("SELECT START_DATE FROM " + dbs + "STUDY WHERE START_DATE LIKE '%" + startdate.substring(0,6) + "%'"); 
	    startdate = temp_str ;
	 }

	 if(enddate.compareTo("") != 0)
         {
            temp_str = ("SELECT END_DATE FROM " + dbs + "STUDY WHERE END_DATE LIKE '%" + enddate.substring(0,6) + "%'"); 
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
         sql.append("SELECT STUDY_ID FROM " + dbs + "STUDY") ;
      }

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
               sb.append(r.getInt("STUDY_ID")) ;
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

            
      log.debug("contents:\t" + sb.toString()) ;

      return sb.toString() ;
    
   }

   //
   //
   // 
   //

   String indentToStr(int indent)
   {
      StringBuffer sbr = new StringBuffer() ;
     
      for(int i = 0; i < indent ; i++)
      {
         sbr.append(" ") ;
      }

      return sbr.toString() ;
   }
   //
   //
   //
   //

   //Indexing  - note ii is initial indent - is the indentation level (i.e. number of space in a String)
   void buildMDTopic(String key, StringBuffer sbr, String  ii, String type) throws SQLException
   {

      sbr.append(ii +  "<Topic>\n") ;

      buildMDKeywords(key, sbr, ii+li, null) ;
      //following will have to list all distinct subjects related to study
      //ISIS only have keywords
      //buildMDSubject(key, sbr, ii+li, null) ;

      sbr.append(ii +  "</Topic>\n") ;

      return ;
   }

   //note spaceing of ii worked out for you by calling function - thus we pass on requests for ii + li to called methods
   void buildMDKeywords(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String keyword = place_holder ;

      r = s.executeQuery("select name from " + dbs + "keyword where keyword.id in (select keyword.id from " + dbs + "keyword_list" +
                         " where study_id  = '" + key + "'") ; 
      
      sbr.append(ii + "<Keywords>\n") ;

      //temp - perhaps need re-classifying
      sbr.append(ii+li+"<Discipline>" + ss.getWrapperName() + "</Discipline>\n") ;

      while(r.next())
      {
         r.getRow() ;
         keyword = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
         sbr.append(ii+li+"<Keyword>" + keyword + "</Keyword>\n") ;
      }
      r.close() ;
 
      sbr.append(ii + "</Keywords>\n") ;

      return ;
   }

   //ISIS only have keywords
   //void buildMDSubjects(String key, StringBuffer sbr, String ii, String type) throws SQLException
   //{
   //   String ii = indentToStr(initial_indent) ;
   //
   //     return ;
   // }

   //Study/investigation information (keeping mainly leaf node level complexity)

   void buildMDStudy(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
    
      String studyname = place_holder ;
      String notes = place_holder ; 

      r = s.executeQuery("select name, notes from study where id='" + key + "'") ;
      if(r.next())
      {
         r.getRow() ;
         studyname = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
         notes = sr.LitWithEnt(xt.makeValid(r.getString("NOTES") ));
      }
      r.close() ;
      sbr.append(ii + "<Study StudyID=\"" + key "\">\n") ;
      sbr.append(ii + li + "<StudyName>" + studyname + "</StudyName>\n") ;

      buildMDStudyInstitution(key, sbr, ii+li, null) ;  
      buildMDStudyPerson(key, sbr, ii+li, null) ;  
      buildMDStudyInformation(key, sbr, ii+li, null) ;  
     
      sbr.append(ii + li + "<Notes>" + notes + "</Notes>\n") ;

      buildMDRelatedReferance(key, sbr, ii+li, null) ;  
      buildMDInvestigation(key, sbr, ii+li, null) ;  

      sbr.append(ii + "</Study>\n") ;

      return ;
   }
       



      return ;
   }
   void buildMDNameRole(String key, StringBuffer sbr, String ii, String type) throws SQLException //some where you might use type as
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
													   //you can have InstitutionRole and PersonRole
   void buildMDContactDetails(String key, StringBuffer sbr, String ii, String type) throws SQLException //similar to buildMDContact template
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDStudyPerson(String key, StringBuffer sbr, String ii, String type) throws SQLException //similar to buildMDContact template
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDStudyInstitution(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      //there is no such info in the ISIS ICAT yet so essentially putting in a meaningful place holder

      sbr.append(ii + "<StudyInsitution>\n");
      sbr.append(ii + li + "<Name institutiontype=\"research\">ISIS, RAL</Name>\n") 
      sbr.append(ii + li + "<Role>Facility</Role>\n") ;
      sbr.append(ii + "</StudyInsitution>\n");

      return ;
   }

   void buildMDRelatedReference(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDRelatedReferanceLocation(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }

   void buildMDInvestigation(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }

   void buildMDDataHolding(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDDataDescription(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDDataTopic(String key, StringBuffer sbr, String ii, String type) throws SQLException //will use Keywords & Subjects above
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }

   void buildMDLogicalDescription(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDParameter(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDTimePeriod(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDFacilityUsed(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDDataHoldingLocator(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }

   void buildMDCollectionLocator(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDAtomicDataObject(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
   void buildMDADOLocator(String key, StringBuffer sbr, String ii, String type) throws SQLException //definately need type here
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }
 													     //files and selects

   //all the Misc type information
   void buildMDAccessConditions(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }

   void buildMDLegalNotice(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }

   void buildMDRelatedPublication(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }

   void buildMDOtherRelatedMaterial(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      return ;
   }

   

   void buildMDContact(String key, StringBuffer sbr, String ii, String contact_type) throws SQLException
   {
      String ii = indentToStr(initial_indent) ;

      r = s.executeQuery("select institution.name, institution.institution_id, address_1, address_2, town, region, postcode, country, " +
                         "title, forename, surname, other_initials, telephone, email, fax from " + dbs +
                         "person, " + dbs + "institution where institution.institution_id = person.institution_id and person_id in " +
                         "(select person_id from "+dbs+"study_person where study_id='" + key + "' and role like '%" + contact_type + "%') " ) ;

      int institute_id = i_place_holder ;
      String institute_name = place_holder ;
      String i_street = place_holder ;
      String i_place = place_holder ;
      String i_pobox_postal_code = place_holder ;
      String i_country = place_holder ;
      String last_name = place_holder ;
      String first_name = place_holder ;
      String second_name = place_holder ;
      String title = place_holder ;
      String telephone = place_holder ;
      String fax = place_holder ;
      String email = place_holder ;

      if(r.next())
      {
         institute_name = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
         institute_id = r.getInt("INSTITUTION_ID") ;
         i_street = sr.LitWithEnt(xt.makeValid(r.getString("ADDRESS_1") ));
         i_place = sr.LitWithEnt(xt.makeValid(r.getString("TOWN") ));
         i_pobox_postal_code = sr.LitWithEnt(xt.makeValid(r.getString("POSTCODE") ));
         i_country = sr.LitWithEnt(xt.makeValid(r.getString("COUNTRY") ));
         last_name = sr.LitWithEnt(xt.makeValid(r.getString("SURNAME") ));
         first_name = sr.LitWithEnt(xt.makeValid(r.getString("FORENAME") ));
         second_name = sr.LitWithEnt(xt.makeValid(r.getString("OTHER_INITIALS") ));
         title = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));
         telephone = sr.LitWithEnt(xt.makeValid(r.getString("TELEPHONE") ));
         fax = sr.LitWithEnt(xt.makeValid(r.getString("FAX") ));
         email = sr.LitWithEnt(xt.makeValid(r.getString("EMAIL") ));
      }
 
      r.close() ;

      if(contact_type.compareToIgnoreCase("Investigator") == 0 )
      {
         sbr.append(ii + "<Investigator>\n") ;
      }
      else if ( contact_type.compareToIgnoreCase("DataManager") == 0 )
      {
         sbr.append(ii + "<DataManager>\n") ;
      }
      else
      {
         log.info("unrecognised contact type:\t" + contact_type) ;
      }

         sbr.append(ii + ti + "<Name>\n") ;
         sbr.append(ii + ti + ti + "<Surname>" + last_name + "</Surname>\n") ;
         sbr.append(ii + ti + ti + "<Initials>" + first_name + " " + second_name + "</Initials>\n") ;
         sbr.append(ii + ti + "</Name>\n") ;
         sbr.append(ii + ti + "<Status>" + "inhouse" + "</Status>\n") ;
         sbr.append(ii + ti + "<Institution>" + institute_name + "</Institution>\n") ;
         sbr.append(ii + ti + "<ContactDetails>\n") ;
         sbr.append(ii + ti + ti +"<Address>\n") ;
         sbr.append(ii + ti + ti + ti + "<Addressline1>" + i_street + "</Addressline1>\n") ;
         sbr.append(ii + ti + ti + ti + "<Town>" + i_place + "</Town>\n") ;
         sbr.append(ii + ti + ti + ti + "<Country>" + i_country + "</Country>\n") ;
         sbr.append(ii + ti + ti + "</Address>\n") ;
         sbr.append(ii + ti + ti + "<Telephone>" + telephone + "</Telephone>\n") ;
         sbr.append(ii + ti + ti + "<Email>" + email + "</Email>\n") ;
         sbr.append(ii + ti + ti + "<Fax>" + fax + "</Fax>\n") ;
         sbr.append(ii + ti + "</ContactDetails>\n") ;
         sbr.append(ii + ti + "<Role>" + contact_type + "</Role>\n") ;

      if(contact_type.compareToIgnoreCase("Investigator") == 0 )
      {
         sbr.append(ii + "</Investigator>\n") ;
      }
      else if ( contact_type.compareToIgnoreCase("DataManager") == 0 )
      {
         sbr.append(ii + "</DataManager>\n") ;
      }
      else
      {
         log.info("unrecognised contact type:\t" + contact_type) ;
      }

      return ;

   }

      

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

      //all elements of the study need extracting as will be used later ;
      String study_id = place_holder ;

      r = s.executeQuery("select study_id from " + dbs + "study where study_id = '" + key + "'") ; 
      if(r.next())
      {
	 r.getRow() ;
	 study_id = sr.LitWithEnt(xt.makeValid(r.getString("STUDY_ID") ));
      }
      r.close() ;

      // li is of type String and it is the number of spaces in the indentation when going to a more nested level 
      //MetadataRecord starts
      sbr.append(li + "<MetadataRecord metadataID=\"" + ss.getWrapperName() + "-" + key + "\"" +
                             " facilityi=\"" + ss.getWrapperName()+ "\">\n");

      //Topic
      buildMDTopic(key, sbr, (li+li), null) ;
      //Study
      buildMDStudy(key, sbr, (li+li), null) ;
      //AccessConditions
      buildMDAccessConditions(key, sbr, (li+li), null) ;
      //LegalNotice
      buildMDLegalNotice(key, sbr, (li+li), null) ;
      //RelatedPublication
      buildMDRelatedPublication(key, sbr, (li+li), null) ;
      //OtherRelatedMaterial
      buildMDOtherRelatedMaterial(key, sbr, (li+li), null) ;

      //MetadataRecord ends
      sbr.append(indentToStr(level_indent) + "</MetadataRecord>\n");

      return ;

   }

   //
   //
   //

   void appendConditionsForEntry(String id, int num_spaces, String type, StringBuffer sbr) throws SQLException
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


      /////
      String dataset_in_list = place_holder ;

      if (type.compareTo("experiment") == 0) 
      {
         List dataset = new LinkedList() ;
         r = s.executeQuery("SELECT DATASET_ID FROM " + dbs + "DATASET WHERE STUDY_ID = '" + id + "'") ;
         while(r.next())
         {
            dataset.add(Integer.toString(r.getInt("DATASET_ID"))) ;
         }
         r.close() ;

         Iterator e = dataset.iterator() ;
         StringBuffer tmp_sb = new StringBuffer() ;

         tmp_sb.append("(") ;

         while(e.hasNext())
         {
            tmp_sb.append(((String) e.next())) ;
            tmp_sb.append(",") ;
         }

         tmp_sb.delete(tmp_sb.length()-1, tmp_sb.length()) ;

	 tmp_sb.append(")") ;

         dataset_in_list = tmp_sb.toString() ;

      }
      else if (type.compareTo("dataset") == 0 )
      {
         dataset_in_list = "(" + id + ")" ;
      }

      String data_in_list = ")" ;

      //if we have no dataset associated with this entry
      if ( dataset_in_list.compareTo(")") != 0 )
      {
   
         if (type.compareTo("experiment") == 0 || type.compareTo("dataset") == 0)
         {
            List data = new LinkedList() ;
   
            r = s.executeQuery("SELECT DATA_ID FROM " + dbs + "DATA WHERE DATASET_ID IN " + dataset_in_list ) ;
            while(r.next())
            {
               data.add(Integer.toString(r.getInt("DATA_ID"))) ;
            }
            r.close() ;

            Iterator e = data.iterator() ;
            StringBuffer tmp_sb = new StringBuffer() ;

            tmp_sb.append("(") ;

            while(e.hasNext())
            {
               tmp_sb.append(((String) e.next())) ;
               tmp_sb.append(",") ;
            }

            tmp_sb.delete(tmp_sb.length()-1, tmp_sb.length()) ;

	    tmp_sb.append(")") ;
       
            data_in_list = tmp_sb.toString() ;

         }
         else //this is just a data object
         {
           data_in_list = "("+ id + ")" ;
         }
      }// datafiles pulled out if any associated with this entry

      //dataset needs data as these contain the actual parameter data

      if (data_in_list.compareTo(")") != 0)
      {
         //get the date for all the rows of conditions infromation for this dataset or file
         String param_name = place_holder ;
         String param_unit = place_holder ;
         String param_value = place_holder ; 

         r = s.executeQuery("SELECT NAME, UNIT, VALUE FROM " + dbs + "PARAMETER WHERE DATA_ID IN " + data_in_list ) ;

         while(r.next())
         {
            param_name = sr.LitWithEnt(xt.makeValid(r.getString("PARAM_NAME") ));
            param_unit = sr.LitWithEnt(xt.makeValid(r.getString("PARAM_UNIT") ));
            param_value = sr.LitWithEnt(xt.makeValid(r.getString("PARAM_VALUE") ));

            sbr.append(spaces + "<Condition>\n") ;
            sbr.append(spaces + "   <ParamName>" + param_name + "</ParamName>\n") ;
            sbr.append(spaces + "   <Units>" + param_unit + "</Units>\n") ;
            sbr.append(spaces + "   <ParamValue>" + param_value + "</ParamValue>\n") ;
            sbr.append(spaces + "</Condition>\n") ;
         }
      } //dataset had data such that conditions have now been added

      return ;
   }

   //
   // main test function could be removed at a later date
   //

   public static void main (String argv[])
   {
      ISISCsmdMapper icm = new ISISCsmdMapper() ;

      String key = icm.getKeys(1) ;

      StringBuffer sbr = new StringBuffer(1024*1024) ;

      try
      {
         icm.buildMetadataRecord(key, sbr) throws SQLException
      }
      catch (SQLExceltion sqle)
      {
          sqle.printStackTrace() ;
      }
   
      System.out.print(sbr.toString) ;

      return  ;
   }


}
