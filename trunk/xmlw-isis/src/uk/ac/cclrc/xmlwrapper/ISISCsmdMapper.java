package uk.ac.cclrc.xmlwrapper ;

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
      //ISIS icat_v2 now has subjects
      buildMDSubject(key, sbr, ii+li, null) ;

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

   void buildMDSubjects(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      String topic_id = place_holder ;
      String subject = place_holder ;

      r = s.executeQuery("select topic_id from  topic_list where study_id = '" + key + "'") ;  

      sbr.append(ii + "<Subjects>\n") ;

      //temp - perhaps need re-classifying
      sbr.append(ii+li+"<Discipline>" + ss.getWrapperName() + "</Discipline>\n") ;

      while(r.next())
      {
         topic_id = sr.LitWithEnt(xt.makeValid(r.getString("TOPIC_ID") ));
 
         //note following needs own statement and resulset handles as called from a nested context 
         buildMDSubjectList(topic_id, sbr, ii+li, null) ;
         
      }
      r.close() ;

      sbr.append(ii + "</Subjects>\n") ;

      return ;
   }

   void buildMDSubjectList(String topic_id, StringBuffer sbr, String ii, String type) throws SQLException 
   {
      //as this is called from a nested context 
      Connection c = ss.getConnection() ;
      Statement t_s = c.createStatement() ;
      Resultset t_r = null ;
 
      List listy = new ArrayList() ;

      int level = 99 ;

      //recursion would have been better - but writing that stuff hurts my head !!
      while(level > 0) 
      {
         t_r = t_s.execute("select name, parent_id, level from topic where topic_id = '" + topic_id +"'") ;
 
         if(t_r.next())
         {
            name = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
            topic_id = sr.LitWithEnt(xt.makeValid(r.getString("PARENT_ID") ));
            level = sr.LitWithEnt(xt.makeValid(r.getString("LEVEL") ));

            listy.add(name) ;
         
            if(topic_id < 0 )
            {
               level = 0 ;
            }

         }
         else
         {
            level=0 ;
         }
      }

      t_r.close() ;
      t_s.close() ;

      int size=listy.size ;
      String ind = ii ;

      while(size > 0)
      {
         sbr.append(ind+"<Subject>\n") ;
         sbr.append(ind+li+"<SubjectName>" + (String)listy.get(--size) + "</SubjectName>\n") ;
         ind = ind+li ;
      }

      size=listy.size() ;
      int num_ent = 0 ;

      while(size > 0)
      {
         sbr.append(ind+"<Subject>\n") ;
         ind = ind.substring(li.length()) ; 
         --size ;
      }  

      return ;

   }

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
      sbr.append(ii + "<Study StudyID=\"" + key + "\">\n") ;
      sbr.append(ii + li + "<StudyName>" + studyname + "</StudyName>\n") ;

      buildMDStudyInstitution(key, sbr, ii+li, null) ;  
      buildMDStudyPerson(key, sbr, ii+li, null) ;  
      buildMDStudyInformation(key, sbr, ii+li, null) ;  
     
      sbr.append(ii + li + "<Notes>" + notes + "</Notes>\n") ;

      //not needed at the moment: - perhaps if their was a workflow extention
      //buildMDRelatedReferance(key, sbr, ii+li, null) ;  
      buildMDInvestigation(key, sbr, ii+li, null) ;  

      sbr.append(ii + "</Study>\n") ;

      return ;
   }
       

   void buildMDStudyInformation(String key, StringBuffer sbr, String ii, String type) throws SQLException //some where you might use type as
   {
      String start_date_date = place_holder ;
      String start_date_time = place_holder ;
      String funding = place_holder ;
      String end_date_date = place_holder ;
      String end_date_time = place_holder ;
      String purpose = place_holder ;
      String status = place_holder ;

      r = s.executeQuery("select funding_info, purpose, status, to_char(start_date, 'YYYY-MM-DD') \"SDD\", " +
                         "to_char(start_date, 'HH24:MI:SS') \"SDT\", " +
	                 "to_char(end_date, 'YYYY-MM-DD') \"EDD\", " +
	                 "to_char(end_date, 'HH24:MI:SS') \"EDT\" " +
	                 "from study where id='" + key + "'") ;

      if(r.next())
      {
         funding = sr.LitWithEnt(xt.makeValid(r.getString("FUNDING_INFO") ));
         start_date_date = sr.LitWithEnt(xt.makeValid(r.getString("SDD") ));
         start_date_time = sr.LitWithEnt(xt.makeValid(r.getString("SDT") ));
         end_date_date = sr.LitWithEnt(xt.makeValid(r.getString("EDD") ));
         end_date_time = sr.LitWithEnt(xt.makeValid(r.getString("EDT") ));
         purpose = sr.LitWithEnt(xt.makeValid(r.getString("PURPOSE") ));
         status = sr.LitWithEnt(xt.makeValid(r.getString("STATUS") ));
      }
      r.close() ;
  
      sbr.append(ii+"<StudyInformation>\n") ;
      sbr.append(ii+li+"<Funding>" + funding + "</Funding>\n") ;
      sbr.append(ii+li+"<TimePeriod>\n") ; 
      sbr.append(ii+li+li+"<StartDate>\n") ; 
      sbr.append(ii+li+li+li+"<Date>"+ start_date_date + "</Date>\n") ; 
      sbr.append(ii+li+li+li+"<Time>"+ start_date_time + "</Time>\n") ; 
      sbr.append(ii+li+li+"</StartDate>\n") ; 
      sbr.append(ii+li+li+"</EndDate>\n") ; 
      sbr.append(ii+li+li+li+"<Date>"+ start_date_date + "</Date>\n") ; 
      sbr.append(ii+li+li+li+"<Time>"+ start_date_time + "</Time>\n") ; 
      sbr.append(ii+li+li+"</EndDate>\n") ; 
      sbr.append(ii+li+"</TimePeriod>\n") ; 
      sbr.append(ii+li+"<Purpose>\n") ; 
      sbr.append(ii+li+li+"<Abstract>" + purpose + "</Abstract>\n")  ; 
      sbr.append(ii+li+"</Purpose>\n") ; 
      sbr.append(ii+li+"<Status>" + status + "</Status>\n") ; 
     

      return ;
   }

   void buildMDNameRole(String key, StringBuffer sbr, String ii, String type) throws SQLException //some where you might use type as
   {
      return ;
   }
													   //you can have InstitutionRole and PersonRole
   void buildMDContactDetails(String key, StringBuffer sbr, String ii, String type) throws SQLException //similar to buildMDContact template
   {

      return ;
   }
   void buildMDStudyPerson(String key, StringBuffer sbr, String ii, String type) throws SQLException //similar to buildMDContact template
   {
      String surname = place_holder ;
      String forename = place_holder ;
      String middle_initial = place_holder ;
      String title = place_holder ;
      String email = place_holder ;

      r = s.executeQuery ("select first_name, middle_name, last_name, title, email_address from party where party.id in " +
	                    "(select party_id from investigator_list where investigation_id in " +
	   		        "(select investigation_id from investigation_list where study_id = '" + key + "')" +
                             ")") ;

      while(r.next())
      {
         surname = sr.LitWithEnt(xt.makeValid(r.getString("LAST_NAME") ));
         forename = sr.LitWithEnt(xt.makeValid(r.getString("FIRST_NAME") ));
         middle_initials = sr.LitWithEnt(xt.makeValid(r.getString("MIDDLE_NAME") ));
         title = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));
         email = sr.LitWithEnt(xt.makeValid(r.getString("EMAIL_ADDRESS") ));

         //if data not present than data will be filled in with an empty string this may need changing

         sbr.append(ii+"<StudyPerson\n") ;
         sbr.append(ii+li+"<Name>\n") ;
         sbr.append(ii+li+li+"<Surname>" + surname + "</Surname>\n") ;
         sbr.append(ii+li+li+"<MiddleInitials>" + middle_initials + "</MiddleInitials>\n") ;
         sbr.append(ii+li+li+"<Forename>" + forename + "</Forename>\n") ;
         sbr.append(ii+li+li+"<Title>" + title + "</Title>\n") ;
         sbr.append(ii+li+"</Name>\n") ;
         sbr.append(ii+li+"<ContactDetails>\n") ;
         sbr.append(ii+li+li+"<Email>" + email + "</Email>\n") ;
         sbr.append(ii+li+"</ContactDetails>\n") ;
         sbr.append(ii+li+"<RoleInStudy>not available</RoleInStudy>\n") ;
         sbr.append(ii+"</StudyPerson\n") ;

      }
      r.close() ;

      return ;
   }
   void buildMDStudyInstitution(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      //there is no such info in the ISIS ICAT yet so essentially putting in a meaningful place holder

      sbr.append(ii + "<StudyInsitution>\n");
      sbr.append(ii + li + "<Name institutiontype=\"research\">ISIS, RAL</Name>\n") ;
      sbr.append(ii + li + "<Role>Facility</Role>\n") ;
      sbr.append(ii + "</StudyInsitution>\n");

      return ;
   }

   void buildMDRelatedReference(String key, StringBuffer sbr, String ii, String type, boolean nested) throws SQLException
   {
      //this can be done for datafile-ado however not populated yet and something to come back to

      //Connection c = ss.getConnection() ;
      //Statement t_s = null ;
      //Resultset t_r = null ;

      //if(nested == true)
      //{
      //   t_s=c.createStatement() ;
      //}
      //else
      //{
      //   t_s=s ;
      //}
      //
      //if ((type.compareTo("ado")==0) 
      //{
      // 
      //}

      return ;
   }

   //perhaps the following needs to be removed - perhaps
   void buildMDRelatedReferanceLocation(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {

      return ;
   }

   void buildMDInvestigation(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {
      //following needed as some calls from nested context:
      //as this is called from a nested context
      Connection c = ss.getConnection() ;
      Statement t_s = c.createStatement() ;
      Resultset t_r = null ;

      //variables
      String id = place_holder ;
      String experiment_number = place_holder ;
      String title = place_holder ;
      String investigation_type =place_holder ;
      String instrument_id = place_holder ;
      String inst_calibaration_id = place_holder ;
      String inv_abstract = place_holder ;
      String prev_experiment_number = place_holder ;
      String bcat_inv_str = place_holder ;
      String comments = place_holder ;

      String name = place_holder ;
      String short_name = place_holder ;
      String instrument_comments =  place_holder ;

      r = s.executeQuery ("select id, experiment_number, title, investigation_type, instrument_id, inst_calibration_id, inv_abstract, " +
                          " prev_experiment_number, bcat_inv_str, comments from investigation where investigation_id in " +
                          "(select investigation_id from investigation_list where study_id='" + key +"')") ;

      while(r.next())
      {
         
         id = sr.LitWithEnt(xt.makeValid(r.getString("ID") ));
         experiment_number = sr.LitWithEnt(xt.makeValid(r.getString("EXPERIMENT_NUMBER") ));
         title = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));
         investigation_type = sr.LitWithEnt(xt.makeValid(r.getString("INVASTIGATION_TYPE") ));
         instrument_id = sr.LitWithEnt(xt.makeValid(r.getString("INSTRUMENT_ID") ));
         inst_calibaration_id = sr.LitWithEnt(xt.makeValid(r.getString("INST_CALIBRATION_ID") ));
         inv_abstract = sr.LitWithEnt(xt.makeValid(r.getString("INV_ABSTRACT") ));
         prev_experiment_number = sr.LitWithEnt(xt.makeValid(r.getString("PREV_EXPERIMENT_NUMBER") ));
         bcat_inv_str = sr.LitWithEnt(xt.makeValid(r.getString("BCAT_INV_STR") ));
         comments = sr.LitWithEnt(xt.makeValid(r.getString("COMMENTS") ));

         sbr.append(ii+"<Investigation InvestigationID=\"investigation_" + id + "\">\n") ;
         sbr.append(ii+li+"<Name>"+title+"</Name>\n") ;
         sbr.append(ii+li+"<InvestigationType>"+investigation_type+"</InvestigationType") ; 
         sbr.append(ii+li+"<Abstract>"+inv_abstract+"</Abstract>\n") ;

         t_r=t_s.executeQuery("Select name, short_name, comments from intrument where id ='"+ instrument_idi +"'") ; 

         if(t_r.next())
         {
            name = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
            short_name = sr.LitWithEnt(xt.makeValid(r.getString("SHORT_NAME") ));
            instrument_comments = sr.LitWithEnt(xt.makeValid(r.getString("COMMENTS") ));
         }
         // put in the resource/instrument used 
         sbr.append(ii+li+"<Resources\n>") ; 
         sbr.append(ii+li+li+"<Name"> + name + "</Name>\n") ;
         sbr.append(ii+li+li+"<ShortName"> + short_name + "</ShortName>\n") ;
         sbr.append(ii+li+li+"<Comments"> + instrument_comments + "</Comments>\n") ;
         sbr.append(ii+li+"<Resources\n>") ; 

         t_r.close() ;

         //getting the related information/experimens

         if(prev_experiment_number.compareTo("")!=0)
         {
            buildMDRelatedReferance(prev_experiment_number, sbr, ii+li, "investigation", true) ;
         }

        //build data holding
        buildMDDataHolding(id, sbr, ii+li, true) ;

     } // all investigation for this study have now been processed
         

      return ;
   }

   void buildMDRelatedReferance(String key, StringBuffer sbr, String ii, String type, boolean nested) throws SQLException
   {
      Connection c = ss.getConnection() ;
      Statement t_s = null ;
      Resultset t_r = null ;

      if(nested == true)
      {
         t_s=c.createStatement() ;
      }
      else
      {
         t_s=s ;
      }
 
      if ((type.compareTo("dataholding")==0) || (type.compareTo("investigation")==0)) 
      {//variables needed

         String title = place_holder ;
         String investigation_id = place_holder ;
      
         t_r=t_s.executeQuery("select TITLE, investigation.id \"INVESTIGATION_ID\" from investigation where experiment_number = '" + key + "'") ;

         if(t_r.next())
         {
            title = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));
            investigation_id = sr.LitWithEnt(xt.makeValid(r.getString("INVESTIGATION_ID") ));

            sbr.append(ii+"<RelatedReference>\n") ;
            sbr.append(ii+li+"<Type>" + "previous" + "</Type>\n") ; 
            sbr.append(ii+li+"<ReferredToItem>" + "Experiment" + "</ReferredToItem>\n") ; 
      

            t_r.close() ;

            t_r = t_s.executeQuery("select study.name \"STUDYNAME\", study.id \"STUDY_ID\" from study where study.id in " +
                             "(select distinct(study_id) from investigation_list where investigation_id in " +
                             "(select investigation.id from invetigation where title='" + title + "'))") ;

            if(t_r.next())
            {
               studyname = sr.LitWithEnt(xt.makeValid(r.getString("STUDYNAME") )); 
               study_id = sr.LitWithEnt(xt.makeValid(r.getString("STUDY_ID") )); 
            }

            t_r.close() ;
    


            sbr.append(ii+li+"<ReferenceLocation>\n") ; 
            sbr.append(ii+li+li+"<Archive>"+"isis"+"</Archive>\n") ;
            sbr.append(ii+li+li+"<StudyName>"+studyname+"</StudyName>\n") ;
            sbr.append(ii+li+li+"<StudyId>"+study_id+"</StudyId>\n") ;
            sbr.append(ii+li+li+"<InvestigationName>"+title+"</InvestigationName>\n") ;
            sbr.append(ii+li+li+"<InvestigationId>"+investigation_id+"</InvestigationId>\n") ;
            sbr.append(ii+li+"</ReferenceLocation>\n") ; 
     
            sbr.append(ii+"<RelatedReference>\n") ;
         } //as you want it all filled in or all returning nothing

      }

      if(nested==true)
      {//as don't want to close global statement handle
         t_s.close() ;
      }

      return ;
   }

   void buildMDDataHolding(String key, StringBuffer sbr, String ii, boolean nested) throws SQLException
   {
      //the key should be the key not of the study but of the investigation - as each dataholding is associated to
      //an investigation

      sbr.append(ii+"<DataHolding InvestigationID=\"" + key + "\">\n") ;

      buildMDDataDescription(key, sbr, ii+li, "dataholding", true) ;
      buildMDDataHoldingLocator(key, sbr, ii+li, "dataholding", true) ;
      //HERE - what does related reference get the info from - note essentially redundant as investigation and dataholding are
      //synonymous - but put in here at the moment anyway - as may change
      buildMDRelatedReferance(key, sbr, ii+li, "dataholding", true) ;
      buildMDAtomicDataObject(key, sbr, ii+li, null) ;
      buildMDDataCollection(key, sbr, ii+li, null, true) ;

      sbr.append(ii+"</DataHolding>\n") ; 

      return ;
   }

   void buildMDDataCollection(String key, StringBuffer sbr, String ii, String type,  boolean nested) throws SQLException
   {
      Connection c = ss.getConnection() ;
      Statement t_s = null ;
      Resultset t_r = null ;

      if(nested == true)
      {
         t_s=c.createStatement() ;
      }
      else
      {
         t_s=s ;
      }

      String dataset_id = place_holder ;
      String name = place_holder ;
      String dataset_type = place_holder ;
      String dataset_status  = place_holder ;
      String description = place_holder ;
      String name = place_holder ;

      t_r = t_s.exectuteQuery("select dataset.id \"DATASETID\", name, dataset_type, dataset_status, description from dataset where "+
                              "dataset.id in (select datasetid from dataset_list where investigationid ='" + key + "')") ;

      while(t_r.next()) 
      {
         dataset_id = sr.LitWithEnt(xt.makeValid(r.getString("DATASETID") ));
         name = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
         dataset_type = sr.LitWithEnt(xt.makeValid(r.getString("dataset_type") ));
         dataset_status = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));
         description = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));

         sbr.append(ii+"<DataCollection dataid=\""+dataset_id+"\">\n") ;
         //HERE - remember dataholding->ado call needs revisit
         //atomic do - needs a recursive call more temp statement handles perhaps
         buildMDDataDescription(dataset_id, sbr, ii+li, "datacollection" , true) ;
         buildMDDataCollectionLocator(key, sbr, ii+li, null, true) ;
         buildMDRelatedReferance(key, sbr, ii+li, "datacollection", true) ;

         Statement t_t_s = null ;
         ResultSet t_t_r = null ;
         t_t_s=c.createStatement() ;
         String datafile_id = place_holder ;
      
         t_t_r=t_t_s.executeQuery("select datafile_id from datafile_list where dataset_id = '" + dataset_id + "'") ;

         while (t_t_r.next())
         {
            //HERE-need to iterate through all ados
            buildMDAtomicDataObject(key, sbr, ii+li, null, true) ;
         }
         t_t_r.close() ;
         t_t_s.close() ;

         //won't need the recursive call for isis - but maybe in the future
         //buildMDDataCollection(key, sbr, ii+li, "datacollection", true) ;

      }

      t_r.close() ;
      if(nested==true)
      {
         t_s.close() ;
      }

      return ;
   }
   //
   void buildMDAtomicDataObject(String key, StringBuffer sbr, String ii, String type,  boolean nested) throws SQLException
   {
      Connection c = ss.getConnection() ;
      Statement t_s = null ;
      Resultset t_r = null ;

      if(nested == true)
      {
         t_s=c.createStatement() ;
      }
      else
      {
         t_s=s ;
      }
      //
      sbr.append("<AtomicDataObject>\n") ;

      buildMDDataDescription(key, sbr, ii+li, "ado" , true) ;
      buildMDADOLocator(key, sbr, ii+li, "ado" , true) ;
      buildMDRelatedReference(key, sbr, ii+li, "ado" , true) ;
      
      sbr.append("</AtomicDataObject>\n") ;

      if(nested==true)
      {
         t_s.close() ;
      }


      //
      return ;
   }
   //
   void buildMDADOLocator(String key, StringBuffer sbr, String ii, String type,  boolean nested) throws SQLException
   {
      Connection c = ss.getConnection() ;
      Statement t_s = null ;
      Resultset t_r = null ;

      if(nested == true)
      {
         t_s=c.createStatement() ;
      }
      else
      {
         t_s=s ;
      }

      //at the moment use a simeple ADOLocator rather that FileADOL - as do not need the features as yet
      String uri = place_holder ;
      String datafile_format = place_holder ;

      t_r=t_s.executeQuery("select uri, datafile_format from datafile where id='"+key+"'") ;

      if(t_r.next())
      {
         uri=sr.LitWithEnt(xt.makeValid(r.getString("URI") ));
         datafile_format=sr.LitWithEnt(xt.makeValid(r.getString("DATAFILE_FORMAT") ));
      }

      t_r.close() ;
      if(nested==true)
      {
         t_s.close() ;
      }
      //need to put some NS stuff here when doing checking vs the schema 
      sbr.append(ii+"<ADOLocator>\n") ; 
      sbr.append(ii+li+"<Locator>"+uri+"</Locator>\n") ; 
      sbr.append(ii+li+"<Filetype>"+datafile_format+"</Filetype>\n") ; 
      sbr.append(ii+"</ADOLocator>\n") ; 
         
      return ;

   }

   //
   void buildMDDataCollectionLocator(String key, StringBuffer sbr, String ii, String type,  boolean nested) throws SQLException
   {
      Connection c = ss.getConnection() ;
      Statement t_s = null ;
      Resultset t_r = null ;

      if(nested == true)
      {
         t_s=c.createStatement() ;
      }
      else
      {
         t_s=s ;
      }

      String name = place_holder ;
      String locator = place_holder ;
      String id = place_holder ;

      t_r=t_s.executeQuery("select name from dataset where id = '"+key+"'") ;
      if(t_r.next())
      {
         name=sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
      }
      t_r.close() ;

      //can't do this which isis data organisation (cf. phone call Damian Flannery 17/01/2005)
      //files exist in the physical directory structure which do not belong with the logical data organisation 
      //expressed in icat - physically instrument->year->run and logically experiment->dataset->datafile - dataset
      //is a virtual concept and therefore does not have a real physical location - the physical resolution of the
      //dataset will only occur by concatenation of the datafiles within the dataset 
      //t_r=t_s.executeQuery("select uri from datafile where datafile.id in " +
      //                     "(select datafile_id from datafile_list where dataset_id ='" + key + "')") ;
      //if(t_r.next())
      //{
      //   locator = sr.LitWithEnt(xt.makeValid(r.getString("URI") ));
      //}

      //StringBuffer sbt=new StringBuffer(locator) ;

      //sbt.reverse() ;

      //boolean process=true ;
      //while(process == true)
      //{
      //   if(sbr.charAt(i) == "\\" || sbr.charAt(i) == "/")
      //   {
      //      proc=false ;
      //   }
      //   else
      //   {
      //      sbr.deleteCharAt(0) ;
      //   }
      //}

      //locator = (sbt.reverse()).toString() ;
         
      sbr.append(ii+"<DataCollectionLocator>\n") ; 
      abr.append(ii+li+"<DataName>"+name+"</DataName>\n") ;
      //abr.append(ii+li+"<Locator>"+locator+"</Locator>\n") ;
      sbr.append(ii+"</DataCollectionLocator>\n") ; 

      t_r.close() ;
      if(nested==true)
      {
         t_s.close() ;
      }

      return ;

   }

   //

         

   void buildMDDataDescription(String key, StringBuffer sbr, String ii, String type,  boolean nested) throws SQLException
   {
      Connection c = ss.getConnection() ;
      Statement t_s = null ;
      Resultset t_r = null ;

      if(nested == true)
      {
         t_s=c.createStatement() ;
      }
      else
      {
         t_s=s ;
      }

      if(type.compareTo("dataholding")==0)
      {
         String name=place_holder ;

         t_r=t_s.executeQuery("Select title from investigation where id='" + key + "'") ;

         if(t_r.next())
         {
            name = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));
         }

         t_r.close() ;

         sbr.append(ii+"<DataDescription>\n") ;
         sbr.append(ii+li+"<DataName>" + name + "</DataName>\n") ;
         //leave it like this for now - not sure what else we can fill in
         sbr.append(ii+"</DataDescription>\n") ;
      }
      else if (type.compareTo("datacollection")==0) 
      {
         String name=place_holder ;
         String dataset_type=place_holder ;
         String dataset_status=place_holder ;
         String description=place_holder ;

         t_r=t_s.executeQuery("select name, dataset_type, dataset_status, description from dataset where id='"+key+"'") ;

         if(t_r.next())
         {
            name = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
            dataset_type = sr.LitWithEnt(xt.makeValid(r.getString("DATASET_TYPE") ));
            dataset_status = sr.LitWithEnt(xt.makeValid(r.getString("DATASET_STATUS") ));
            description = sr.LitWithEnt(xt.makeValid(r.getString("DESCRIPTION") ));
         }

         t_r.close() ;

         sbr.append(ii+"<DataDescription>\n") ;
         sbr.append(ii+li+"<DataName>"+name+"</DataName>\n") ;
         sbr.append(ii+li+"<Description>"+description+"</Description>\n") ;
         sbr.append(ii+li+"<TypeOfData>"+dataset_type+"</TypeOfData>\n") ;
         sbr.append(ii+li+"<Status>"+dataset_status+"</Status>\n") ;

         buildMDLogicalDescription(key, sbr, ii+li, "datacollection", true) ;
         
         sbr.append(ii+"</DataDescription>\n") ;

      }
      else if (type.compareTo("ado")==0)
      {
         String name = place_holder ;
         String datafile_type = place_holder ;
         String comments = place_holder ;

         t_r=t_s.executeQuery("select name, datafile_type, comments from datafile where id = '"+key+"'") ;

         if(t_r.next())
         {
            name = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
            datafile_type = sr.LitWithEnt(xt.makeValid(r.getString("DATAFILE_TYPE") ));
            comments = sr.LitWithEnt(xt.makeValid(r.getString("COMMENTS") ));
         }
         t_r.close() ;
         
         sbr.append(ii+"<DataDescription>\n") ;
         sbr.append(ii+li+"<DataName>"+name+"</DataName>\n") ;
         sbr.append(ii+li+"<Description>"+comments+"</Description>\n") ;
         sbr.append(ii+li+"<TypeOfData>"+datafile_type+"</TypeOfData>\n") ;
         sbr.append(ii+"<DataDescription>\n") ;

      }
        
      if(nested==true)
      {
         t_s.close() ;
      }

      return ;
   }

   void buildMDDataTopic(String key, StringBuffer sbr, String ii, String type) throws SQLException //will use Keywords & Subjects above
   {

      return ;
   }

   void buildMDLogicalDescription(String key, StringBuffer sbr, String ii, String type, boolean nested) throws SQLException
   {
      Connection c = ss.getConnection() ;
      Statement t_s = null ;
      Resultset t_r = null ;

      if(nested == true)
      {
         t_s=c.createStatement() ;
      }
      else
      {
         t_s=s ;
      }

      if(type.compareTo("datacollection")==0)
      {
         sbr.append(ii+"</LogicalDescription>\n") ;

         buildMDParemeters(key, sbr, ii+li, "datacollection", true) ;
         buildMDTimePeriod(key, sbr, ii+li, "datacollection", true) ;
         buildMDFacilityUsed(key, sbr, ii+li, "datacollection", true) ;

         sbr.append(ii+"</LogicalDescription>\n") ;
      }

      if(nested==true)
      {
         t_s.close() ;
      }


      return ;
   }
   //
   void buildMDParameters(String key, StringBuffer sbr, String ii, String type, boolean nested) throws SQLException
   {

      Connection c = ss.getConnection() ;
      Statement t_s = null ;
      Resultset t_r = null ;

      if(nested == true)
      {
         t_s=c.createStatement() ;
      }
      else
      {
         t_s=s ;
      }

      if(type.compareTo("ado")==0)
      {
         String name=place_holder ;
         String value=place_holder ;
         String units=place_holder ;
         String exponent=place_holder ;
         String range_top=place_holder ;
         String range_bottom=place_holder ;
         String error=place_holder ;

         t_r=t_s.execute("select id, name, value, units, exponent, range_top, range_bottom, error from parameter "+
                         "where parameter.id in (select parameterid from parameter_list where datafileid = '" + key + "')") ;

         //may need to check to see results otherwise might get empty tags - not a serious one

         while(t_r.next())
         {
            id = sr.LitWithEnt(xt.makeValid(r.getString("ID") ));
            name = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
            value = sr.LitWithEnt(xt.makeValid(r.getString("VALUE") ));
            units = sr.LitWithEnt(xt.makeValid(r.getString("UNITS") ));
            exponent = sr.LitWithEnt(xt.makeValid(r.getString("EXPONENT") ));
            range_top = sr.LitWithEnt(xt.makeValid(r.getString("RANGE_TOP") ));
            range_bottom = sr.LitWithEnt(xt.makeValid(r.getString("RANGE_BOTTOM") ));
            error = sr.LitWithEnt(xt.makeValid(r.getString("ERROR") ));

            sbr.append(ii+"<Parameter paramid=\""+id+"\">\n") ;

            sbr.append(ii+li+"<ParamName>"+name+"</ParamName>\n") ;
            sbr.append(ii+li+"<Units>\n") ;
            sbr.append(ii+li+li+"<UnitName>"+units+"</UnitName>\n") ;
            sbr.append(ii+li+li+"<UnitAcronym>"+units+"</UnitAcronym>\n") ;
            sbr.append(ii+li+"</Units>\n") ;
            //range information does not seem to be filled in - need to perhaps clear some of the semantics when
            //instances appear
            sbr.append(ii+li+"<ParamValue>"+value+"</ParamValue>\n") ;
            sbr.append(ii+"</Parameter>\n") ;

         }

      }// all done for ado

      t_r.close() ;
      if(nested==true)
      {
         t_s.close() ;
      }

      return ;
   }

   //

   void buildMDTimePeriod(String key, StringBuffer sbr, String ii, String type, boolean nested) throws SQLException

   {

      return ;
   }

   //

   void buildMDFacilityUsed(String key, StringBuffer sbr, String ii, String type, boolean nested) throws SQLException
   {

      return ;
   }

   //

   void buildMDDataHoldingLocator(String key, StringBuffer sbr, String ii, String type, boolean nested) throws SQLException
   {
      Connection c = ss.getConnection() ;
      Statement t_s = null ;
      Resultset t_r = null ;

      if(nested == true)
      {
         t_s=c.createStatement() ;
      }
      else
      {
         t_s=s ;
      }
 
      String name=place_holder ;

      t_r=t_s.executeQuery("select title from experiment where id ='" + key + "'") ;
      if(t_r.next())
      {
         name = sr.LitWithEnt(xt.makeValid(r.getString("TITLE") ));
      }
      t_r.close() ;

      sbr.append(ii+"<DataHoldingLocator>\n") ;
      sbr.append(ii+li+"<DataName>" + name + "</DataName>\n") ;
      //is there a locator ?
      sbr.append(ii+li+"<Locator>" + "N/A" + "</Locator>\n") ;
      sbr.append(ii+"<DataHoldingLocator>\n") ;

      if(nested==true)
      {
         t_s.close() ;
      }


      return ;
   }


   //all the Misc type information
   void buildMDAccessConditions(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {

      return ;
   }

   void buildMDLegalNotice(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {

      return ;
   }

   void buildMDRelatedPublication(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {

      return ;
   }

   void buildMDOtherRelatedMaterial(String key, StringBuffer sbr, String ii, String type) throws SQLException
   {

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
      sbr.append(li + "</MetadataRecord>\n");

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
         icm.buildMetadataRecord(key, sbr) ;
      }
      catch (SQLExceltion sqle)
      {
          sqle.printStackTrace() ;
      }
   
      System.out.print(sbr.toString) ;

      return  ;
   }


}
