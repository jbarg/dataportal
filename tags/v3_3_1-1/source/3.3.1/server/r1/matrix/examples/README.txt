SDSC Matrix Project
~~~~~~~~~~~~~~~~~~~

This directory contains some examples.The examples illustrate the use of the Data Grid Language (DGL) in the Grid Workflow (GridFlow) Manageemnt


The DGL document can be created using an XML editor like (XML Spy) or Programmatically using the JAVA API or using a Graphical User Interface focussing the end domain user.

Each of these example directories focus on creating DGL Request document using an approach.


The directory "Using-XMLeditor" contains dgl files that were created using XML Spy. Using XML editor like XML Spy makes it easy to create DGL documents at ease. It is very easy if you know a little bit of XML. 
It is highly encouraged to look into DGL schema using an XML Editor to get started. The DGL documents made could be sent to the Matrix server using the java program DGLSender.java. Running this program with no command line arguments will give a message on how to use it. 
One can create the DGL documents using his/her favorite XML Editor and use the DGLSender program to send a request to Matrix. This generates a Request-Id, which is shown on screen. The user can again use the same program and use the Request-Id to enquire status of any DGL Request.

The directory "Using-JavaAPI" contains java programs that illustrate how programmatically a DGL could be developed and sent to the Matrix Server. The "HelloMatrix.java" is a simple program to start with. You may wish to look at the source code. It used the Matrix Client API. 
The Client API docs are available in the website. They can also be created by running the ant command "ant client-docs" in the Matrix home directory (contains build.xml file).

The directory "DGL-Builder-GUI" is supposed to house the customizable GUI for end users to prepare, save and execute DGL requests. The DGL builder ideally could be customized for each project, with domain specific flows. This would be available in version 3.3. 
If you are interested in participating with DGL-Builder (as projects would like this), please e-mail talk2matrix@sdsc.edu. 











Note: XMLSpy is a trademark of Altova GmbH, Germany.