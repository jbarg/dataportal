package xportlets.gridftp;

import java.util.LinkedList;

/* 
This class is a First In First Out (FIFO) scheduler.
In addition to the methods defined by the Scheduler interface,
it allows for a particular waiting task to be removed or modified.
It also allows for shuffling of waiting tasks.

It maintains and allows access to a history of the tasks 
that have already been scheduled.

It uses the LinkedList class as its back-end.
*/

public class FIFOscheduler implements Scheduler
{
    private LinkedList list;

    // headPosition denotes the next element to be scheduled
    private int headPosition;
    
    public FIFOscheduler()
    {
        list = new LinkedList();
        headPosition = 0;
    }
    
    public synchronized void add(Task task)
    {
        list.add(task);
	
	// If there is a consumer thread waiting for 
	// something to be put in the list, then wake it up.
	if((list.size() - headPosition) == 1)
		notifyAll();
    }
    
    // The index has to consider the history tasks
    public synchronized Task remove(int index) throws IndexOutOfBoundsException
    {
        return (Task)(list.remove(index));
    }
    
    public synchronized void removeAll()
    {
        list.clear();
    }
    
    public synchronized void removeHistory()
    {
        try {
        for(int i=0; i<headPosition; i++)
            list.remove(i);
        }catch(IndexOutOfBoundsException e) { /* Should not happen */ }
    }
    
    // The positions i and j have to consider the history tasks
    public synchronized void exchange(int i, int j) throws IndexOutOfBoundsException
    {
        if(i >= headPosition && j >= headPosition && i != j)
            // Cannot mess with history items,
            // and nothing to do if i=j
            {
               Task temp = (Task)(list.get(j));
               list.set(j, list.get(i));
               list.set(i, temp);
            }
    }
    
    public synchronized boolean isHistory(int index) throws IndexOutOfBoundsException
    {
        if(index >= list.size())
            throw (new IndexOutOfBoundsException());
        return (index < headPosition);
    }
   
    // Blocks until there is something to be returned. 
    public synchronized Task getNext()
    {
        while (headPosition >= list.size())
	{
		try 
		{
            		// wait for Producer to put value
            		wait();
        	} catch (InterruptedException e) {}
        }

        headPosition++;
        try{
                return (Task)(list.get(headPosition - 1));
            }catch(IndexOutOfBoundsException ioobe) 
            { 
                // This should not happen
                return null;  
            }
    }
    
    // The index has to consider the history tasks
    public synchronized Task get(int index) throws IndexOutOfBoundsException
    {
        return (Task)(list.get(index));
    }
    
    // The index has to consider the history tasks
    public synchronized void modify(int index, Task newTask)
        throws IndexOutOfBoundsException
    {
        list.set(index, newTask);
    }
    
    // Returns the size of tasks in the scheduler
    // This also includes the history tasks
    public synchronized int size()
    {
        return list.size();
    }

    public synchronized boolean isEmpty()
    {
        return (list.size() == 0);
    }
}
