// Demo.java
// Daniel Moore
// 8-22-03
// A program to test the JTree class

import java.awt.*;
import java.awt.event.*;
import java.net.URI;
import javax.swing.*;
import javax.swing.tree.*;


public class Demo extends JPanel{
	private int newNodeSuffix = 1;

	public Demo(JFrame frame) {
	  this(frame,
			"srb://testuser.sdsc:TESTUSER@srb.sdsc.edu:7573/home/testuser.sdsc");
	}


	public Demo(JFrame frame, String uri) {
	    try {
					final FileTree treePanel = new FileTree(new URI("file:/"));
					final FileTree testPanel = new FileTree( new URI( uri ));

					JButton addButton = new JButton("Add");
        	addButton.addActionListener(new ActionListener() {
	            public void actionPerformed(ActionEvent e) {
        	        treePanel.addObject("New Node " + newNodeSuffix++);
	            }
        	});

	        JButton removeButton = new JButton("Remove");
        	removeButton.addActionListener(new ActionListener() {
	            public void actionPerformed(ActionEvent e) {
        	        treePanel.removeCurrentNode();
	            }
        	});

	        JButton clearButton = new JButton("Clear");
        	clearButton.addActionListener(new ActionListener() {
	            public void actionPerformed(ActionEvent e) {
        	        treePanel.clear();
			testPanel.clear();
	            }
        	});

	        JButton peruseButton = new JButton("Peruse");
        	peruseButton.addActionListener(new ActionListener() {
	            public void actionPerformed(ActionEvent e) {
        	        treePanel.peruse();
			testPanel.peruse();
	            }
        	});

	        //Lay everything out.
        	setLayout(new BorderLayout());
	        treePanel.setPreferredSize(new Dimension(400, 300));
		//testPanel.setPreferredSize(new Dimension(400, 300));
        	add(treePanel, BorderLayout.CENTER);
		add(testPanel, BorderLayout.WEST);

	        JPanel panel = new JPanel();
        	panel.setLayout(new GridLayout(0,1));
	        //panel.add(addButton);
        	//panel.add(removeButton);
		panel.add(peruseButton);
	        panel.add(clearButton);
        	add(panel, BorderLayout.EAST);
	    } catch (java.net.URISyntaxException e) {
		System.err.println(e);
		e.printStackTrace();
	    }
	}

	public static void main(String[] args) {
			JFrame frame = new JFrame("Demo");

			Container contentPane = frame.getContentPane();
			contentPane.setLayout(new GridLayout(1,1));
			if (args.length == 1) {
				contentPane.add(new Demo(frame, args[0]));
			}
			else if (args.length == 0) {
				contentPane.add(new Demo(frame));
			}
			else {
				throw new IllegalArgumentException(
					"Wrong number of arguments sent to Demo.\nUsage:\tjava Demo [uriString]\n\tjava Demo");
			}

			frame.addWindowListener(new WindowAdapter() {
					public void windowClosing(WindowEvent e) {
							System.exit(0);
					}
			});

			frame.pack();
			frame.setVisible(true);
	}
}