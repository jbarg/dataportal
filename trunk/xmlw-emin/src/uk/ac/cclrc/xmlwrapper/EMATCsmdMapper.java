package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;

import java.sql.* ; //jdbc

import org.apache.log4j.* ;

public class EMATCsmdMapper implements CsmdMapper
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
            temp_str = ("SELECT STUDY_ID FROM " + dbs + "STUDY WHERE STUDY_ID IN " +
                        "(SELECT STUDY_ID FROM " + dbs + "STUDY_TOPICS WHERE TOPIC_ID IN " +
                        "(SELECT TOPIC_ID FROM " + dbs + "TOPICS WHERE lower(TOPIC) LIKE '%" + discipline + "%'))") ;
	    discipline = temp_str ;
	 }

	 if(subject.compareTo("") != 0)
         {
            temp_str = ("SELECT DISTINCT STUDY_ID FROM " + dbs + "STUDY WHERE STUDY_ID IN " +
                        "(SELECT STUDY_ID FROM " + dbs + "INVESTIGATION WHERE INVESTIGATION_ID IN " +
                        "(SELECT INVESTIFGATION_ID FROM " + dbs + "DATA WHERE DATA_ID IN " +
                        "(SELECT DATA_ID FROM " + dbs + "PARAMETERS WHERE lower(NAME) LIKE '%" + subject + "%')))") ;
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

   // contact_type can be Investigator or DataManager

   void buildMDContact(String key, StringBuffer sbr, int initial_indent, String contact_type) throws SQLException
   {
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

      String ii = indentToStr(initial_indent) ;
      String ti = indentToStr(indent) ;

      //retrieve the database name
      String dbs = ss.getDbs() ;

      r = s.executeQuery("select institution.name, institution.institution_id address_line_1, address_line_2, town, region, postcode, country, " +
                         "title, forename, surname, other_initials, telephone, email, fax from " + dbs +
                         "person, " + dbs + "institution where institution.institution_id = person.institution_id and institution.institution_id in " +
                         "(select institution_id from study_institution where study_id in " +
                         "(select study_id from " + dbs + "study_person where study_id='" + key + "' and role like '%" + contact_type + "%')) " ) ;

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
         institute_name = sr.LitWithEnt(xt.makeValid(r.getString("INSTITUTION.NAME") ));
         institute_id = r.getInt("INSTITUTION_ID") ;
         i_street = sr.LitWithEnt(xt.makeValid(r.getString("ADDRESS_LINE_1") ));
         i_place = sr.LitWithEnt(xt.makeValid(r.getString("TOWN") ));
         i_pobox_postal_code = sr.LitWithEnt(xt.makeValid(r.getString("POSTCODE") ));
         i_country = sr.LitWithEnt(xt.makeValid(r.getString("COUNTRY") ));
         last_name = sr.LitWithEnt(xt.makeValid(r.getString("SURNAME") ));
         first_name = sr.LitWithEnt(xt.makeValid(r.getString("FORENAME") ));
         second_name = sr.LitWithEnt(xt.makeValid(r.getString("MIDDLE_INITIALS") ));
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

      //all elements of the study need extracting as will be used later ;
      String study_id = place_holder ;
      String study_name = place_holder ;
      String funding = place_holder ;
      String start_date = place_holder ;
      String end_date = place_holder ;
      String access_conditions = place_holder ;
      String purpose = place_holder ;
      String status = place_holder ;
      String resources = place_holder ;
      String notes = place_holder ;
      //String related_study = place_holder ;
      //String relation_type = place_holder ;

      r = s.executeQuery("select study_id, name, funding, start_date, end_date, access_conditions, " +
                         "purpose, status, resources, notes, related_study, relation_type from " + 
                          dbs + "study where study_id = '" + key + "'") ;
      if(r.next())
      {
	 r.getRow() ;
	 study_id = sr.LitWithEnt(xt.makeValid(r.getString("STUDY_ID") ));
	 study_name = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
	 funding = sr.LitWithEnt(xt.makeValid(r.getString("FUNDING") ));
	 start_date = sr.LitWithEnt(xt.makeValid(r.getString("START_DATE") ));
	 end_date = sr.LitWithEnt(xt.makeValid(r.getString("END_DATE") ));
	 access_conditions = sr.LitWithEnt(xt.makeValid(r.getString("ACCESS_CONDITIONS") ));
	 purpose = sr.LitWithEnt(xt.makeValid(r.getString("PURPOSE") ));
	 status = sr.LitWithEnt(xt.makeValid(r.getString("STATUS") ));
	 resources = sr.LitWithEnt(xt.makeValid(r.getString("RESOURCES") ));
	 notes = sr.LitWithEnt(xt.makeValid(r.getString("NOTES") ));
	 //related_study = sr.LitWithEnt(xt.makeValid(r.getString("RELATED_STUDY") ));
	 //relation_type = sr.LitWithEnt(xt.makeValid(r.getString("RELATION_TYPE") ));
      }

      sbr.append("   <MetadataRecord metadataID=\"" + ss.getWrapperName() + "-" + key + "\">\n");

      r.close() ;

      // lookup Topic - at the moment only one leaf topic per study perhaps something like:
      // gptopic/ptopic/topic, gptopic/ptopic/topic is needed

      sbr.append("      <Topic>\n") ;

      r = s.executeQuery("select topic from " + dbs + "topic where topic_id in" +
                        "(select distinct topic_id  from " + dbs + "study_topic where study_id='" + key + "')" ) ;
     

      String topic_name = place_holder ;

      if(r.next())
      {
         r.getRow() ;
         topic_name = sr.LitWithEnt(xt.makeValid(r.getString("TOPIC") ));
      }

      sbr.append("         <Discipline>" + topic_name + "</Discipline>\n") ;
     

      // following should the resultset handle
      r.close() ;

      //r = s.executeQuery("SELECT KEYWORD FROM " + dbs + "KEYWORD WHERE KEYWORD_ID IN " +
      //                  "(SELECT DISTINCT KEYWORD_ID FROM " + dbs + "STUDY_KEYWORD WHERE " +
      //                  " STUDY_ID='" + key + "' )") ;

      //as there can be multiple ones - i.e. Keywords - 

      String subject = place_holder ;

      //while(r.next())
      //{
      //   subject = sr.LitWithEnt(xt.makeValid(r.getString("KEYWORD") ));
      //   sbr.append("         <Subject>" + sr.LitWithEnt(xt.makeValid(subject)) + "</Subject>\n") ;
      //}

      // duplication needed as there maybe multiple values
      if(subject.compareTo(place_holder) == 0 )
      {
         sbr.append("         <Subject>" + place_holder + "</Subject>\n") ;
      }

      //r.close() ;

      sbr.append("      </Topic>\n") ;

      // end of Topic

      // start of Investigation (rxperiment)

      sbr.append("      <Experiment>\n") ;

      // may need to link this with role in study_person as we have the concept of
      //The investigator could be one of: investigator, inhouse or originator
      //The data manager could be one of: distributor or owner

      
      sbr.append("         <StudyName>" + study_name + "</StudyName>\n" ) ;

      //finding institution information
      String institution_id = place_holder ;
      String institution_name = place_holder ;

      r = s.executeQuery("SELECT INSTITUTION_ID, NAME FROM " + dbs + "INSTITUTION WHERE INSTITUTION_ID IN " +
                         "(SELECT INSTITUTION_ID FROM " + dbs + "STUDY_INSTITUTION WHERE STUDY_ID='" + key + "' )") ;
      while(r.next())
      {
         institution_id = sr.LitWithEnt(xt.makeValid(r.getString("INSTITUTION_ID") ));
         institution_name = sr.LitWithEnt(xt.makeValid(r.getString("NAME") ));
      }
     

      if(study_name.compareTo(place_holder) != 0 )
      {
         if(institution_name.compareTo(place_holder) != 0)
         {
            sbr.append("         <StudyID studyid=\"" + study_name + "\" institutionref=\"I" + institution_id + "\"/>\n") ;
         }
         else
         {
            sbr.append("         <StudyID studyid=\"" + study_name + "\"/>\n") ;
         }
      }
      else
      {
         sbr.append("         <StudyID studyid=\"" + place_holder + "\" institutionref=\"" + place_holder + "\"/>\n") ;
      }

      r.close() ;

      //Investigator
      buildMDContact(key, sbr, 3*indent, "Investigator") ;

      // onto the StudyInformation
      // may have to fill a section in with place holders

      sbr.append("          <StudyInformation>\n") ;
      sbr.append("             <Funding>" + funding + "</Funding>\n") ;
      sbr.append("             <TimePeriod>\n") ;
      sbr.append("                <StartDate>\n") ;
      sbr.append("                   <Date>"+ start_date + "</Date>\n") ;
      sbr.append("                </StartDate>\n") ;
      sbr.append("             </TimePeriod>\n") ;
      sbr.append("             <Purpose>\n") ;
      sbr.append("                <Abstract>" + purpose + "</Abstract>\n") ;
      sbr.append("             </Purpose>\n") ;
      sbr.append("             <StudyStatus>" + status + "</StudyStatus>\n") ;
      sbr.append("             <Resources>"+ resources + "</Resources>\n") ;
      sbr.append("          </StudyInformation>\n") ;

      //DataManager 
       buildMDContact(key, sbr, 3*indent, "DataManager") ;

      //Instrument

      sbr.append("          <Instrument>" + place_holder + "</Instrument>\n") ;

      appendConditionsForEntry(key, 3*indent, "experiment",  sbr) ; // 9 = 3 spaces per level - and 3 level 3x3

 
      // end of measurement now measurement
      sbr.append("       </Experiment>\n") ;

      //access_conditions

      sbr.append("       <AccessConditions>" + access_conditions + "</AccessConditions>\n") ;

      //DataHolding

      //Each experiment for this Study will be the name of a Data Holding
      // really need a loop for this in case there are more that one experiment - save re-implementation
      // later

      List experiments = new LinkedList() ;
      List datasets = new LinkedList() ;
      List datafiles = new LinkedList() ;
      
      //as the dtd and this version of the model only support one investigation for one study
      // the experiment in this case is the study
        
      experiments.add(key) ;


      //and now for each investigation

      Iterator e = experiments.iterator() ;

      String exp_id = place_holder ;
      
      while(e.hasNext())
      {
	 exp_id = (String) e.next() ;

	 String typeofdata = "EXPERIMENT" ; //could replace with a search

         sbr.append("       <DataHolding dataid=\"" + study_id + "\">\n") ;
	 sbr.append("          <DataName>" + study_name + "</DataName>\n") ;
	 sbr.append("          <TypeOfData>" + typeofdata + "</TypeOfData>\n") ;
	 sbr.append("          <Status>" + status + "</Status>\n") ;

	 // fill in the dataholdinglocation details - save uneccessary calls later
	 //////

	 sbr_dhl.append("      <DataHoldingLocator dataidref=\"" + study_id + "\">\n") ;
         sbr_dhl.append("         <DataName>" + study_name + "</DataName>\n") ;
	 sbr_dhl.append("         <Locator type=\"absolute\">\n") ;
         sbr_dhl.append("            <URL>" + place_holder + "</URL>\n") ;
         sbr_dhl.append("            <DataSourceAccess>" + access_conditions + "</DataSourceAccess>\n") ;
	 sbr_dhl.append("         </Locator>\n") ;
	 
         //////
	 // now select the datasets associated with this experiment

	 log.debug("STUDY_ID at this point is: " + study_id) ;

         r = s.executeQuery("SELECT DATASET_ID FROM " + dbs + "DATASET WHERE STUDY_ID = '" + exp_id + "'") ;

         while(r.next())
         {
	    datasets.add(Integer.toString(r.getInt("DATASET_ID"))) ;
         }
         r.close() ;

         //and now for each dataset
         Iterator g = datasets.iterator() ;
         String dataset_id = place_holder ;

         //create object references outside of the loop
         String dataset_name = place_holder ;
         String dataset_description = place_holder ;
         String dataset_facility = place_holder ;
         String dataset_uri = place_holder ;
         String access_method = place_holder ;
         String dataset_start_date = place_holder ;
         String dataset_end_date = place_holder ;
      
         while(g.hasNext())
         {
	    dataset_id = (String) g.next() ;
	    typeofdata = "DATA_SET" ; //could replace with a search

	    r = s.executeQuery("SELECT DATASET_NAME, DESCRIPTION, FACILITY_USED, URI, " +
                               "ACCESS_METHOD, START_DATE, END_DATE FROM " + dbs + "DATASET WHERE DATASET_ID  = '" + dataset_id + "'") ;
	    if(r.next())
	    {
	       dataset_name = sr.LitWithEnt(xt.makeValid(r.getString("DATASET_NAME") ));
	       dataset_description = sr.LitWithEnt(xt.makeValid(r.getString("DESCRIPTION") ));
	       dataset_facility = sr.LitWithEnt(xt.makeValid(r.getString("FACILITY_USED") ));
	       dataset_uri = sr.LitWithEnt(xt.makeValid(r.getString("URI") ));
	       access_method = sr.LitWithEnt(xt.makeValid(r.getString("ACCESS_METHOD") ));
	       dataset_start_date = sr.LitWithEnt(xt.makeValid(r.getString("START_DATE") ));
	       dataset_end_date = sr.LitWithEnt(xt.makeValid(r.getString("END_DATE") ));
	    }
	    r.close() ;
	 
            sbr.append("          <DataSet dataid=\"" + dataset_id + "\">\n") ;
	    sbr.append("             <DataName>" + dataset_name + "</DataName>\n") ;
	    sbr.append("             <TypeOfData>" + typeofdata + "</TypeOfData>\n") ;
	    sbr.append("             <Status>" + place_holder + "</Status>\n") ;
	    sbr.append("             <LogicalDescription>\n") ;
            appendConditionsForEntry(dataset_id, 5*indent, "dataset", sbr) ; // 5 levels in
	    sbr.append("             </LogicalDescription>\n") ;

            //a fair place to populate the DataHoldingLocator
            sbr_dhl.append("         <DataSetLocator dataidref=\"" + dataset_id + "\">\n") ;
            sbr_dhl.append("            <DataName>" + dataset_name + "</DataName>\n") ;
            sbr_dhl.append("            <Locator type=\"absolute\">\n") ;
            sbr_dhl.append("               <URL>" + dataset_uri + "</URL>\n") ;
            sbr_dhl.append("            </Locator>\n") ;
            sbr_dhl.append("         </DataSetLocator>\n") ;


	    // now select the data_files associated with this data_set
            r = s.executeQuery("SELECT DATA_ID FROM " + dbs + "DATA WHERE DATASET_ID = '" + dataset_id + "'") ;
            while(r.next())
            {
	       r.getRow() ;
	       datafiles.add(Integer.toString(r.getInt("DATA_ID"))) ;
            }
            r.close() ;

            //and now for each entry
            Iterator ro = datafiles.iterator() ;
            String data_id = place_holder ;
      
	    //put in the child references for files

            //object references here
            String data_name = place_holder ;
            String data_format = place_holder ;
            //status = place_holder ;
            //String data_description = place_holder ;
            //String producing_facility = place_holder ;
            String data_uri = place_holder ;
            access_method = place_holder ;
            String data_start_date = place_holder ;
            String data_end_date = place_holder ;

            while(ro.hasNext())
            {
	       data_id = (String) ro.next() ;
	       
	       r = s.executeQuery("SELECT DATA_NAME, DATA_FORMAT, DESCRIPTION, " +
                                  "URI, ACCESS_METHOD, START_DATE, END_DATE FROM " + dbs + "DATA WHERE DATA_ID = '" + data_id + "'") ;
	       if(r.next())
	       {
		  data_name = sr.LitWithEnt(xt.makeValid(r.getString("DATA_NAME") ));
		  data_format = sr.LitWithEnt(xt.makeValid(r.getString("DATA_FORMAT") ));
		  //most of these commented out due to null data causing null references
		  //this needs addressing and the null value replacing with a place_holder
		  //status = sr.LitWithEnt(xt.makeValid(r.getString("STATUS") ));
		  //data_description = sr.LitWithEnt(xt.makeValid(r.getString("DESCRIPTION") ));
		  //producing_facility = sr.LitWithEnt(xt.makeValid(r.getString("PRODUCING_FACILITY") ));
		  data_uri = sr.LitWithEnt(xt.makeValid(r.getString("URI") ));
		  access_method = sr.LitWithEnt(xt.makeValid(r.getString("ACCESS_METHOD") ));
		  data_start_date = sr.LitWithEnt(xt.makeValid(r.getString("START_DATE") ));
		  data_end_date = sr.LitWithEnt(xt.makeValid(r.getString("END_DATE") ));
	       }
	       r.close() ;

	       sbr.append("            <File dataid=\"" + data_id + "\">\n") ;
	       sbr.append("               <DataName>" + data_name + "</DataName>\n") ;
	       sbr.append("               <URI>" + data_name + "</URI>\n") ;
	       sbr.append("            </File>\n") ;

	       //add dataholdinglocation info to sbr_dhl
	       //////
	       sbr_dhl_files.append("         <FileLocator dataidref=\"" + data_id + "\">\n") ;
               sbr_dhl_files.append("            <URI>" + data_name + "</URI>\n") ;
	       sbr_dhl_files.append("            <Locator type=\"absolute\">\n") ;
               sbr_dhl_files.append("               <URL>" + data_uri + "</URL>\n") ;
	       sbr_dhl_files.append("            </Locator>\n") ;
	       sbr_dhl_files.append("         </FileLocator>\n") ;
	       //////
	    }


	    sbr.append("            </DataSet>\n") ;

	 } //filled in the data sets

         sbr.append("      </DataHolding>\n") ;

      } //filled in the experiments

      // - following moved to experiments bit so get the id attribute
      //sbr.append("      <DataHoldingLocator>\n") ;

      //add collected holding location data
      sbr.append(sbr_dhl.toString()) ;
      //as FileLocators needed after DataSetLocatos
      sbr.append(sbr_dhl_files.toString()) ; 
      sbr.append("      </DataHoldingLocator>\n") ;
      //
      

      sbr.append("   </MetadataRecord>\n") ;

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


      String data_in_list = place_holder ;

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

      return ;
   }

   //
   //
   //


}
