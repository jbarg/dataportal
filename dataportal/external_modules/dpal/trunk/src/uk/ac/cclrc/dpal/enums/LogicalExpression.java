package uk.ac.cclrc.dpal.enums ;


public enum LogicalExpression {
    
    AND("AND"), OR("OR"), NOT("NOT"), XOR("XOR") ;
    
    LogicalExpression(String value) {
        this.value = value;
    }
    
    private final String value;
    
    public String toString() {
        return value;
    }
    
}
