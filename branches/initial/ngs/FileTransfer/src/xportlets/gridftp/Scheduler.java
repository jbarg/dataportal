package xportlets.gridftp;

/* 
This is a generic scheduler interface.
The objects to be scheduled are called Tasks.
The strategy used by the scheduler depends on the
particular class that implements the interface.
The strategies could include FIFO, Shortest task first, etc.
*/

public interface Scheduler
{
    // Add a task to the scheduler
    public void add(Task task);

    // Remove all tasks
    public void removeAll();

    // Get the next task to be worked on
    public Task getNext();

    // Total no. of tasks in the scheduler at present
    public int size();

    // Convenience method
    public boolean isEmpty(); 
}
