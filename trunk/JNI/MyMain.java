public class MyMain{

    public MyMain(){
	System.out.println("Hello from Java");
	System.out.println("java.library.path="+System.getProperty("java.library.path"));
	System.out.println("java.class.path="+System.getProperty("java.class.path"));
	new javacog.MyGramTest();
	for ( int i=0; i<1000; i++ ){
		System.out.println( "i="+i );
	}
    }

    public static void main(String args[]){
	new MyMain();
    }
    
}
