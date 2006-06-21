package uk.ac.cclrc.webinterface.bean;

public class SearchBean {

    private keyword = null ;
    
    public SearchBean() {
    }

    public setKeyword(String keyword){
       this.keyword=keyword ;
    }

    public String getKeyword() {
       return keyword ;
    }
    
}
