package xportlets.gridftp;

/*
Any object that wants to be scheduled with the scheduler
needs to implement this interface.
This interface defines the states of the task during its life cycle.
*/

public interface Task 
{
        public static final int WAITING = 0;
        public static final int ONGOING = 1;
        public static final int DONE = 2;
        public static final int FAILED = 3;
        public static final int RESTARTABLE = 4;

        public int getStatus();
        public void setStatus(int status) throws IllegalArgumentException; 
}
