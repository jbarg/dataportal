package hello;

public class HelloServer{
	public String sayHelloTo( String name ){
		System.out.println( "sayHelloTo( String name )" );
		return "Hello " + name + ". How are you today?";
	}

	public String sayHelloTo( Name theName ){
		System.out.println( "sayHelloTo( Name theName )" );
		return "Hello " + theName.getName() + ". How are you today?";
	}
}
