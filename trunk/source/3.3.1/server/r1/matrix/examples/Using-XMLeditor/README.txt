SDSC Matrix Project
~~~~~~~~~~~~~~~~~~~

This directory contains some examples.The examples illustrate the use of the Data Grid Language (DGL) in the Grid Workflow (GridFlow) Management.


The DGL document can be created using an XML editor like (XML Spy) or Programmatically using the JAVA API or using a Graphical User Interface focussing the end domain user.

This directory "Using-XMLeditor", contains dgl files that were created using XML Spy. Using XML editor like XML Spy makes it easy to create DGL documents at ease. It is very easy if you know a little bit of XML. 
It is highly encouraged to look into DGL schema using an XML Editor to get started. The DGL documents made could be sent to the Matrix server using the java program DGLSender.java. Running this program with no command line arguments will give a message on how to use it. 
One can create the DGL documents using his/her favorite XML Editor and use the DGLSender program to send a request to Matrix. This generates a Request-Id, which is shown on screen. The user can again use the same program and use the Request-Id to enquire status of any DGL Request.


To send the DGL requests to Matrix use the program DGLSender.java
1) set Classpath first (in windows you can do that using setMatrixClasspath.bat file present in examples directory)
2) javac DGLSender.java
3) java DGLSender <dgl file name>
4) java DGLSender status:<request id>   (to check status)




Note: XMLSpy is a trademark of Altova GmbH, Germany.