package uk.ac.cclrc.dpal.enums ;


public enum LogicalOperator {
    
    AND("AND"), OR("OR"), NOT("NOT"), XOR("XOR") ;
    
    LogicalOperator(String value) {
        this.value = value;
    }
    
    private final String value;
    
    public String toString() {
        return value;
    }
    
}
