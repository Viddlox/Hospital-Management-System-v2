#ifndef PAGINATOR_H
#define PAGINATOR_H

#include <iostream>
#include <vector>
#include <string>

class Paginator
{
private:
    std::vector<std::string> data; // Dataset
    int pageSize;                  // Number of items per page
    int currentPage;               // Current page index (0-based)
    int totalPages;                // Total number of pages

public:
    Paginator(const std::vector<std::string> &data, int pageSize)
        : data(data), pageSize(pageSize), currentPage(0)
    {
        // Calculate total pages
        totalPages = (data.size() + pageSize - 1) / pageSize;
    }

    // Display the current page
    std::vector<std::string> getCurrentPage() const
    {
        int startIndex = currentPage * pageSize;
        int endIndex = std::min(startIndex + pageSize, static_cast<int>(data.size()));
		std::vector<std::string> res;

        for (int i = startIndex; i < endIndex; i++)
        {
            res.push_back(data[i]);
        }
		return res;
    }

    // Move to the next page
    void nextPage()
    {
        if (currentPage < totalPages - 1)
        {
            currentPage++;
        }
    }

    // Move to the previous page
    void previousPage()
    {
        if (currentPage > 0)
        {
            currentPage--;
        }
    }

    // Move to the first page
    void firstPage()
    {
        currentPage = 0;
    }

    // Move to the last page
    void lastPage()
    {
        currentPage = totalPages - 1;
    }

    // Get the current page index
    int getCurrentPageIndex() const
    {
        return currentPage;
    }

    // Get the total number of pages
    int getTotalPages() const
    {
        return totalPages;
    }
};

#endif