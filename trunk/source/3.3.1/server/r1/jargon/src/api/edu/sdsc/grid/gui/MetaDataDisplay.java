/*
 * MetaDataDisplay.java
 *
 * Created on July 16, 2004, 11:06 AM
 */

package edu.sdsc.grid.gui;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.local.*;

import java.io.*;
import java.util.*;//EventObject
import javax.swing.*;
import javax.swing.table.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;



import javax.swing.plaf.basic.*;

/**
 *
 * @author  iktome
 */
/*TODO public*/ class MetaDataDisplay extends javax.swing.JPanel
{
//-----------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------
	/**
	 * The cells of tables were too small for the text.
	 */
	static final int TEXT_PADDING = 3;

	//I don't know maybe optimizable
	/**
	 * Number of rows looked at when sizing the display of the MetaDataTable
	 */
	static /*final*/ int TEST_ROWS = 5;


//-----------------------------------------------------------------------
// Fields
//-----------------------------------------------------------------------
	GeneralFileSystem fileSystem;

	/**
	 * Holds query results to be displayed.
	 */
	private MetaDataRecordList[] rl;

	//TODO maybe have?
	/**
	 * Holds MetaDataTable to be displayed.
	 */
	private MetaDataTable table;


	//just going to save this file(filesystem?) in case we need it later
	//to do more queries
	private GeneralFile queryObj;

	private MetaDataCondition[] conditions;

	private MetaDataSelect[] selects;


	// Swing variables
	private JComboBox jComboBox1;
	private JPanel mainPanel;
	private JPanel jPanel2;
	private JScrollPane jScrollPane1;
	private JScrollPane jScrollPane2;
	private JSeparator jSeparator1;
	private JTree jTree1;



	private boolean action = false;
	private javax.swing.JButton jButton1;
	private javax.swing.JList jList1;



//-----------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------
	/**
	 * Displays <code>recordList</code>
	 *
	 */
	public MetaDataDisplay( MetaDataRecordList recordList )
	{
		rl = new MetaDataRecordList[1];
		rl[0] = recordList;

		initComponents();
	}

	/** Creates new form MetaDataDisplay */
	public MetaDataDisplay( MetaDataRecordList recordList[] )
	{
		rl = recordList;

		initComponents();
	}


	/** Creates new form MetaDataDisplay */
	MetaDataDisplay( GeneralFile file, MetaDataRecordList recordList )
	{
		queryObj = file;
		rl = new MetaDataRecordList[1];
		rl[0] = recordList;

		initComponents();
	}

	/** Creates new form MetaDataDisplay */
	MetaDataDisplay( GeneralFile file, MetaDataRecordList recordList[] )
	{
		queryObj = file;
		rl = recordList;

		initComponents();
	}


	/** Creates new form MetaDataDisplay */
	public MetaDataDisplay( MetaDataTable table )
	{
		this.table = table;

		initComponents();
	}


//maybe this whole action thing won't work.
//Like what actions are a llowable and how do I allow them?
//they can choose among my list?
//they can use .reflect to give me methods to run?
	/** Creates new form MetaDataDisplay */
	MetaDataDisplay( GeneralFile file,
		MetaDataRecordList rl, String[] actionList )
	{
		queryObj = file;
		this.rl = new MetaDataRecordList[1];
		this.rl[0] = rl;

		action = true;

		initComponents();
	}

//maybe this whole action thing won't work.
	/** Creates new form MetaDataDisplay */
	MetaDataDisplay( GeneralFile file,
		MetaDataRecordList rl[], String[] actionList )
	{
		queryObj = file;
		this.rl = rl;

		action = true;

		initComponents();
	}

	/*
	MetaDataDisplay( GeneralFileSystem fileSystem,
		MetaDataCondition conditions[] )
	{
		queryObj = fileSystem;
		this.conditions = conditions;

		initComponents();
	}

	/** Creates new form MetaDataDisplay *
	MetaDataDisplay( GeneralFileSystem fileSystem,
		MetaDataCondition conditions[], MetaDataSelect selects[] )
		throws IOException
	{
		queryObj = fileSystem;
		this.conditions = conditions;
		this.selects = selects;

		rl = fileSystem.query( conditions, selects );

		initComponents();
	}
*/
	/** Creates new form MetaDataDisplay */
	MetaDataDisplay( GeneralFile file, MetaDataSelect selects[] )
		throws IOException
	{
		queryObj = file;
		this.selects = selects;

		rl = file.query( selects );

		initComponents();
	}

	/** Creates new form MetaDataDisplay */
	MetaDataDisplay( GeneralFile file,
		MetaDataCondition conditions[], MetaDataSelect selects[] )
		throws IOException
	{
		queryObj = file;
		this.conditions = conditions;
		this.selects = selects;

		rl = file.query( conditions, selects );

		initComponents();
	}



	/**
	 * This method is called from within the constructor to
	 * initialize the display.
	 */
	private void initComponents()
	{
		setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

		//if constructed with full query or query results.
		//display results table
		if (rl != null) {
			add(createResultsComponent(rl), new java.awt.GridBagConstraints());
		}
		else if (table != null) {
			add(MetaDataTableDisplay.createMetaDataTable( table ), new java.awt.GridBagConstraints());
		}


		//if constructed with condition arguments
		//display the selects "selecting" panel
		if (conditions != null) {
			jComboBox1 = new JComboBox();
			jPanel2 = new JPanel();
			jSeparator1 = new JSeparator();
			jScrollPane1 = new JScrollPane();

			mainPanel = new JPanel();
			mainPanel.add(jComboBox1);
			mainPanel.add(jPanel2);
			mainPanel.add(jSeparator1);
			mainPanel.add(jScrollPane1);
			add(mainPanel, new java.awt.GridBagConstraints());
		}



		//if action panel
		if (action)	{
			jComboBox1 = new JComboBox();
			jSeparator1 = new JSeparator();
			jPanel2 = new JPanel();
			mainPanel = new JPanel();
			mainPanel.add(jComboBox1);
			mainPanel.add(jSeparator1);
			mainPanel.add(jPanel2);
			add(mainPanel, new java.awt.GridBagConstraints());

			///*
			jList1 = new javax.swing.JList();
			jButton1 = new javax.swing.JButton();

			add(jList1);

			jButton1.setMinimumSize(new java.awt.Dimension(40, 25));
			add(jButton1);
			//*/
		}
	}
//TODO move up if good.
//need better name?
boolean keyValue = true;
void setKeyValue( boolean keyValuePair ) { keyValue = keyValuePair; }
boolean getKeyValue() { return keyValue; }

	/**
	 *
	 */
	JTable createResultsTable( MetaDataRecordList[] rl )
	{
		return createResultsTable(rl, 0);
	}

	/**
	 *
	 */
//TODO multiple recordlists with MetaDataTables that have varying # of rows
	JTable createResultsTable( MetaDataRecordList[] rl, int scrollValue )
	{
		if (rl == null) return null;

		int definableRows=0;
		int column=0;
		int fieldsLength = rl[0].getFieldCount();
		int recordListLength = rl.length;
		String[] fields = new String[fieldsLength];
		Object[][] data = new Object[recordListLength][fieldsLength];

		JTable resultsTable = null;

int tempFieldsLength = 0;

		//Setup for the Object[][]
		for (int i=0;i<fieldsLength;i++) {
				switch (rl[0].getFieldType(i)) {
					case MetaDataField.TABLE:
if (keyValue) {
//NOTE:*********************
//assumes all results have the same number of def_metadata
//also, only displays first two values of a row
	MetaDataTable subTable = rl[0].getTableValue(i);
	//have to turn rows into columns of key-values
	tempFieldsLength += fieldsLength + subTable.getRowCount();
	String[] tempFields = new String[tempFieldsLength-1];
	System.arraycopy( fields, 0, tempFields, 0, fieldsLength );
//System.out.println("fieldsLength "+fieldsLength);
//System.out.println("tempFieldsLength "+tempFieldsLength);
//System.out.println("subTable.getRowCount() "+subTable.getRowCount());
	for (int j=fieldsLength;j<tempFieldsLength;j++) {
//System.out.println("j "+j);
		tempFields[j-1] = subTable.getStringValue( j-fieldsLength, 0 );
	}
	fields = tempFields;
}
else {
	fields[i] = rl[0].getFieldName(i);
}

						break;
					default:
						fields[i] = rl[0].getFieldName(i);
						break;
				};
		}
		for (int i=0;i<recordListLength;i++) {
			for (int j=0;j<fieldsLength;j++) {
if (rl[i].getFieldCount() <= j) { }
else {
				switch (rl[i].getFieldType(j)) {
					case MetaDataField.TABLE:
if (keyValue) {
//NOTE:*********************
//assumes all results have the same number of def_metadata
//also, only displays first two values of a row
MetaDataTable subTable = rl[i].getTableValue(j);
	//have to turn rows into columns of key-values
	//tempFieldsLength += fieldsLength + subTable.getRowCount();
	Object[] tempData = new Object[tempFieldsLength];
	System.arraycopy( data[i], 0, tempData, 0, fieldsLength );
	for (int k=fieldsLength;k<tempFieldsLength;k++) {
		tempData[k-1] = subTable.getStringValue( k-fieldsLength, 1 );
	}
	data[i] = tempData;
}
else {
						data[i][j] = new MetaDataTableDisplay( rl[i].getTableValue(j) );
data[i][j] = ((MetaDataTableDisplay) data[i][j]).createMetaDataTable( rl[i].getTableValue(j) );
//						definableRows = definableRows+((MetaDataTableDisplay)data[i][j]).getRows();
definableRows = definableRows+rl[i].getTableValue(j).getRowCount();
}
						break;
					case MetaDataField.STRING:
					default:
						data[i][j] = rl[i].getStringValue(j);
						break;
				}
}
			}
		}

		//create the table of metadata
		if (definableRows > 0) {
			resultsTable = displayMetaData(data, fields, rl);
		}
		else {
//resultsTable = displayMetaData(data, fields, rl);
			resultsTable = new JTable(new MetaDataTableModel( data, fields )) {
				public TableCellEditor getDefaultEditor(Class columnClass) {
					return super.getDefaultEditor(columnClass);
				}
			};
TableCellRenderer defaultRenderer =
	resultsTable.getDefaultRenderer(String.class);
resultsTable.setDefaultRenderer(String.class,
	 new MetaDataTableCellRenderer(defaultRenderer));
TableCellEditor defaultEditor =
	resultsTable.getDefaultEditor(String.class);
resultsTable.setDefaultEditor(String.class,
	new MetaDataTableCellEditor(defaultEditor) );

		}

		//don't want to double count the first row and header
		if (definableRows > 1) {
			definableRows-=2;

			//creates proper column sizes and returns total table width
			int columnWidth = initColumnSizes(resultsTable);
			//total height = (rows + header + MetaDataTable_Rows ) * font height
//TODO this plus one is needed because of the header when displaying in the JargonTree, but not good when just showing a MetaDataRecordlist on its own.
			int columnHeight = (data.length+1+definableRows)*fontPixelHeight;
			resultsTable.setPreferredScrollableViewportSize(
				new Dimension( columnWidth,	columnHeight*2 ));

			resultsTable.setRowHeight(columnHeight);
		}
		else {
			//creates proper column sizes and returns total table width
			int columnWidth = initColumnSizes(resultsTable);
			//total height = (rows + header + MetaDataTable_Rows ) * font height
			int columnHeight = (data.length)*fontPixelHeight;
			resultsTable.setPreferredScrollableViewportSize(
				new Dimension( columnWidth,	columnHeight ));

			resultsTable.setRowHeight(columnHeight);
		}

		return resultsTable;
	}



	/**
	 *
	 */
	JComponent createResultsComponent( MetaDataRecordList[] rl )
	{
		return createResultsComponent(rl, 0);
	}

	/**
	 *
	 */
	JComponent createResultsComponent(
		MetaDataRecordList[] rl, int scrollValue )
	{
		if (rl == null) return null;
		if (scrollValue < 0) {
			return createResultsTable( rl, scrollValue );
		}

		JTable resultsTable = null;
		JScrollPane jScrollPane = new JScrollPane();

		JScrollBar verticalScrollBar = jScrollPane.createVerticalScrollBar();

		resultsTable = createResultsTable( rl, scrollValue );

		//insure the scrollbar responds properly
		verticalScrollBar.setValue(scrollValue);
		if (!rl[0].isQueryComplete()) {
			//query still has more (undisplayed) results.
//TODO temp commented out			verticalScrollBar.addAdjustmentListener(
//				new MoreQueryAdjustmentListener(verticalScrollBar, rl, jScrollPane));
		}
		jScrollPane.setVerticalScrollBar(verticalScrollBar);
		jScrollPane.setViewportView(resultsTable);

		Dimension d = ((Component)resultsTable).getPreferredSize();
		d.setSize(d.width, d.height+fontPixelHeight+TEXT_PADDING);
		((JComponent)jScrollPane).setPreferredSize(d);


resultsTable.getModel().addTableModelListener(new TableModelListener() {
	public void tableChanged(TableModelEvent e) {
			int row = e.getFirstRow();
			int column = e.getColumn();
			TableModel model = (TableModel)e.getSource();
			String columnName = model.getColumnName(column);
			Object data = model.getValueAt(row, column);


//System.out.println("####### "+data);
	}
});

/*
((JComponent)resultsTable).setBackground(Color.BLUE);

((JComponent)jScrollPane).setBackground(Color.GREEN);

((JComponent)verticalScrollBar).setBackground(Color.YELLOW);
*/

		return jScrollPane;
	}


	/**
	 * This method sets good column sizes for the table.
	 * But it doesn't really try that hard.
	 * It only compares the header and first row.
	 *
	 * @return total column widths
	 */
	static int initColumnSizes(JTable table)
	{
		TableModel model = table.getModel();

		TableColumn column = null;
		int columnCount = table.getColumnCount();

		Component comp = null;

		Object value;
		int headerWidth = 0;
		int cellWidth = 0;
		int totalWidth = TEXT_PADDING;

		TableCellRenderer headerRenderer =
				table.getTableHeader().getDefaultRenderer();

		for (int i = 0; i < columnCount; i++) {
			column = table.getColumnModel().getColumn(i);

			comp = headerRenderer.getTableCellRendererComponent(
				null, column.getHeaderValue(), false, false, 0, 0);
			setupComp(comp);


			//header size + some padding for readability.
			headerWidth = comp.getPreferredSize().width+TEXT_PADDING;
int oldWidth = 0;
int testRows = Math.min( TEST_ROWS, table.getRowCount() );
for (int j=0;j<testRows;j++) {
//			value = table.getValueAt( 0, i );
value = table.getValueAt( j, i );
oldWidth = cellWidth;
//System.out.println("metadatatable value "+value);
			if (value == null) {
				cellWidth = 1;
			}
			else if (value instanceof String) {
				cellWidth = getCellWidth((String) value.toString());
			}
			else if (value instanceof Integer) {
				cellWidth = getCellWidth(value.toString());
			}
			else if (value instanceof Long) {
				cellWidth = getCellWidth(value.toString());
			}
			else if (value instanceof Float) {
				cellWidth = getCellWidth(value.toString());
			}
			else if (value instanceof java.util.Date) {
				cellWidth = getCellWidth(value.toString());
			}
/*never occurs
			else if (value instanceof MetaDataTable) {
				MetaDataTable metaDataTable = (MetaDataTable) value;
				for (int j=0;j<metaDataTable.getColumnCount();j++) {
					int tableCellwidth = getCellWidth(metaDataTable.getStringValue(0,j));

					//checking just the first row wasn't good enough
					for (int ii=1;i<metaDataTable.getRowCount();i++) {
						tableCellwidth = Math.max( tableCellwidth,
							getCellWidth(metaDataTable.getStringValue(i,j)) );
					}
					cellWidth += tableCellwidth;
				}
//System.out.println("metadatatable cellWidth "+cellWidth);
			}
*/
			else if (value instanceof JTable) {
//System.out.println("Jtable cellWidth "+cellWidth);
				cellWidth = initColumnSizes((JTable)value);
			}
cellWidth = Math.max(cellWidth, oldWidth);
//System.out.println("metadatatable cellWidth "+cellWidth+"     i "+i+"   j "+j);
}
//System.out.println("value "+value);
//System.out.println("value class "+value.getClass());
//System.out.println("before cellWidth "+cellWidth);
			//pick the bigger of the two
			cellWidth = Math.max(headerWidth, cellWidth);

//System.out.println("after cellWidth "+cellWidth);
			column.setPreferredWidth(cellWidth);

			totalWidth += cellWidth;

cellWidth=0;
		}
//System.out.println("totalWidth "+totalWidth);
		return totalWidth;
	}


static int fontPixelHeight = -1;
static Font font;
static FontMetrics fontMetrics;
	private static void setupComp(Component comp)
	{
		if (fontMetrics==null) {
			font = comp.getFont();

			if (font==null) {
				font = new Font("SansSerif", Font.PLAIN, 12);
				comp.setFont(font);
			}
			fontMetrics = comp.getFontMetrics(font);
		}
	}

	private static int getCellWidth(String value)
	{
		int w =  fontMetrics.stringWidth(value);

		if (fontPixelHeight <= 0) {
			fontPixelHeight = fontMetrics.getHeight();
		}
		return w + TEXT_PADDING;
	}


//-----------------------------------------------------------------------
// Listeners
//-----------------------------------------------------------------------
	class MoreQueryAdjustmentListener implements AdjustmentListener
	{
		/**
		 * The component adjusted
		 */
		JScrollBar jScrollBar;

		/**
		 * Needed if the pane is to be further extended.
		 */
		JScrollPane jScrollPane;

		/**
		 * Needed if more data left to display
		 */
		MetaDataRecordList[] rl;

		/**
		 *
		 */
		MoreQueryAdjustmentListener(JScrollBar jScrollBar)
		{
			this.jScrollBar = jScrollBar;
		}

		/**
		 * Constructor used for when the result set has more data that was not
		 * displayed in total when initComponents() was executed.
		 */
		MoreQueryAdjustmentListener(JScrollBar jScrollBar,
			MetaDataRecordList[] rl, JScrollPane jScrollPane )
		{
			this.jScrollBar = jScrollBar;
			this.rl = rl;
			this.jScrollPane = jScrollPane;
		}


		/**
		 * Dynamically add more data to the MetaDataRecordList[] table display.
		 */
		/* protected */ void expandTable(int scrollValue)
			throws IOException
		{
			//check the last recordlist to see if there are more to follow
			if (rl[rl.length-1].isQueryComplete()) return;

//TODO sigh, this way is kinda dumb, why can't I add rows to a JTable?
			//create one list for a new table
			MetaDataRecordList oldRL[] = rl;
			MetaDataRecordList newRL[] = rl[0].getMoreResults();
			int newLength = newRL.length, oldLength = oldRL.length;
			rl = new MetaDataRecordList[oldLength + newLength];

			System.arraycopy( oldRL, 0, rl, 0, oldLength );
			System.arraycopy( newRL, 0, rl, oldLength, newLength );

//TODO extra dumb here
			//jScrollPane.removeAll();
			remove(jScrollPane);
			add(createResultsComponent(rl), new java.awt.GridBagConstraints());
			updateUI();
		}


		public void adjustmentValueChanged(AdjustmentEvent e)
		{
			int scrollValue = jScrollBar.getValue();
			if (scrollValue >= (jScrollBar.getMaximum()*.75))
			{
				try {
					expandTable(scrollValue);
				} catch (IOException ioe) {
					ioe.printStackTrace();
				}
			}
		}
	}







	JTable displayMetaData( Object[][] data, String[] names, MetaDataRecordList[] rl )
	{
		JTable table = new JTable(new MetaDataTableModel(data, names));/* {
				public TableCellEditor getDefaultEditor(Class columnClass) {
//System.out.println("1 "+cellEditor+ " "+columnClass);
					TableCellEditor tce  =  super.getDefaultEditor(columnClass);
//System.out.println("2 "+cellEditor);
//System.out.println("tce "+tce);
					return tce;
				}
			};*/
		TableCellRenderer defaultRenderer =
			table.getDefaultRenderer(JTable.class);
		table.setDefaultRenderer(JTable.class,
			 new MetaDataTableCellRenderer(defaultRenderer));

		//has a filesystem so the metadata is modifiable.
		if (queryObj != null) {
			TableCellEditor defaultEditor =
				table.getDefaultEditor(JTable.class);
			table.setDefaultEditor(JTable.class,
				new MetaDataTableCellEditor(defaultEditor) );
		}
/*
defaultRenderer =
	table.getDefaultRenderer(String.class);
table.setDefaultRenderer(String.class,
	 new MetaDataTableCellRenderer(defaultRenderer));
defaultEditor =
	table.getDefaultEditor(String.class);
table.setDefaultEditor(String.class,
	new MetaDataTableCellEditor(defaultEditor) );
*/

/*
table.getModel().addTableModelListener(new TableModelListener() {
	public void tableChanged(TableModelEvent e) {
System.out.println(" tableChanged "+e);
			int row = e.getFirstRow();
			int column = e.getColumn();
			TableModel model = (TableModel)e.getSource();
			String columnName = model.getColumnName(column);
			Object data = model.getValueAt(row, column);


//System.out.println("%%%%%%%%%% "+data);
	}
});*/

		return table;
	}

//-----------------------------------------------------------------------
// Inner Classes
//-----------------------------------------------------------------------
	class MetaDataTableCellEditor extends AbstractCellEditor
		implements TableCellEditor, ActionListener
	{
		private TableCellEditor defaultEditor;
		Object component;
		String originalValue;
		int currentRow = -1, currentColumn = -1;


		public MetaDataTableCellEditor( TableCellEditor editor )
		{
			super( );

			defaultEditor = editor;

			addCellEditorListener( new CellEditorListener() {
				public void editingCanceled(ChangeEvent e)
				{
//System.out.println("aaaaaaaaaaaG");
				}

				public void editingStopped(ChangeEvent e)
				{
MetaDataRecordList[] tempRL = new MetaDataRecordList[1];
					System.out.println("editingStopped");
try {

if (getCellEditorValue() instanceof JTable) {
System.out.println("JTable");
	JTable table = (JTable)getCellEditorValue();
	int rows = table.getRowCount();
	int columns = table.getColumnCount();

	String[][] temp = new String[rows][columns];
	int[] op = new int[rows];
	Object t = null;
	for (int i=0;i<rows;i++) {
		for (int j=0;j<columns;j++) {
			t = table.getValueAt(i,j);
			if (t != null) {
				temp[i][j] = t.toString();
			}
		}
		op[i] = MetaDataCondition.EQUAL;
	}
	MetaDataTable mdt = new MetaDataTable( op, temp );
	tempRL[0] = new SRBMetaDataRecordList(
		rl[currentRow].getField(currentColumn), mdt );

	queryObj.modifyMetaData(tempRL[0]);
}
else if (getCellEditorValue() instanceof JTextField) {
System.out.println("JTextField "+((JTextField)component).getText());
System.out.println("originalValue "+originalValue);
	JTextField textField = (JTextField)getCellEditorValue();

	String text = textField.getText();
	if (!text.equals(originalValue)) {
System.out.println("rl "+rl);
Object um = rl[currentRow].getValue(currentColumn);
System.out.println("um "+um);
		if (um instanceof MetaDataTable) {
System.out.println("table "+um);

			rl[currentRow].setValue(currentColumn, (MetaDataTable)um);
			tempRL[0] = new SRBMetaDataRecordList(
				rl[currentRow].getField(currentColumn), (MetaDataTable)um );
		}
		else {
			rl[currentRow].setValue(currentColumn, text);
			tempRL[0] = new SRBMetaDataRecordList(
				rl[currentRow].getField(currentColumn), text );
		}

		queryObj.modifyMetaData(tempRL[0]);
	}
}
else if (getCellEditorValue() instanceof String) {
	System.out.println("getCellEditorValue- String "+getCellEditorValue());
}
System.out.println(queryObj +"\n------\n");

} catch( SRBException t ) {
	int foo = t.getType( );	System.out.println( "Standardized SRB Server Error Type: "+ foo );		String bar = t.getStandardMessage( );	System.out.println( "Standardized SRB Server Error Message: "+ bar );		System.out.println( "\nJargon Error Message: "+ t.getMessage() );	t.printStackTrace();
}
catch ( Throwable t ) {
	t.printStackTrace();
}
//					}
				}
			}); //end of add listener


		}

		/**
		 * Finalizes the object by explicitly letting go of each of
		 * its internally held values.
		 */
		protected void finalize( )
			throws Throwable
		{
			super.finalize();

			if (defaultEditor != null)
				defaultEditor = null;

			if (component != null)
				component = null;
		}



		/**
		 * @return the component used for drawing the cell. This method is used to configure the renderer appropriately before drawing.
		 *
		 * @param table the JTable that is asking the renderer to draw; can be null
		 * @param value the value of the cell to be rendered. It is up to the specific renderer to interpret and draw the value. For example, if value is the string "true", it could be rendered as a string or it could be rendered as a check box that is checked. null is a valid value
		 * @param isSelected true if the cell is to be rendered with the selection highlighted; otherwise false
		 * @param hasFocus if true, render cell appropriately. For example, put a special border on the cell, if the cell can be edited, render in the color used to indicate editing
		 * @param row the row index of the cell being drawn. When drawing the header, the value of row is -1
		 * @param column the column index of the cell being drawn
		 */
		public Component getTableCellEditorComponent( JTable table, Object value,
			boolean isSelected, int row, int column )
		{
//System.out.println("inner-MDdis getTableCellEditorComponent "+value.getClass());
			component = value;

			currentRow = row;
			currentColumn = column;

			if (value instanceof String) {
				component = new JTextField( (String)value );
				originalValue = (String)value;
			}
			else {
		    component = defaultEditor.getTableCellEditorComponent(
			  	table, value, isSelected, row, column);
//System.out.println("def");
			}
//return new MetaDataTableCellEditor( defaultEditor, null).getTableCellEditorComponent(
//	  	table, value, isSelected, row, column);

			return (Component) component;
		}

    public void actionPerformed(ActionEvent e) {
//System.out.println(e);
    }

		public Object getCellEditorValue()
		{
//System.out.println("getCellEditorValue "+super.getCellEditorValue());

			return component;
		}

		//TODO what? why does it say I need to override these?
		public void addCellEditorListener(CellEditorListener l)
		{
//System.out.println("addCellEditorListener ");
			super.addCellEditorListener(l);
		}
		public void cancelCellEditing() {
//System.out.println("cancelCellEditing ");
			super.cancelCellEditing();
		}
		public boolean isCellEditable(EventObject anEvent)
		{
//System.out.println("isCellEditable ");
			return super.isCellEditable(anEvent);
		}
		public void removeCellEditorListener(CellEditorListener l)
		{
//System.out.println("removeCellEditorListener ");
			super.removeCellEditorListener(l);
		}
		public boolean shouldSelectCell(EventObject anEvent)
		{
//System.out.println("shouldSelectCell ");
			return super.shouldSelectCell(anEvent);
		}
		public boolean stopCellEditing()
		{
//System.out.println("stopCellEditing ");
			return super.stopCellEditing();
		}

	}



//tabley
	class MetaDataTableCellRenderer implements TableCellRenderer {
	  private TableCellRenderer __defaultRenderer;
	  private TableCellEditor defaultEditor;

	  public MetaDataTableCellRenderer(TableCellRenderer renderer) {
	    __defaultRenderer = renderer;
	  }

		/**
		 * @return the component used for drawing the cell. This method is used to configure the renderer appropriately before drawing.
		 *
		 * @param table the JTable that is asking the renderer to draw; can be null
		 * @param value the value of the cell to be rendered. It is up to the specific renderer to interpret and draw the value. For example, if value is the string "true", it could be rendered as a string or it could be rendered as a check box that is checked. null is a valid value
		 * @param isSelected true if the cell is to be rendered with the selection highlighted; otherwise false
		 * @param hasFocus if true, render cell appropriately. For example, put a special border on the cell, if the cell can be edited, render in the color used to indicate editing
		 * @param row the row index of the cell being drawn. When drawing the header, the value of row is -1
		 * @param column the column index of the cell being drawn
		 */
	  public Component getTableCellRendererComponent(JTable table, Object value,
			boolean isSelected,	boolean hasFocus,	int row, int column)
	  {
	    if(value instanceof JComponent) {
	      return (JComponent)value;
	    }
	    return __defaultRenderer.getTableCellRendererComponent(
		   table, value, isSelected, hasFocus, row, column);
	  }
	}


//MetaDataTableModel
	class MetaDataTableModel extends AbstractTableModel
	{
	  /*private */Object[][] data;

	  private String[] names;

		MetaDataTableModel( Object[][] data, String[] names )
		{
			this.data = data;
			this.names = names;

addTableModelListener(new TableModelListener() {
	public void tableChanged(TableModelEvent e) {
			int row = e.getFirstRow();
			int column = e.getColumn();
			TableModel model = (TableModel)e.getSource();
			String columnName = model.getColumnName(column);
			Object data = model.getValueAt(row, column);


System.out.println("~~~~~~~~~tableChanged "+data);
	}
});
		}


	  public String getColumnName(int column) {
	    return names[column];
	  }

	  public int getRowCount() {
	    return data.length;
	  }

	  public int getColumnCount() {
	    return names.length;
	  }

	  public Object getValueAt(int row, int column) {
while (column >= data[row].length)
column--;
	      return data[row][column];
	  }

	  public boolean isCellEditable(int row, int column) {
	    return true;
	  }

	  public Class getColumnClass(int column) {
while (column >= data[0].length)
column--;
Object obj = 			getValueAt(0, column);
if (obj != null)
		return obj.getClass();
return String.class;
	  }
	}
//end MetaDataTableModel
}

