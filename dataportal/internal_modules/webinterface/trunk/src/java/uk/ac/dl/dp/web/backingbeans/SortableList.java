package uk.ac.dl.dp.web.backingbeans;

import java.io.Serializable;

public abstract class SortableList 
{
    private String _sort;
    private boolean _ascending;

    protected SortableList(String defaultSortColumn)
    {
        _sort = defaultSortColumn;
        _ascending = isDefaultAscending(defaultSortColumn);
    }

    /**
     * Sort the list.
     */
    protected abstract void sort(String column, boolean ascending);

    /**
     * Is the default sort direction for the given column "ascending" ?
     */
    protected abstract boolean isDefaultAscending(String sortColumn);


    public void sort(String sortColumn)
    {
        if (sortColumn == null)
        {
            throw new IllegalArgumentException("Argument sortColumn must not be null.");
        }

        if (_sort.equals(sortColumn))
        {
            //current sort equals new sortColumn -> reverse sort order
            _ascending = !_ascending;
        }
        else
        {
            //sort new column in default direction
            _sort = sortColumn;
            _ascending = isDefaultAscending(_sort);
        }

        sort(_sort, _ascending);
    }

    public String getSort()
    {
        return _sort;
    }

    public void setSort(String sort)
    {
        _sort = sort;
    }

    public boolean isAscending()
    {
        return _ascending;
    }

    public void setAscending(boolean ascending)
    {
        _ascending = ascending;
    }
}
