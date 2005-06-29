//
//	Copyright (c) 2004  San Diego Supercomputer Center (SDSC),
//	University of California, San Diego (UCSD), San Diego, CA, USA.
//
//	Users and possessors of this source code are hereby granted a
//	nonexclusive, royalty-free copyright and design patent license
//	to use this code in individual software.  License is not granted
//	for commercial resale, in whole or in part, without prior written
//	permission from SDSC/UCSD.  This source is provided "AS IS"
//	without express or implied warranty of any kind.
//
//
//  FILE
//	TransferStatusPanel.java	-  edu.sdsc.grid.gui.TransferStatusPanel
//
//  CLASS HIERARCHY
//	javax.swing.JPanel
//	    |
//	    +-edu.sdsc.grid.gui.TransferStatusPanel
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//

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



/**
 * Displays the progress of files being copied. A progress bar will be
 * created for each file to be copied. This bar displays the amount of the file
 * currently copied. A ticker in the bar of that file will also be regularily
 * updated to make the user feel better. That is, due to network lag, sometimes
 * it appears as if the file has stopped copying and the program has froze.
 * The ticker lets the user know if the program has crashed or is just lagging.
 *
 * @author Lucas Ammon Gilbert
 * @see JargonGui
 * @since JARGON1.5
 */
public class TransferStatusPanel extends JPanel implements Runnable
{
//-----------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------
	private final static String SYMBOL_PERCENTAGE = "/";
	private final static String SYMBOL_DIVIDE = "\\";

	/**
	 * How many characters the filename label can hold
	 */
	private final static int FILENAME_WIDTH = 20;

	private final static Font gridFont =
			new Font("courier", Font.PLAIN, 12);

	/**
	 * If the file exists, force it to be overwritten;
	 * if the file cannot be overwritten throw IOException.
	 */
	public final static int OVERWRITE = 2;

	/**
	 * If the file exists, the source file will be appended
	 * to the destination file.
	 */
//TODO append doesn't work...
	private final static int APPEND = 1;

	/**
	 * If the file exists, ignore it and go to the next.
	 */
	public final static int SKIP = 0;



//-----------------------------------------------------------------------
// Fields
//-----------------------------------------------------------------------
	/**
	 * The / or \ symbol in the progress bar. true = SYMBOL_DIVIDE
	 */
	private boolean toggleSymbol = true;

	/**
	 * The number of bytes copied of the current file.
	 * @see <code>index</code>
	 */
	private int bytesCopied = 0;

	/**
	 * This thread contains this object,
	 * and is used to update the progress bar displays.
	 */
	private Thread drawSymbolThread = null;

	private long fileLengths[];

	private int sourcesLength;

	private int index;


	protected GeneralFile[] sources;
	protected GeneralFile[] destinations;

	protected JProgressBar[] progressBars;
	protected JLabel[] fileNameLabel;

	/**
	 * Overwrite code, defaults to <code>SKIP</code>.
	 */
	protected int overwrite = SKIP;

	/**
	 * Default length to make threads sleep.
	 */
	private int sleepLength = 750;



//-----------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------
	/**
	 * Creates a GUI panel to transfer the <code>source</code> file to
	 * the current location on the local filesystem. The <code>destinations</code>
	 * file will have the same name as the source.
	 */
	public TransferStatusPanel( GeneralFile source )
	{
		this.sources = new GeneralFile[1];
		this.sources[0] = source;

		this.destinations = new LocalFile[1];
		this.destinations[0] = new LocalFile( source.getName() );

		initComponents();
	}

	/**
	 * Creates a GUI panel to transfer the <code>sources</code> files to
	 * the current location on the local filesystem. The
	 * <code>destinations</code> files will have the same name as the sources.
	 */
	public TransferStatusPanel( GeneralFile[] sources )
	{
		this.sources = sources;

		this.destinations = new LocalFile[sources.length];
		for (int i=0;i<sources.length;i++) {
			this.destinations[i] = new LocalFile( sources[i].getName() );
		}

		initComponents();
	}

	/**
	 * Creates a GUI panel to transfer the <code>source</code> file to
	 * the <code>destination</code> file.
	 */
	public TransferStatusPanel( GeneralFile source, GeneralFile destination )
	{
		this.sources = new GeneralFile[1];
		this.sources[0] = source;
		this.destinations = new GeneralFile[1];
		this.destinations[0] = destination;

		initComponents();
	}

	/**
	 * Creates a GUI panel to transfer the <code>sources</code> files to
	 * the <code>destinations</code>.
	 */
	public TransferStatusPanel( GeneralFile[] sources,
		GeneralFile[] destinations )
	{
		this.sources = sources;
		this.destinations = destinations;

		initComponents();
	}



//-----------------------------------------------------------------------
// Init & setter/getter Methods
//-----------------------------------------------------------------------
	/**
	 * This method is called from within the constructor to
	 * initialize the form.
	 */
	private void initComponents( )
	{
		sourcesLength = sources.length;
		if (sourcesLength != destinations.length) {
//TODO dir?
			throw new IllegalArgumentException();
		}

		progressBars = new JProgressBar[sourcesLength];
		fileNameLabel = new JLabel[sourcesLength];
		fileLengths = new long[sourcesLength];

		GridBagConstraints gbc = new GridBagConstraints();
		setLayout(new GridBagLayout());

		for (int i=0;i<sourcesLength;i++) {
			String fileName = sources[i].getAbsolutePath();

			this.fileLengths[i] = sources[i].length();
			this.bytesCopied = 0;

			// Set up the progress bar
			progressBars[i] = new JProgressBar();
			progressBars[i].setString("Queued");
			progressBars[i].setStringPainted(true);
			progressBars[i].setMaximum((int)fileLengths[i]);
			progressBars[i].setValue(0);
			progressBars[i].setPreferredSize(new Dimension(150, 20));

			// Before adding the name, check the size of the name so that
			// all the characters will fit.
			if(fileName.length() > FILENAME_WIDTH){
				int removeChars = fileName.length() - FILENAME_WIDTH;
				// The 3 is to add in the ... at the front of the name
				removeChars += 3;
				fileName = fileName.substring(removeChars, fileName.length());
				fileName = "..."+fileName;
			}

			// Create the file name label.
			fileNameLabel[i] = new JLabel(fileName, JLabel.LEFT);
//			fileNameLabel[i].setPreferredSize(new Dimension(160, 20));
//			fileNameLabel[i].setFont(gridFont);
//			fileNameLabel[i].setForeground(Color.white);

			// create this JPanel
			gbc.insets = new Insets(0,5,0,5);
			gbc.fill = GridBagConstraints.BOTH;
			gbc.gridx = 0;
			gbc.gridy = i;

			this.add(fileNameLabel[i], gbc);

			gbc.gridx = 1;
			gbc.gridy = i;

			this.add(progressBars[i], gbc);
		}
	}

	/**
	 * If the file exists:
	 * If overwrite, force it to be overwritten;
	 * if the file cannot be overwritten throw IOException.<br>
	 * If append, the source file will be appended to the destination file.
	 * If skip, ignore the file and go to the next.
	 */
	public void setOverwrite( int overwrite )
	{
		if ((overwrite <= OVERWRITE) && (overwrite >= SKIP)) {
			this.overwrite = overwrite;
		}
	}

	/**
	 * If the file exists:
	 * If overwrite, force it to be overwritten;
	 * if the file cannot be overwritten throw IOException.<br>
	 * If append, the source file will be appended to the destination file.
	 * If skip, ignore the file and go to the next.
	 *
	 * @return the overwrite value.
	 */
	public int getOverwrite( )
	{
		return overwrite;
	}



//TODO if they change a source/dest in the middle of the copy, what happens?
	/**
	 *
	 */
	public GeneralFile[] getSources( )
	{
		return sources;
	}

	/**
	 *
	 */
	public GeneralFile[] getDestinations( )
	{
		return destinations;
	}


	/**
	 * Time that threads will sleep. A longer time will give more
	 * resources to the copy part. But too long and the users tend to think the
	 * copy failed. Min. time 100ms. Default time 750ms.
	 */
	public void setSleepLength( int sleepFor )
	{
		if (sleepFor > 100) {
			sleepLength = sleepFor;
		}
	}

	/**
	 * Time that threads will sleep. A longer time will give more
	 * resources to the copy part. But too long and the users tend to think the
	 * copy failed.
	 */
	public int getSleepLength()
	{
		return sleepLength;
	}

//TODO
		boolean skipFile = false;
//-----------------------------------------------------------------------
// Methods
//-----------------------------------------------------------------------
//TODO? should be run()
	/**
	 *
	 */
	public void start()
		throws IOException
	{
		SwingUtilities.invokeLater(
			new Runnable()
			{
				public void run(){
					startSymbolThread();
				}
			}
		);

		copy( sources, destinations );
	}

	private void copy( final GeneralFile[] sources,
		final GeneralFile[] destinations )
		throws IOException
	{
		Thread copyThread;
		for (index=0;index<sourcesLength;index++)
		{
			if ( (overwrite == OVERWRITE) ||
					 (overwrite == APPEND) ||
					 (!destinations[index].exists()) )
			{
//TODO appending existing files isn't working?
				//create a thread to copy the files.
				copyThread = new Thread(
					new Runnable() {
						public void run() {
							try {
								if (overwrite == OVERWRITE)
									sources[index].copyTo(destinations[index], true);
								else if (overwrite == APPEND)
									sources[index].copyTo(destinations[index], false);
								else if (!destinations[index].exists())
									sources[index].copyTo(destinations[index], false);
								else {
									//file exists skip it, code should never reach here?
									skipFile = true;
								}
							} catch(SRBException e) {
								JOptionPane.showMessageDialog(null, e+" "+
									e.getStandardMessage());
							} catch(IOException e) {
								JOptionPane.showMessageDialog(null, ""+e);
							}
						}
					},
				"Copy");
				copyThread.start();

				//TODO not way to check for other General
				if (sources[index] instanceof SRBFile)
				{
					while (((int)((SRBFile)
						sources[index]).fileCopyStatus() < fileLengths[index]) &&
						copyThread.isAlive())
					{
						bytesCopied = (int)((SRBFile) sources[index]).fileCopyStatus();
						try{
							Thread.sleep(sleepLength);
						} catch(InterruptedException e) {
							JOptionPane.showMessageDialog(null, e);
						}
					}
				}
				else if (destinations[index] instanceof SRBFile)
				{
					while (((int)((SRBFile)
						destinations[index]).fileCopyStatus() < fileLengths[index]) &&
						copyThread.isAlive())
					{
						bytesCopied = (int)((SRBFile) destinations[index]).fileCopyStatus();
						try{
							Thread.sleep(sleepLength);
						} catch(InterruptedException e) {
							JOptionPane.showMessageDialog(null, e);
						}
					}
				}

				bytesCopied = (int)fileLengths[index];

				//make the numbers match when done, eg. show 100% complete, not 99.8%
				progressBars[index].setString("     "+bytesCopied+" "+
					SYMBOL_PERCENTAGE+" "+fileLengths[index]);
				progressBars[index].setValue(bytesCopied);
				progressBars[index].paintImmediately(
					progressBars[index].getVisibleRect());


				try {
					copyThread.join();
				} catch(InterruptedException e) {
					JOptionPane.showMessageDialog(null, ""+e);
				}
			}
			else if (destinations[index].isDirectory()) {
				//copy all the sources that don't already exist
//ok maybe I'm being kind of lazy
					GeneralFile[] subSources = { sources[index] };
					GeneralFile[] subDestinations = {
						FileFactory.newFile(destinations[index], sources[index].getName())
					};
					copy( subSources, subDestinations );
			}
			else {
				progressBars[index].setString("Skipped");
			}
		}
	}

	private void startSymbolThread()
	{
		if(drawSymbolThread == null) {
			drawSymbolThread = new Thread(this, "ProgressBars");
			drawSymbolThread.setPriority(Thread.MIN_PRIORITY);
			drawSymbolThread.start();
		}
	}

	/**
	 * This is to help show that the application has not frozen
	 */
	public void run()
	{
		while(true) {
			try {
				drawSymbolThread.sleep(sleepLength);
			} catch(InterruptedException e) {
				JOptionPane.showMessageDialog(null, ""+e);
//e.printStackTrace();
			}

			synchronized (this) {
				if(index >= fileLengths.length)
					break;

				if(toggleSymbol) {
					progressBars[index].setString("     "+bytesCopied+" "+
							SYMBOL_DIVIDE+" "+fileLengths[index]);
					toggleSymbol = false;
				}
				else {
					progressBars[index].setString("     "+bytesCopied+" "+
							SYMBOL_PERCENTAGE+" "+fileLengths[index]);
					toggleSymbol = true;
				}
				progressBars[index].setValue(bytesCopied);
				progressBars[index].paintImmediately(
					progressBars[index].getVisibleRect());
			}
		}
	}






//-----------------------------------------------------------------------
// Testing
//-----------------------------------------------------------------------
	/**
	 * Test method
	 */
	public static void main(String[] args)
	{
		JFrame frame = new JFrame("Test Panel");
		GeneralFile[] sources = new GeneralFile[2];
		GeneralFile[] destinations = new GeneralFile[2];
		SRBFileSystem fs = null;

		try {
			fs = new SRBFileSystem();
			sources[1] = new LocalFile( "Test.class" );
			sources[0] = new SRBFile( fs, "Test.class" );
		} catch (Throwable e) {
			e.printStackTrace();
		}
		try {
			destinations[1] = new SRBFile( fs, "TransferStatusPanel_SRB_Test" );
			destinations[0] = new LocalFile( "TransferStatusPanel_Local_Test" );
		} catch (Throwable e) {
			e.printStackTrace();
		}

		final TransferStatusPanel self = new TransferStatusPanel(
			sources, destinations );
		frame.getContentPane().add( "Center", self );
		frame.setResizable(true);
		frame.pack();
		frame.show();

		try {
			self.start();
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}
}
