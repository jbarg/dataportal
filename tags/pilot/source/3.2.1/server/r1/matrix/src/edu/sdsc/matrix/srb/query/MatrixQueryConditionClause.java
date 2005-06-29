package edu.sdsc.matrix.srb.query;

/*
 * 
 * @author rmathew
 *
 * Class maintains the details of the "condition" clause for Matrix XQuery
 * For Example the Matrix XQuery clause might have a condition
 * $coll/SystemMetadata/OFFSET <5000
 * In this case the following values will be set for this class
 *  	conditionClause = $coll/SystemMetadata/OFFSET <5000
 *  	conditionVariable = $coll/SystemMetadata/OFFSET
 * 		operator = "="
 * 		conditionValue = 5000
 */
import edu.sdsc.matrix.srb.util.MatrixUtil;

public class MatrixQueryConditionClause
{
	private String conditionClause;		// Condition Clause as String
	private String conditionVariable;   // Variable for condition
	private String operator;			// Operator used for Condition
	private String conditionValue;		// Value checked for condition
	private String matrixCondition;		
	
	/*
	 * Public Constructor
	 */
	public 	MatrixQueryConditionClause()
	{
		
	}
	
	/*
	 * Set the Condition Clause
	 */
	public void setConditionClause(String expr)
	{
		conditionClause = expr;
	}
	
	/*
	 * Return Condition Clause
	 */
	public String getConditionClause()
	{
		return conditionClause;
	}
	
	/*
	 * Set Condition Variable
	 */
	public void setConditionVariable(String var)
	{
		conditionVariable = var;
	}
	
	/*
	 * Return Condition Variable
	 */
	public String getConditionVariable()
	{
		return conditionVariable;
	}
	
	/*
	 * Set Operator
	 */
	public void setOperator(String op)
	{
		operator = op;
	}
	
	/*
	 * Return Operator
	 */
	public String getOperator()
	{
		return operator;
	}
	
	/*
	 * Set Condition Value
	 * This method extracts the "" at the start & end of string
	 */
	public void setConditionValue(String value)
	{
		if(value!=null && value.length()>0)
		{
			if((value.charAt(0)=='\"') && (value.charAt(value.length()-1)=='\"'))
				value = value.substring(1,value.length()-1);
		}
		conditionValue = value;
	}
	
	/*
	 * Return Condition Value
	 */
	public String getConditionValue()
	{
		return conditionValue;
	}
	
	/*
	 * Print the Clause details 
	 */
	public void printClause()
	{
		MatrixUtil.toLog("MatrixQueryConditionClause-printClause():BEGIN", MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("\nMatrixQueryConditionClause-printClause():Condition Clause -"+conditionClause,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryConditionClause-printClause():Condition Variable : "+conditionVariable,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryConditionClause-printClause():Condition Operator : "+operator,MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryConditionClause-printClause():Condition Value : "+conditionValue,MatrixUtil.DEBUG_LOG);		
		MatrixUtil.toLog("MatrixQueryConditionClause-printClause():END", MatrixUtil.DEBUG_LOG);
	}	
	
}